"""
数据处理模块
处理AI驱动的测试用例生成和解析
"""

import re
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import Any, Dict, List

from .interface import IDataProcessor
from .exception import DataProcessingException
from ..config.setting import get_config
from ..util.logger_util import get_logger

logger = get_logger(__name__)

class OutputParser:
    """AI生成的测试用例输出解析器"""
    
    @staticmethod
    def _clean_text(text: str) -> str:
        """通过删除多余空白来清理和格式化文本"""
        if not text:
            return ""
        lines = [line.strip() for line in text.split('\n') if line.strip()]
        return '\n'.join(lines)
    
    @staticmethod
    def _extract_numbered_items(text: str) -> str:
        """从文本中提取编号列表项"""
        if not text:
            return ""
        items = []
        for line in text.split('\n'):
            match = re.match(r'^\s*(\d+)\.\s*(.+)$', line.strip())
            if match:
                items.append(f"{match.group(1)}. {match.group(2)}")
        return '\n'.join(items) if items else text.strip()
    
    @staticmethod
    def parse_test_cases(ai_output: str) -> List[Dict[str, str]]:
        """将AI输出解析为结构化的测试用例"""
        test_cases = []
        if not ai_output or not ai_output.strip():
            return test_cases
            
        try:
            case_blocks = re.split(r'(?=测试点：)', ai_output.strip())
            
            for block in case_blocks:
                if not block.strip():
                    continue
                
                case = OutputParser._parse_single_case(block)
                if any(case.values()):
                    test_cases.append(case)
            
            return test_cases
            
        except Exception as e:
            logger.error(f"解析测试用例失败: {e}")
            return []
    
    @staticmethod
    def _parse_single_case(block: str) -> Dict[str, str]:
        """解析单个测试用例块"""
        return {
            "测试点": OutputParser._extract_field(block, r'测试点：\s*(.+)'),
            "测试点编号": OutputParser._extract_field(block, r'测试点编号：\s*(.+)'),
            "测试点描述": OutputParser._extract_field(block, r'测试点描述：\s*(.+)'),
            "前置条件": OutputParser._clean_text(
                OutputParser._extract_field(block, r'前置条件：\s*(.+?)(?=测试步骤：|$)', re.DOTALL)
            ),
            "测试步骤": OutputParser._extract_numbered_items(
                OutputParser._extract_field(block, r'测试步骤：\s*(.+?)(?=预期结果：|$)', re.DOTALL)
            ),
            "预期结果": OutputParser._extract_numbered_items(
                OutputParser._extract_field(block, r'预期结果：\s*(.+?)(?=\n\n测试点：|\n\n$|\Z)', re.DOTALL)
            )
        }
    
    @staticmethod
    def _extract_field(block: str, pattern: str, flags: int = 0) -> str:
        """使用正则表达式提取字段值"""
        match = re.search(pattern, block, flags)
        return match.group(1).strip() if match else ""

class DataProcessor(IDataProcessor):
    """用于生成测试用例的主要数据处理器"""
    
    def __init__(self, llm_client, prompt_manager):
        """使用依赖项初始化处理器"""
        self._llm_client = llm_client
        self._prompt_manager = prompt_manager
        self._parser = OutputParser()
        
        # 从配置获取线程数
        config = get_config()
        processing_config = config.get_processing_config()
        self._thread_count = processing_config.get("default_threads", 4)
    
    def process_batch(self, items: List[Dict[str, Any]], sheet_name: str) -> List[Dict[str, Any]]:
        """并行处理数据项批次"""
        start_time = time.time()
        logger.info(f"[表格 {sheet_name}] 使用 {self._thread_count} 个线程处理 {len(items)} 个数据项")
        
        if not items:
            logger.warning(f"[表格 {sheet_name}] 没有数据项需要处理")
            return []
        
        all_results = []
        try:
            if self._thread_count > 1:
                all_results = self._process_concurrent(items, sheet_name)
            else:
                all_results = self._process_sequential(items, sheet_name)
            
            # 按原始行号排序
            sorted_results = sorted(all_results, key=lambda x: x.get("原始行号", 0))
            
            elapsed = time.time() - start_time
            logger.info(f"[表格 {sheet_name}] 在 {elapsed:.2f}秒内处理了 {len(sorted_results)} 个测试用例")
            
            return sorted_results
            
        except Exception as e:
            raise DataProcessingException(f"处理数据批次失败: {e}")
    
    def _process_concurrent(self, items: List[Dict[str, Any]], sheet_name: str) -> List[Dict[str, Any]]:
        """并发处理数据项"""
        all_results = []
        
        with ThreadPoolExecutor(max_workers=self._thread_count) as executor:
            futures = []
            row_indices = []
            
            for idx, item in enumerate(items):
                row_idx = idx + 1
                # 深度清理数据，确保没有不可哈希的类型
                cleaned_item = self._deep_clean_data(item)
                future = executor.submit(self._process_single, row_idx, cleaned_item, sheet_name)
                futures.append(future)
                row_indices.append(row_idx)
            
            for i, future in enumerate(as_completed(futures)):
                try:
                    result = future.result()
                    all_results.extend(result)
                except Exception as e:
                    logger.error(f"处理失败: {e}")
                    row_idx = row_indices[i]
                    all_results.append(self._create_empty_case(row_idx))
        
        return all_results
    
    def _process_sequential(self, items: List[Dict[str, Any]], sheet_name: str) -> List[Dict[str, Any]]:
        """顺序处理数据项"""
        all_results = []
        
        for idx, item in enumerate(items):
            row_idx = idx + 1
            try:
                cleaned_item = self._deep_clean_data(item)
                result = self._process_single(row_idx, cleaned_item, sheet_name)
                all_results.extend(result)
            except Exception as e:
                logger.error(f"处理行 {row_idx} 失败: {e}")
                all_results.append(self._create_empty_case(row_idx))
        
        return all_results
    
    def _deep_clean_data(self, data: Any) -> Any:
        """深度清理数据，确保所有值都是可哈希的基本类型"""
        if isinstance(data, dict):
            cleaned_dict = {}
            for key, value in data.items():
                key_str = str(key) if key is not None else "Unknown"
                cleaned_dict[key_str] = self._deep_clean_data(value)
            return cleaned_dict
        elif isinstance(data, list):
            return [self._deep_clean_data(item) for item in data]
        elif data is None:
            return ""
        elif isinstance(data, (int, float, bool, str)):
            return data
        else:
            return str(data)
    
    def _process_single(self, row_idx: int, row_data: Dict[str, Any], sheet_name: str) -> List[Dict[str, Any]]:
        """处理单行数据"""
        try:
            logger.info(f"[表格 {sheet_name}] [行 #{row_idx}] 开始处理")
            
            test_point_input = self._prepare_input(row_data)
            
            if not test_point_input.strip():
                logger.info(f"[表格 {sheet_name}] [行 #{row_idx}] 数据为空，跳过")
                return []
            
            # 生成测试点
            test_case_input = self._generate_test_points(test_point_input, row_idx, sheet_name)
            
            # 生成测试用例
            test_case_output = self._generate_test_cases(test_case_input, test_point_input, row_idx, sheet_name)
            
            # 解析结果
            parsed_results = self._parser.parse_test_cases(test_case_output)
            valid_results = [result for result in parsed_results if any(result.values())]
            
            if valid_results:
                logger.info(f"[表格 {sheet_name}] [行 #{row_idx}] 生成了 {len(valid_results)} 个测试用例")
            else:
                logger.info(f"[表格 {sheet_name}] [行 #{row_idx}] 未生成有效测试用例")
            
            return [{"原始行号": row_idx, **result} for result in valid_results]
            
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_idx}] 处理失败: {e}")
            return [self._create_empty_case(row_idx)]
    
    def _prepare_input(self, item: Dict[str, Any]) -> str:
        """从数据项准备测试点输入"""
        parts = []
        last_value = None
        
        for key, value in item.items():
            if value is not None and str(value).strip():
                parts.append(f"{key}：{value}")
                last_value = value
        
        return str(last_value).strip() if last_value is not None else ""
    
    def _generate_test_points(self, test_point_input: str, row_idx: int, sheet_name: str) -> str:
        """使用AI生成测试点"""
        try:
            prompt = self._prompt_manager.get_prompt("test_point", {"test_point_input": test_point_input})
            response = self._llm_client.invoke(prompt)
            return response
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_idx}] 测试点生成失败: {e}")
            return ""
    
    def _generate_test_cases(self, test_case_input: str, test_point_input: str, row_idx: int, sheet_name: str) -> str:
        """使用AI生成测试用例"""
        try:
            prompt = self._prompt_manager.get_prompt(
                "test_case", 
                {"test_case_input": test_case_input, "test_point_input": test_point_input}
            )
            response = self._llm_client.invoke(prompt)
            return response
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_idx}] 测试用例生成失败: {e}")
            return ""
    
    def _create_empty_case(self, row_idx: int) -> Dict[str, Any]:
        """为错误处理创建空的测试用例"""
        return {
            "原始行号": row_idx,
            "测试点": "",
            "测试点编号": "",
            "测试点描述": "",
            "前置条件": "",
            "测试步骤": "",
            "预期结果": ""
        }