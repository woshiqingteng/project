import re
import time
from typing import Dict, Any, List
from concurrent.futures import ThreadPoolExecutor, as_completed
from src.llm.api_client import LLMClient
from src.llm.prompt_manager import PromptManager
from src.util.logging_util import get_logger

logger = get_logger(__name__)

class OutputParser:
    """输出解析器"""
    
    @staticmethod
    def parse_test_case_output(ai_output: str) -> List[Dict[str, str]]:
        """解析测试用例输出，支持多个测试用例，去除预期结果中的---"""
        try:
            test_cases = []
            
            # 分割多个测试用例
            case_blocks = re.split(r'(?=需求名称：)', ai_output.strip())
            
            for block in case_blocks:
                if not block.strip():
                    continue
                    
                case = {}
                
                # 提取需求名称
                requirement_match = re.search(r'需求名称：\s*(.+)', block)
                if requirement_match:
                    case["需求名称"] = requirement_match.group(1).strip()
                else:
                    case["需求名称"] = ""
                
                # 提取测试点编号
                test_point_id_match = re.search(r'测试点编号：\s*(.+)', block)
                if test_point_id_match:
                    case["测试点编号"] = test_point_id_match.group(1).strip()
                else:
                    case["测试点编号"] = ""
                
                # 提取测试点
                test_point_match = re.search(r'测试点：\s*(.+)', block)
                if test_point_match:
                    case["测试点"] = test_point_match.group(1).strip()
                else:
                    case["测试点"] = ""
                
                # 提取前置条件
                precondition_match = re.search(r'前置条件：\s*(.+?)(?=测试步骤：|$)', block, re.DOTALL)
                if precondition_match:
                    case["前置条件"] = precondition_match.group(1).strip()
                else:
                    case["前置条件"] = ""
                
                # 提取测试步骤
                test_steps_match = re.search(r'测试步骤：\s*(.+?)(?=预期结果：|$)', block, re.DOTALL)
                if test_steps_match:
                    case["测试步骤"] = test_steps_match.group(1).strip()
                else:
                    case["测试步骤"] = ""
                
                # 提取预期结果并去除---
                expected_result_match = re.search(r'预期结果：\s*(.+)', block, re.DOTALL)
                if expected_result_match:
                    expected_result = expected_result_match.group(1).strip()
                    # 去除---分隔符
                    expected_result = re.sub(r'---+\s*', '', expected_result)
                    case["预期结果"] = expected_result
                else:
                    case["预期结果"] = ""
                
                # 只有当有实际内容时才添加到结果中
                if any(case.values()):
                    test_cases.append(case)
            
            # 如果没有解析到任何测试用例，返回空列表
            return test_cases
            
        except Exception as e:
            logger.error(f"解析测试用例输出失败: {e}")
            return []

class DataProcessor:
    """数据处理器"""
    
    def __init__(self, llm_client: LLMClient, prompt_manager: PromptManager, settings):
        self.llm_client = llm_client
        self.prompt_manager = prompt_manager
        self.settings = settings
        self.output_parser = OutputParser()
        self.default_threads = settings.get_config_value("input_excel_processing.default_threads")
    
    def prepare_requirement_document(self, item: Dict[str, Any]) -> str:
        """准备需求文档内容"""
        try:
            requirement_parts = []
            
            for key, value in item.items():
                if value is not None and str(value).strip() != '':
                    requirement_parts.append(f"{key}：{value}")
            
            requirement_document = "  ".join(requirement_parts)
            
            logger.debug(f"构建的需求文档: {requirement_document}")
            return requirement_document
            
        except Exception as e:
            logger.error(f"构建需求文档失败: {e}")
            raise
    
    def process_single_row(self, row_index: int, row_data: Dict[str, Any], sheet_name: str) -> List[Dict[str, Any]]:
        """处理单行数据，生成测试点和测试用例，支持多个测试用例"""
        try:
            logger.info(f"[表格 {sheet_name}] [行 #{row_index}] 开始处理")
            
            # 构建需求文档
            requirement_document = self.prepare_requirement_document(row_data)
            
            if not requirement_document.strip():
                logger.info(f"[表格 {sheet_name}] [行 #{row_index}] 数据内容为空，跳过处理")
                return []
            
            # 生成测试点
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 开始生成测试点")
            test_points = self._generate_test_points(requirement_document, row_index, sheet_name)
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 生成测试点完成")
            
            # 生成测试用例
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 开始生成测试用例")
            test_case_outline = self._generate_test_cases(requirement_document, test_points, row_index, sheet_name)
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 生成测试用例完成")
            
            # 解析测试用例输出，可能包含多个测试用例
            parsed_results = self.output_parser.parse_test_case_output(test_case_outline)
            
            # 过滤掉空结果
            valid_results = [result for result in parsed_results if any(result.values())]
            
            if valid_results:
                logger.info(f"[表格 {sheet_name}] [行 #{row_index}] 处理完成，生成 {len(valid_results)} 个测试用例")
            else:
                logger.info(f"[表格 {sheet_name}] [行 #{row_index}] 未生成有效测试用例")
            
            # 为每个测试用例添加原始行号
            results = []
            for parsed_result in valid_results:
                results.append({
                    "原始行号": row_index,
                    **parsed_result
                })
            
            return results
            
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_index}] 处理数据失败: {e}")
            # 响应错误时，返回一个空内容的测试用例
            return [{
                "原始行号": row_index,
                "需求名称": "",
                "测试点编号": "",
                "测试点": "",
                "前置条件": "",
                "测试步骤": "",
                "预期结果": ""
            }]
    
    def _generate_test_points(self, requirement_document: str, row_index: int, sheet_name: str) -> str:
        """生成测试点"""
        try:
            test_point_prompt = self.prompt_manager.get_prompt(
                "test_point",
                {"requirement_document": requirement_document}
            )
            
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 测试点提示词: {test_point_prompt}")
            response = self.llm_client.invoke_llm(test_point_prompt)
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 测试点AI输出: {response}")
            return response
            
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_index}] 生成测试点失败: {e}")
            # 响应错误时返回空字符串
            return ""
    
    def _generate_test_cases(self, requirement_document: str, test_points: str, row_index: int, sheet_name: str) -> str:
        """生成测试用例"""
        try:
            test_case_prompt = self.prompt_manager.get_prompt(
                "test_case",
                {
                    "requirement_document": requirement_document,
                    "test_points_document": test_points
                }
            )
            
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 测试用例提示词: {test_case_prompt}")
            response = self.llm_client.invoke_llm(test_case_prompt)
            logger.debug(f"[表格 {sheet_name}] [行 #{row_index}] 测试用例AI输出: {response}")
            return response
            
        except Exception as e:
            logger.error(f"[表格 {sheet_name}] [行 #{row_index}] 生成测试用例失败: {e}")
            # 响应错误时返回空字符串
            return ""
    
    def process_batch_data(self, items: List[Dict[str, Any]], sheet_name: str) -> List[Dict[str, Any]]:
        """批量处理数据，支持一个测试点生成多个测试用例"""
        start_time = time.time()
        all_results = []
        
        logger.info(f"[表格 {sheet_name}] 开始批量处理 {len(items)} 条数据，使用 {self.default_threads} 个工作线程")
        
        with ThreadPoolExecutor(max_workers=self.default_threads) as executor:
            # 提交任务 - 并行处理每一行数据
            future_to_item = {
                executor.submit(self.process_single_row, idx + 1, item, sheet_name): (item, idx + 1)
                for idx, item in enumerate(items)
            }
            
            # 收集结果
            for future in as_completed(future_to_item):
                try:
                    row_results = future.result()
                    all_results.extend(row_results)
                except Exception as e:
                    logger.error(f"处理任务失败: {e}")
                    # 发生错误时添加一个空内容的测试用例
                    item, row_index = future_to_item[future]
                    all_results.append({
                        "原始行号": row_index,
                        "需求名称": "",
                        "测试点编号": "",
                        "测试点": "",
                        "前置条件": "",
                        "测试步骤": "",
                        "预期结果": ""
                    })
        
        # 按照原始输入顺序重新排序
        sorted_results = sorted(all_results, key=lambda x: x.get("原始行号", 0))
        
        elapsed_time = time.time() - start_time
        logger.info(f"[表格 {sheet_name}] 批量处理完成，共生成 {len(sorted_results)} 个测试用例，总耗时: {elapsed_time:.2f}秒")
        
        return sorted_results