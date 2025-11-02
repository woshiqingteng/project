"""
Excel工具模块
提供Excel文件读取和处理功能
"""

import pandas as pd
from typing import Dict, List, Optional

from .logger import get_logger


logger = get_logger(__name__)


class ExcelHelper:
    """Excel文件操作辅助类"""
    
    @staticmethod
    def read_excel(
        file_path: str,
        sheet_names: List[str],
        header_rows: int,
        data_start_row: int,
        column_range: Optional[List[int]] = None
    ) -> Dict[str, pd.DataFrame]:
        """从Excel文件读取指定表格并进行处理
        
        Args:
            file_path: Excel文件路径
            sheet_names: 要读取的表格名称列表
            header_rows: 表头行数
            data_start_row: 数据起始行
            column_range: 可选的列范围 [开始, 结束]
            
        Returns:
            映射表名到DataFrame的字典
            
        Raises:
            ValueError: 如果文件无法读取或表格未找到
        """
        try:
            excel_file = pd.ExcelFile(file_path, engine='openpyxl')
            available_sheets = excel_file.sheet_names
            
            valid_sheets = [name for name in sheet_names if name in available_sheets]
            if not valid_sheets:
                raise ValueError(f"未找到有效表格。请求的: {sheet_names}, 可用的: {available_sheets}")
            
            all_data = {}
            for sheet_name in valid_sheets:
                df = ExcelHelper._process_sheet(file_path, sheet_name, header_rows, data_start_row, column_range)
                all_data[sheet_name] = df
                logger.info(f"已处理表格 '{sheet_name}': {len(df)} 行")
            
            return all_data
            
        except Exception as e:
            logger.error(f"Excel读取失败: {e}")
            raise
    
    @staticmethod
    def _process_sheet(
        file_path: str,
        sheet_name: str,
        header_rows: int,
        data_start_row: int,
        column_range: Optional[List[int]]
    ) -> pd.DataFrame:
        """使用格式化和筛选处理单个表格"""
        # 读取原始数据
        df_raw = pd.read_excel(file_path, sheet_name=sheet_name, header=None, engine='openpyxl')
        
        # 应用列范围筛选器
        if column_range and len(column_range) == 2:
            start_col = max(0, column_range[0] - 1)
            end_col = min(df_raw.shape[1], column_range[1])
            df_raw = df_raw.iloc[:, start_col:end_col]
        
        # 填充缺失值并提取表头
        df_filled = df_raw.ffill(axis=0)
        headers = ExcelHelper._extract_headers(df_filled, header_rows)
        
        # 提取数据部分
        if df_filled.shape[0] >= data_start_row:
            data_df = df_filled.iloc[data_start_row-1:, :].copy()
            data_df.columns = headers
            return data_df.dropna(how='all').reset_index(drop=True)
        else:
            logger.warning(f"表格 '{sheet_name}' 行数不足")
            return pd.DataFrame(columns=headers)
    
    @staticmethod
    def _extract_headers(df: pd.DataFrame, header_rows: int) -> List[str]:
        """从DataFrame提取列表头"""
        headers = []
        for col in range(df.shape[1]):
            cell_value = df.iloc[0, col]
            header_name = str(cell_value).strip() if pd.notna(cell_value) else f"列_{col+1}"
            headers.append(header_name)
        return headers