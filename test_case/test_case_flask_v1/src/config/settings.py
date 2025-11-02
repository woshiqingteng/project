"""
配置管理模块
处理从JSON文件加载和访问配置
"""

import json
import sys
from pathlib import Path
from typing import Any, Dict


class Settings:
    """全局配置设置管理器"""
    
    def __init__(self, config_path: Path):
        """从配置文件初始化设置
        
        Args:
            config_path: JSON配置文件路径
            
        Raises:
            FileNotFoundError: 如果配置文件不存在
            RuntimeError: 如果配置文件无法加载
        """
        self._config_path = Path(config_path)
        self._config = self._load_config()
        self._ensure_directories()
    
    def _load_config(self) -> Dict[str, Any]:
        """从JSON文件加载配置"""
        if not self._config_path.exists():
            raise FileNotFoundError(f"配置文件不存在: {self._config_path}")
        
        try:
            with open(self._config_path, 'r', encoding='utf-8') as f:
                return json.load(f)
        except Exception as e:
            raise RuntimeError(f"加载配置失败: {e}")
    
    def _ensure_directories(self) -> None:
        """确保所需目录存在"""
        try:
            # 输出目录
            output_file = self.get("file.output_file")
            output_dir = Path(output_file).parent
            if output_dir != Path(".") and not output_dir.exists():
                output_dir.mkdir(parents=True, exist_ok=True)
            
            # 日志目录
            log_dir = Path("log")
            if not log_dir.exists():
                log_dir.mkdir(parents=True, exist_ok=True)
                
            # 上传目录
            upload_dirs = ['upload/input', 'upload/prompt']
            for upload_dir in upload_dirs:
                upload_path = Path(upload_dir)
                if not upload_path.exists():
                    upload_path.mkdir(parents=True, exist_ok=True)
                    
        except Exception as e:
            print(f"警告: 创建目录失败: {e}")
    
    def get(self, key_path: str) -> Any:
        """通过点分隔的键路径获取配置值
        
        Args:
            key_path: 到配置值的点分隔路径
            
        Returns:
            配置值
            
        Raises:
            KeyError: 如果键路径不存在
        """
        keys = key_path.split('.')
        value = self._config
        
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                raise KeyError(f"配置键未找到: {key_path}")
        
        return value


def get_base_dir() -> Path:
    """获取基础目录路径，兼容开发和打包环境"""
    if getattr(sys, 'frozen', False):
        return Path(sys.executable).parent
    else:
        return Path(__file__).parent.parent.parent