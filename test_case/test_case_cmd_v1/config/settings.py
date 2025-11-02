from pathlib import Path
import sys
import json
import logging

logger = logging.getLogger(__name__)

def get_base_directory():
    """获取基础目录，兼容开发环境和打包后环境"""
    if getattr(sys, 'frozen', False):
        return Path(sys.executable).parent
    else:
        return Path(__file__).parent.parent

class Settings:
    """全局配置类"""
    
    def __init__(self, config_path: Path):
        # 动态获取基础目录
        self.BASE_DIR = get_base_directory()
        
        # 必须通过参数指定配置文件路径
        self.config_path = Path(config_path)
        self.config = self._load_configuration()
        
        # 初始化时检查输出目录
        self._ensure_output_directories()
    
    def _load_configuration(self):
        """从JSON文件加载配置"""
        if not self.config_path.exists():
            raise FileNotFoundError(f"配置文件不存在: {self.config_path}")
        
        try:
            with open(self.config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            # 使用当前配置的日志记录器记录信息
            current_logger = logging.getLogger(__name__)
            current_logger.info(f"配置文件加载成功: {self.config_path}")
            return config
        except Exception as e:
            current_logger = logging.getLogger(__name__)
            current_logger.error(f"加载配置文件失败: {e}")
            raise RuntimeError(f"加载配置文件失败: {e}")
    
    def _ensure_output_directories(self):
        """确保输出目录和日志目录存在"""
        try:
            # 检查输出文件目录
            output_file = self.get_config_value("file.output_file")
            output_path = Path(output_file)
            output_dir = output_path.parent
            
            # 如果输出目录不是当前目录，确保创建
            if str(output_dir) != "." and not output_dir.exists():
                output_dir.mkdir(parents=True, exist_ok=True)
                current_logger = logging.getLogger(__name__)
                current_logger.info(f"创建输出目录: {output_dir}")
            
            # 检查输入文件目录
            input_file = self.get_config_value("file.input_file")
            input_path = Path(input_file)
            input_dir = input_path.parent
            
            # 如果输入目录不是当前目录且不存在，尝试创建
            if str(input_dir) != "." and not input_dir.exists():
                input_dir.mkdir(parents=True, exist_ok=True)
                current_logger = logging.getLogger(__name__)
                current_logger.info(f"创建输入目录: {input_dir}")
            
            # 检查日志目录
            log_dir = Path("log")
            if not log_dir.exists():
                log_dir.mkdir(parents=True, exist_ok=True)
                current_logger = logging.getLogger(__name__)
                current_logger.info(f"创建日志目录: {log_dir}")
                
        except Exception as e:
            current_logger = logging.getLogger(__name__)
            current_logger.warning(f"创建输出目录时出现警告: {e}")
    
    def get_config_value(self, key_path):
        """获取配置值"""
        keys = key_path.split('.')
        value = self.config
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                raise KeyError(f"配置项 '{key_path}' 不存在")
        return value