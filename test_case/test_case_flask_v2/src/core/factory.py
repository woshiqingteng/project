"""
工厂模式实现
通过工厂根据配置创建对象，解耦对象创建
"""

from typing import Dict, Any
from pathlib import Path

from .interface import IDataLoader, IDataProcessor, IFileWriter, ILLMClient, IPromptManager
from .data_loader import ExcelDataLoader
from .data_processor import DataProcessor
from .file_writer import ExcelWriter
from .exception import AppException
from ..llm.client import LLMClient
from ..llm.prompt_manager import PromptManager
from ..config.setting import get_config
from ..util.logger_util import get_logger


logger = get_logger(__name__)


class DataLoaderFactory:
    """数据加载器工厂"""
    
    @staticmethod
    def create(loader_type: str = None, **kwargs) -> IDataLoader:
        """创建数据加载器实例"""
        config = get_config()
        loader_type = loader_type or "excel"  # 默认使用excel加载器
        
        if loader_type == "excel":
            return ExcelDataLoader(**kwargs)
        else:
            raise AppException(f"不支持的加载器类型: {loader_type}")


class DataProcessorFactory:
    """数据处理器工厂"""
    
    @staticmethod
    def create(llm_client: ILLMClient, prompt_manager: IPromptManager, **kwargs) -> IDataProcessor:
        """创建数据处理器实例"""
        return DataProcessor(llm_client, prompt_manager, **kwargs)


class FileWriterFactory:
    """文件写入器工厂"""
    
    @staticmethod
    def create(writer_type: str = None, **kwargs) -> IFileWriter:
        """创建文件写入器实例"""
        config = get_config()
        writer_type = writer_type or "excel"  # 默认使用excel写入器
        
        if writer_type == "excel":
            return ExcelWriter(**kwargs)
        else:
            raise AppException(f"不支持的写入器类型: {writer_type}")


class LLMClientFactory:
    """LLM客户端工厂"""
    
    @staticmethod
    def create(client_type: str = None, **kwargs) -> ILLMClient:
        """创建LLM客户端实例"""
        config = get_config()
        client_type = client_type or "openai"  # 默认使用openai客户端
        
        if client_type == "openai":
            return LLMClient(**kwargs)
        else:
            raise AppException(f"不支持的客户端类型: {client_type}")


class PromptManagerFactory:
    """提示词管理器工厂"""
    
    @staticmethod
    def create(**kwargs) -> IPromptManager:
        """创建提示词管理器实例"""
        return PromptManager(**kwargs)


class ServiceFactory:
    """服务工厂 - 统一创建各种服务"""
    
    def __init__(self):
        self.config = get_config()
    
    def create_llm_client(self) -> ILLMClient:
        """创建LLM客户端"""
        return LLMClientFactory.create()
    
    def create_prompt_manager(self) -> IPromptManager:
        """创建提示词管理器"""
        return PromptManagerFactory.create()
    
    def create_data_loader(self) -> IDataLoader:
        """创建数据加载器"""
        return DataLoaderFactory.create()
    
    def create_file_writer(self) -> IFileWriter:
        """创建文件写入器"""
        return FileWriterFactory.create()
    
    def create_data_processor(self) -> IDataProcessor:
        """创建数据处理器"""
        llm_client = self.create_llm_client()
        prompt_manager = self.create_prompt_manager()
        return DataProcessorFactory.create(llm_client, prompt_manager)