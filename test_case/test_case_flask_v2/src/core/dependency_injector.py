"""
依赖注入容器
通过构造函数或函数参数传入依赖，替代硬编码依赖
"""

from typing import Dict, Any, Optional
from pathlib import Path

from ..config.setting import ConfigService
from ..util.logger_util import get_logger

logger = get_logger(__name__)

class DIContainer:
    """依赖注入容器"""
    
    def __init__(self, config_path: Path):
        # 首先初始化配置服务
        self._config_service = ConfigService.initialize(config_path)
        self._components: Dict[str, Any] = {}
    
    @property
    def config(self) -> ConfigService:
        """获取配置服务"""
        return self._config_service
    
    @property
    def llm_client(self):
        """获取LLM客户端"""
        return self._get_component('llm_client', self._create_llm_client)
    
    @property
    def prompt_manager(self):
        """获取提示词管理器"""
        return self._get_component('prompt_manager', self._create_prompt_manager)
    
    @property
    def data_loader(self):
        """获取数据加载器"""
        return self._get_component('data_loader', self._create_data_loader)
    
    @property
    def file_writer(self):
        """获取文件写入器"""
        return self._get_component('file_writer', self._create_file_writer)
    
    @property
    def data_processor(self):
        """获取数据处理器"""
        return self._get_component('data_processor', self._create_data_processor)
    
    def _get_component(self, name: str, factory_method):
        """获取或创建组件实例"""
        if name not in self._components:
            self._components[name] = factory_method()
            logger.debug(f"创建组件: {name}")
        return self._components[name]
    
    def _create_llm_client(self):
        from ..llm.client import LLMClient
        return LLMClient()
    
    def _create_prompt_manager(self):
        from ..llm.prompt_manager import PromptManager
        return PromptManager()
    
    def _create_data_loader(self):
        from .data_loader import ExcelDataLoader
        return ExcelDataLoader()
    
    def _create_file_writer(self):
        from .file_writer import ExcelWriter
        return ExcelWriter()
    
    def _create_data_processor(self):
        from .data_processor import DataProcessor
        return DataProcessor(self.llm_client, self.prompt_manager)

# 全局容器实例
_container: Optional[DIContainer] = None

def init_container(config_path: Path) -> DIContainer:
    """初始化全局依赖注入容器"""
    global _container
    _container = DIContainer(config_path)
    return _container

def get_container() -> DIContainer:
    """获取全局依赖注入容器"""
    if _container is None:
        raise RuntimeError("依赖注入容器未初始化")
    return _container