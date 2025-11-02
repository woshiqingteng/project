"""
提示词管理模块
处理AI提示词的加载和格式化
"""

import re
from pathlib import Path
from typing import Dict

from ..core.interface import IPromptManager
from ..core.exception import FileOperationException, ValidationException
from ..config.setting import get_config
from ..util.logger_util import get_logger

logger = get_logger(__name__)

class PromptManager(IPromptManager):
    """具有变量替换功能的AI提示词管理器"""
    
    def __init__(self):
        """使用提示词文件路径初始化管理器"""
        self._config = get_config()
        file_config = self._config.get_file_config()
        
        self._test_point_file = Path(file_config.get('test_point_prompt_file', 'prompt/test_point.md'))
        self._test_case_file = Path(file_config.get('test_case_prompt_file', 'prompt/test_case.md'))
        self._prompts = self._load_prompts()
    
    def _load_prompts(self) -> Dict[str, str]:
        """从文件加载提示词"""
        prompts = {}
        
        # 测试点提示词
        test_point_content = self._load_prompt_file(self._test_point_file)
        if test_point_content:
            prompts["test_point"] = test_point_content
            logger.info(f"已加载测试点提示词: {self._test_point_file}")
        
        # 测试用例提示词
        test_case_content = self._load_prompt_file(self._test_case_file)
        if test_case_content:
            prompts["test_case"] = test_case_content
            logger.info(f"已加载测试用例提示词: {self._test_case_file}")
        
        if not prompts:
            raise FileOperationException("未加载任何提示词")
        
        logger.info(f"成功加载 {len(prompts)} 个提示词")
        return prompts
    
    def _load_prompt_file(self, file_path: Path) -> str:
        """从文件加载提示词内容，支持回退路径"""
        if file_path.exists():
            with open(file_path, 'r', encoding='utf-8') as f:
                return f.read().strip()
        
        # 尝试回退路径
        base_dir = Path(__file__).parent.parent.parent
        fallback_path = base_dir / file_path
        
        if fallback_path.exists():
            with open(fallback_path, 'r', encoding='utf-8') as f:
                return f.read().strip()
        
        raise FileOperationException(f"提示词文件未找到: {file_path}")
    
    def get_prompt(self, prompt_name: str, variables: Dict[str, str] = None) -> str:
        """获取带有变量替换的格式化提示词"""
        if prompt_name not in self._prompts:
            raise ValidationException(f"提示词未找到: {prompt_name}")
        
        prompt = self._prompts[prompt_name]
        
        if variables:
            self._validate_variables(prompt_name, prompt, variables)
            prompt = prompt.format(**variables)
        
        return prompt
    
    def _validate_variables(self, prompt_name: str, prompt: str, variables: Dict[str, str]) -> None:
        """验证是否提供了所有必需的变量"""
        required_vars = set(re.findall(r'\{(\w+)\}', prompt))
        provided_vars = set(variables.keys())
        missing_vars = required_vars - provided_vars
        
        if missing_vars:
            raise ValidationException(f"提示词 '{prompt_name}' 缺少变量: {missing_vars}")