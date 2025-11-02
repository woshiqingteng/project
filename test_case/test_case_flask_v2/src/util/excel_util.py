"""
Excel工具模块
提供Excel文件读取和处理功能
"""

import pandas as pd
from typing import Dict, List, Optional

from .logger_util import get_logger

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
        """从Excel文件读取指定表格并进行处理"""
        try:
            excel_file = pd.ExcelFile(file_path, engine='openpyxl')
            available_sheets = excel_file.sheet_names
            
            valid_sheets = [name for name in sheet_names if name in available_sheets]
            if not valid_sheets:
                valid_sheets = available_sheets
            
            all_data = {}
            for sheet_name in valid_sheets:
                df = ExcelHelper._process_sheet(file_path, sheet_name, header_rows, data_start_row, column_range)
                if df is not None and not df.empty:
                    all_data[sheet_name] = df
                    logger.info(f"已处理表格 '{sheet_name}': {len(df)} 行, {len(df.columns)} 列")
                else:
                    logger.warning(f"表格 '{sheet_name}' 无数据或处理失败")
            
            if not all_data:
                raise ValueError("所有表格都无有效数据")
                
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
        try:
            df_raw = pd.read_excel(file_path, sheet_name=sheet_name, header=None, engine='openpyxl')
            
            if df_raw.empty:
                logger.warning(f"表格 '{sheet_name}' 为空")
                return pd.DataFrame()
            
            if column_range and len(column_range) == 2:
                start_col = max(0, column_range[0] - 1)
                end_col = min(df_raw.shape[1], column_range[1])
                if start_col < end_col:
                    df_raw = df_raw.iloc[:, start_col:end_col]
            
            df_filled = df_raw.ffill(axis=0)
            headers = ExcelHelper._extract_headers(df_filled, header_rows)
            
            if df_filled.shape[0] >= data_start_row:
                data_df = df_filled.iloc[data_start_row-1:, :].copy()
                data_df.columns = headers
                
                data_df = data_df.dropna(how='all')
                
                if not data_df.empty:
                    data_df = data_df.reset_index(drop=True)
                    return data_df
                else:
                    logger.warning(f"表格 '{sheet_name}' 数据部分全为空")
                    return pd.DataFrame(columns=headers)
            else:
                logger.warning(f"表格 '{sheet_name}' 行数不足")
                return pd.DataFrame(columns=headers)
                
        except Exception as e:
            logger.error(f"处理表格 '{sheet_name}' 失败: {e}")
            return pd.DataFrame()
    
    @staticmethod
    def _extract_headers(df: pd.DataFrame, header_rows: int) -> List[str]:
        """从DataFrame提取列表头"""
        headers = []
        
        for col in range(df.shape[1]):
            header_parts = []
            for row in range(min(header_rows, df.shape[0])):
                cell_value = df.iloc[row, col]
                if pd.notna(cell_value) and str(cell_value).strip():
                    header_parts.append(str(cell_value).strip())
            
            if header_parts:
                header_name = ' - '.join(header_parts)
            else:
                header_name = f"列_{col+1}"
            
            base_name = header_name
            counter = 1
            while header_name in headers:
                header_name = f"{base_name}_{counter}"
                counter += 1
                
            headers.append(header_name)
        
        return headers