import re
from pathlib import Path
from typing import Dict
from src.util.logging_util import get_logger

logger = get_logger(__name__)

class PromptManager:
    """提示词管理器"""
    
    def __init__(self, settings):
        self.settings = settings
        # 从配置获取提示词文件路径
        self.test_point_prompt_file = Path(settings.get_config_value("file.test_point_prompt_file"))
        self.test_case_prompt_file = Path(settings.get_config_value("file.test_case_prompt_file"))
        self.prompts = self._load_prompts()
    
    def _load_prompts(self) -> Dict[str, str]:
        """加载提示词"""
        prompts = {}
        try:
            # 加载测试点提示词
            if self.test_point_prompt_file.exists():
                with open(self.test_point_prompt_file, 'r', encoding='utf-8') as f:
                    prompts["test_point"] = f.read().strip()
                logger.info(f"加载测试点提示词: {self.test_point_prompt_file}")
            else:
                # 如果提示词文件不存在，尝试在相对路径中查找
                base_dir = Path(__file__).parent.parent.parent
                fallback_path = base_dir / self.test_point_prompt_file
                if fallback_path.exists():
                    with open(fallback_path, 'r', encoding='utf-8') as f:
                        prompts["test_point"] = f.read().strip()
                    logger.info(f"加载测试点提示词: {fallback_path}")
                else:
                    raise FileNotFoundError(f"测试点提示词文件不存在: {self.test_point_prompt_file}")
            
            # 加载测试用例提示词
            if self.test_case_prompt_file.exists():
                with open(self.test_case_prompt_file, 'r', encoding='utf-8') as f:
                    prompts["test_case"] = f.read().strip()
                logger.info(f"加载测试用例提示词: {self.test_case_prompt_file}")
            else:
                # 如果提示词文件不存在，尝试在相对路径中查找
                base_dir = Path(__file__).parent.parent.parent
                fallback_path = base_dir / self.test_case_prompt_file
                if fallback_path.exists():
                    with open(fallback_path, 'r', encoding='utf-8') as f:
                        prompts["test_case"] = f.read().strip()
                    logger.info(f"加载测试用例提示词: {fallback_path}")
                else:
                    raise FileNotFoundError(f"测试用例提示词文件不存在: {self.test_case_prompt_file}")
                
            logger.info(f"成功加载 {len(prompts)} 个提示词")
        except Exception as e:
            logger.error(f"加载提示词失败: {e}")
            raise
        return prompts
    
    def _validate_variables(self, prompt_name: str, provided_variables: Dict[str, str]) -> bool:
        """验证提供的变量是否完整"""
        required_variables = self._extract_variables(prompt_name)
        missing_variables = set(required_variables) - set(provided_variables.keys())
        
        if missing_variables:
            logger.error(f"提示词 '{prompt_name}' 缺失变量: {missing_variables}")
            return False
        
        return True
    
    def _extract_variables(self, prompt_name: str):
        """提取提示词中的变量"""
        if prompt_name not in self.prompts:
            return []
        
        prompt = self.prompts[prompt_name]
        # 使用正则表达式匹配 {variable} 格式的变量
        variables = re.findall(r'\{(\w+)\}', prompt)
        return list(set(variables))  # 去重
    
    def get_prompt(self, prompt_name: str, variables: Dict[str, str] = None) -> str:
        """获取提示词"""
        if prompt_name not in self.prompts:
            logger.error(f"提示词不存在: {prompt_name}")
            raise ValueError(f"提示词不存在: {prompt_name}")
        
        prompt = self.prompts[prompt_name]
        
        if variables:
            # 验证变量
            if not self._validate_variables(prompt_name, variables):
                raise ValueError(f"提示词变量验证失败: {prompt_name}")
            
            try:
                prompt = prompt.format(**variables)
            except KeyError as e:
                logger.error(f"提示词变量缺失: {e}")
                raise ValueError(f"提示词变量 '{e}' 缺失")
        
        return prompt