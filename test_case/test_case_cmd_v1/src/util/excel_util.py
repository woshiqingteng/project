import pandas as pd
from typing import List, Dict, Any
from src.util.logging_util import get_logger
logger = get_logger(__name__)
class ExcelProcessor:
    """Excel文件处理器"""
    @staticmethod
    def read_excel_with_sheets(file_path: str, sheet_names: List[str], header_rows: int, data_start_row: int, column_range: List[int] = None):
        """
        读取指定sheet的Excel文件
        """
        try:
            all_data = {}
            # 读取整个Excel文件
            excel_file = pd.ExcelFile(file_path)
            available_sheets = excel_file.sheet_names
            # 检查请求的sheet是否存在
            valid_sheets = []
            for sheet_name in sheet_names:
                if sheet_name in available_sheets:
                    valid_sheets.append(sheet_name)
                else:
                    logger.warning(f"Sheet '{sheet_name}' 不存在，跳过")
            if not valid_sheets:
                raise ValueError("没有找到有效的sheet")
            # 按原始顺序处理每个sheet
            for sheet_name in valid_sheets:
                # 读取整个sheet，不设置列名
                df_raw = pd.read_excel(file_path, sheet_name=sheet_name, header=None)
                # 应用列范围过滤
                if column_range and len(column_range) == 2:
                    start_col = max(0, column_range[0] - 1)  # 转换为0-based索引
                    end_col = min(df_raw.shape[1], column_range[1])
                    df_raw = df_raw.iloc[:, start_col:end_col]
                # 对整个数据框进行垂直方向的向下填充
                df_filled = df_raw.ffill(axis=0)
                # 提取列标题（使用第1行，填充后的）
                headers = []
                for col in range(df_filled.shape[1]):
                    cell_value = df_filled.iloc[0, col]
                    header_name = cell_value if pd.notna(cell_value) else f"列_{col+1}"
                    headers.append(header_name)
                # 提取数据部分（从data_start_row开始）
                if df_filled.shape[0] >= data_start_row:
                    data_df = df_filled.iloc[data_start_row-1:, :].copy()
                    data_df.columns = headers
                    # 移除完全为空的行
                    data_df = data_df.dropna(how='all')
                    # 重置索引
                    data_df.reset_index(drop=True, inplace=True)
                    all_data[sheet_name] = data_df
                else:
                    logger.warning(f"Sheet '{sheet_name}' 数据行数不足")
                    all_data[sheet_name] = pd.DataFrame(columns=headers)
            return all_data
        except Exception as e:
            logger.error(f"读取Excel失败: {e}")
            raise
