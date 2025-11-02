"""
统一配置服务
所有配置通过这个单一服务获取
"""

import json
import sys
import os
from pathlib import Path
from typing import Any, Dict, Optional

from ..util.logger_util import get_logger

logger = get_logger(__name__)

class ConfigService:
    """统一配置服务"""
    
    _instance: Optional['ConfigService'] = None
    
    def __init__(self, config_path: Path):
        if ConfigService._instance is not None:
            raise RuntimeError("ConfigService是单例，请使用get_instance()方法")
        
        self._config_path = self._get_absolute_path(config_path)
        self._config = self._load_config()
        self._ensure_directories()
    
    @classmethod
    def get_instance(cls) -> 'ConfigService':
        """获取配置服务单例实例"""
        if cls._instance is None:
            raise RuntimeError("配置服务未初始化")
        return cls._instance
    
    @classmethod
    def initialize(cls, config_path: Path) -> 'ConfigService':
        """初始化配置服务"""
        if cls._instance is None:
            cls._instance = cls(config_path)
        return cls._instance
    
    def _get_absolute_path(self, config_path: Path) -> Path:
        """获取配置文件的绝对路径，支持打包环境"""
        # 如果是打包后的可执行文件
        if getattr(sys, 'frozen', False):
            # 在打包后的环境中，配置文件应该在可执行文件同目录
            base_dir = Path(sys._MEIPASS) if hasattr(sys, '_MEIPASS') else Path(sys.executable).parent
            return base_dir / config_path.name
        else:
            # 开发环境中使用相对路径
            return Path(config_path)
    
    def _load_config(self) -> Dict[str, Any]:
        """从JSON文件加载配置"""
        if not self._config_path.exists():
            raise FileNotFoundError(f"配置文件不存在: {self._config_path}")
        
        with open(self._config_path, 'r', encoding='utf-8') as f:
            config = json.load(f)
            logger.info(f"配置已从{self._config_path}加载")
            return config
    
    def _ensure_directories(self) -> None:
        """确保所需目录存在"""
        directories = [
            self.get_file_path("upload_dir") / "input",
            self.get_file_path("upload_dir") / "prompt", 
            self.get_file_path("output_dir"),
            self.get_file_path("prompt_dir"),
            self._get_base_dir() / "log"
        ]
        
        for directory in directories:
            if not directory.exists():
                directory.mkdir(parents=True, exist_ok=True)
    
    def _get_base_dir(self) -> Path:
        """获取基础目录路径，支持打包环境"""
        if getattr(sys, 'frozen', False):
            # 打包后的环境，使用可执行文件所在目录
            return Path(sys.executable).parent
        else:
            # 开发环境，使用当前工作目录
            return Path(os.path.abspath("."))
    
    def get(self, key_path: str, default: Any = None) -> Any:
        """通过点分隔的键路径获取配置值"""
        keys = key_path.split('.')
        value = self._config
        
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                if default is not None:
                    return default
                raise KeyError(f"配置键未找到: {key_path}")
        
        return value
    
    def get_file_path(self, config_key: str) -> Path:
        """获取文件路径配置，支持打包环境"""
        path_str = self.get(f"file.{config_key}")
        path = Path(path_str)
        
        # 如果是相对路径，转换为基于基础目录的绝对路径
        if not path.is_absolute():
            return self._get_base_dir() / path
        return path
    
    def get_model_config(self) -> Dict[str, Any]:
        """获取模型配置"""
        return self.get("model", {})
    
    def get_file_config(self) -> Dict[str, Any]:
        """获取文件配置"""
        return self.get("file", {})
    
    def get_processing_config(self) -> Dict[str, Any]:
        """获取处理配置"""
        return self.get("input_excel_processing", {})
    
    def get_style_config(self) -> Dict[str, Any]:
        """获取样式配置"""
        return self.get("output_excel_style", {})

def get_config() -> ConfigService:
    """获取配置服务实例"""
    return ConfigService.get_instance()