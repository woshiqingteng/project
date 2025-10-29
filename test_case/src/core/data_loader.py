from pathlib import Path
from typing import List, Dict, Any
import pandas as pd
import warnings
from src.util.logging_util import get_logger
from src.util.excel_util import ExcelProcessor

# 使用模块级日志记录器
logger = get_logger(__name__)

class ExcelDataLoader:
    """Excel数据加载器"""
    
    def __init__(self, settings):
        self.settings = settings
        self.header_rows = settings.get_config_value("input_excel_processing.header_rows")
        self.data_start_row = settings.get_config_value("input_excel_processing.data_start_row")
        self.target_sheets = settings.get_config_value("input_excel_processing.target_sheets")
        self.column_range = settings.get_config_value("input_excel_processing.column_range")
    
    def load_data(self, file_path: Path) -> Dict[str, List[Dict[str, Any]]]:
        """加载Excel数据"""
        try:
            # 记录开始加载数据的日志 - 只在这里记录一次
            logger.info(f"开始加载数据: {file_path}")
                
            data_frames = ExcelProcessor.read_excel_with_sheets(
                str(file_path), 
                self.target_sheets,
                self.header_rows, 
                self.data_start_row,
                self.column_range
            )
            
            # 转换为字典记录
            data_records = {}
            for sheet_name, df in data_frames.items():
                # 抑制DataFrame列重复的警告
                with warnings.catch_warnings():
                    warnings.filterwarnings("ignore", message="DataFrame columns are not unique, some columns will be omitted.")
                    records = df.to_dict('records')
                
                if self._validate_data(records):
                    data_records[sheet_name] = records
                    logger.info(f"[表格 {sheet_name}] 数据加载完成，共 {len(records)} 条记录")
                else:
                    logger.warning(f"表格 '{sheet_name}' 数据验证失败")
            
            if not data_records:
                logger.warning("没有加载到任何有效数据")
                
            return data_records
                
        except Exception as e:
            logger.error(f"加载数据失败: {e}")
            raise
    
    def _validate_data(self, data: List[Dict[str, Any]]) -> bool:
        """验证数据完整性"""
        if not data:
            logger.warning("数据为空")
            return False
        
        # 检查是否有有效数据
        valid_records = 0
        for record in data:
            if any(value is not None and str(value).strip() != '' for value in record.values()):
                valid_records += 1
        
        if valid_records == 0:
            logger.warning("没有有效数据")
            return False
        
        return True

class DataLoaderFactory:
    """数据加载器工厂"""
    
    @staticmethod
    def create_data_loader(loader_type: str = "excel", settings=None, **kwargs):
        """创建数据加载器"""
        if loader_type == "excel":
            return ExcelDataLoader(settings=settings, **kwargs)
        else:
            raise ValueError(f"不支持的加载器类型: {loader_type}")