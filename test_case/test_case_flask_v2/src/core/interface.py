"""
模块接口定义
定义清晰的模块边界，每个模块只暴露必要接口
"""

from abc import ABC, abstractmethod
from pathlib import Path
from typing import Any, Dict, List

class IDataLoader(ABC):
    """数据加载器接口"""
    
    @abstractmethod
    def load(self, file_path: Path) -> Dict[str, List[Dict[str, Any]]]:
        """从文件加载数据"""
        pass

class IDataProcessor(ABC):
    """数据处理器接口"""
    
    @abstractmethod
    def process_batch(self, items: List[Dict[str, Any]], sheet_name: str) -> List[Dict[str, Any]]:
        """批量处理数据项"""
        pass

class IFileWriter(ABC):
    """文件写入器接口"""
    
    @abstractmethod
    def write(self, data_dict: Dict[str, List[Dict[str, Any]]], output_path: Path) -> bool:
        """将数据写入文件"""
        pass

class ILLMClient(ABC):
    """LLM客户端接口"""
    
    @abstractmethod
    def invoke(self, prompt: str) -> str:
        """调用LLM"""
        pass

class IPromptManager(ABC):
    """提示词管理器接口"""
    
    @abstractmethod
    def get_prompt(self, prompt_name: str, variables: Dict[str, str] = None) -> str:
        """获取格式化提示词"""
        pass