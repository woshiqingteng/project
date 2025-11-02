"""
数据加载模块
处理Excel数据的加载和验证
"""

from pathlib import Path
from typing import Any, Dict, List

import pandas as pd
import warnings

from ..util.excel_helper import ExcelHelper
from ..util.logger import get_logger


logger = get_logger(__name__)


class ExcelDataLoader:
    """带有验证的Excel数据加载器"""
    
    def __init__(self, settings):
        """使用配置初始化加载器"""
        self._settings = settings
        self._header_rows = settings.get("input_excel_processing.header_rows")
        self._data_start_row = settings.get("input_excel_processing.data_start_row")
        self._target_sheets = settings.get("input_excel_processing.target_sheets")
        self._column_range = settings.get("input_excel_processing.column_range")
    
    def load(self, file_path: Path) -> Dict[str, List[Dict[str, Any]]]:
        """从Excel文件加载数据
        
        Args:
            file_path: Excel文件路径
            
        Returns:
            映射表名到数据记录的字典
            
        Raises:
            FileNotFoundError: 如果文件不存在
            ValueError: 如果数据验证失败
        """
        if not file_path.exists():
            raise FileNotFoundError(f"输入文件不存在: {file_path}")
        
        logger.info(f"加载数据: {file_path}")
        
        data_frames = ExcelHelper.read_excel(
            str(file_path),
            self._target_sheets,
            self._header_rows,
            self._data_start_row,
            self._column_range
        )
        
        return self._process_data_frames(data_frames)
    
    def _process_data_frames(self, data_frames: Dict[str, pd.DataFrame]) -> Dict[str, List[Dict[str, Any]]]:
        """将DataFrame转换为字典记录并进行验证"""
        data_records = {}
        
        for sheet_name, df in data_frames.items():
            with warnings.catch_warnings():
                warnings.filterwarnings("ignore", 
                    message="DataFrame columns are not unique, some columns will be omitted.")
                records = df.to_dict('records')
            
            if self._validate_records(records):
                data_records[sheet_name] = records
                logger.info(f"[表格 {sheet_name}] 加载了 {len(records)} 条记录")
            else:
                raise ValueError(f"表格数据验证失败: {sheet_name}")
        
        return data_records
    
    def _validate_records(self, records: List[Dict[str, Any]]) -> bool:
        """验证数据记录是否有意义的内容"""
        if not records:
            logger.warning("未找到记录")
            return False
        
        valid_count = sum(
            1 for record in records 
            if any(value is not None and str(value).strip() for value in record.values())
        )
        
        if valid_count == 0:
            logger.warning("未找到有效数据")
            return False
        
        return True


class DataLoaderFactory:
    """数据加载器工厂"""
    
    @staticmethod
    def create(loader_type: str = "excel", settings=None, **kwargs):
        """创建数据加载器实例
        
        Args:
            loader_type: 要创建的加载器类型
            settings: 配置设置
            **kwargs: 附加参数
            
        Returns:
            数据加载器实例
            
        Raises:
            ValueError: 如果加载器类型不受支持
        """
        if loader_type == "excel":
            return ExcelDataLoader(settings=settings, **kwargs)
        else:
            raise ValueError(f"不支持的加载器类型: {loader_type}")