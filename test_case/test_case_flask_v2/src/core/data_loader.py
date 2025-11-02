"""
数据加载模块
处理Excel数据的加载和验证
"""

from pathlib import Path
from typing import Any, Dict, List
import pandas as pd
import warnings

from .interface import IDataLoader
from .exception import FileOperationException, ValidationException
from ..config.setting import get_config
from ..util.excel_util import ExcelHelper
from ..util.logger_util import get_logger

logger = get_logger(__name__)

class ExcelDataLoader(IDataLoader):
    """带有验证的Excel数据加载器"""
    
    def __init__(self):
        self._config = get_config()
        processing_config = self._config.get_processing_config()
        
        self._header_rows = processing_config.get("header_rows", 2)
        self._data_start_row = processing_config.get("data_start_row", 3)
        self._target_sheets = processing_config.get("target_sheets", [])
        self._column_range = processing_config.get("column_range", [1, 4])
    
    def load(self, file_path: Path) -> Dict[str, List[Dict[str, Any]]]:
        if not file_path.exists():
            raise FileOperationException(f"输入文件不存在: {file_path}")
        
        logger.info(f"加载数据: {file_path}")
        
        try:
            data_frames = ExcelHelper.read_excel(
                str(file_path),
                self._target_sheets,
                self._header_rows,
                self._data_start_row,
                self._column_range
            )
            
            return self._process_data_frames(data_frames)
            
        except Exception as e:
            logger.error(f"加载Excel文件失败: {e}")
            raise FileOperationException(f"加载Excel文件失败: {e}")
    
    def _process_data_frames(self, data_frames: Dict[str, pd.DataFrame]) -> Dict[str, List[Dict[str, Any]]]:
        data_records = {}
        
        for sheet_name, df in data_frames.items():
            df_cleaned = self._clean_dataframe(df)
            
            with warnings.catch_warnings():
                warnings.filterwarnings("ignore")
                records = df_cleaned.to_dict('records')
            
            if self._validate_records(records, sheet_name):
                data_records[sheet_name] = records
                logger.info(f"[表格 {sheet_name}] 加载了 {len(records)} 条记录")
            else:
                raise ValidationException(f"表格数据验证失败: {sheet_name}")
        
        return data_records
    
    def _clean_dataframe(self, df: pd.DataFrame) -> pd.DataFrame:
        df_clean = df.copy()
        
        new_columns = []
        for i, col in enumerate(df_clean.columns):
            if pd.isna(col) or col == '':
                new_columns.append(f"Column_{i+1}")
            else:
                col_str = str(col).strip()
                if col_str in new_columns:
                    col_str = f"{col_str}_{i+1}"
                new_columns.append(col_str)
        
        df_clean.columns = new_columns
        df_clean = df_clean.fillna('')
        
        return df_clean
    
    def _validate_records(self, records: List[Dict[str, Any]], sheet_name: str) -> bool:
        if not records:
            logger.warning(f"表格 {sheet_name} 未找到记录")
            return False
        
        valid_count = 0
        for record in records:
            has_valid_data = False
            for value in record.values():
                if value is not None and str(value).strip():
                    has_valid_data = True
                    break
            
            if has_valid_data:
                valid_count += 1
        
        if valid_count == 0:
            logger.warning(f"表格 {sheet_name} 未找到有效数据")
            return False
        
        logger.info(f"表格 {sheet_name} 有效记录数: {valid_count}/{len(records)}")
        return True