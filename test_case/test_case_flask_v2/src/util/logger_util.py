"""
日志工具模块
为应用程序提供配置的日志记录
"""

import logging
import sys
from datetime import datetime
from pathlib import Path

def setup_logging() -> None:
    """配置应用程序日志记录"""
    log_dir = Path("log")
    log_dir.mkdir(exist_ok=True)
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_file = log_dir / f"main_{timestamp}.log"
    
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)
    
    # 清除现有处理器
    for handler in root_logger.handlers[:]:
        root_logger.removeHandler(handler)
    
    # 文件处理器（DEBUG级别）
    file_handler = logging.FileHandler(log_file, encoding='utf-8')
    file_handler.setLevel(logging.DEBUG)
    file_formatter = logging.Formatter(
        '%(asctime)s [Thread-%(thread)d] %(levelname)s: %(message)s',
        datefmt="%y%m%d-%H:%M:%S"
    )
    file_handler.setFormatter(file_formatter)
    
    # 控制台处理器（INFO级别）
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_formatter = logging.Formatter(
        '%(asctime)s %(levelname)s: %(message)s',
        datefmt="%y%m%d-%H:%M:%S"
    )
    console_handler.setFormatter(console_formatter)
    
    # 添加处理器
    root_logger.addHandler(file_handler)
    root_logger.addHandler(console_handler)
    
    # 抑制嘈杂的库
    logging.getLogger('openai').setLevel(logging.WARNING)
    logging.getLogger('urllib3').setLevel(logging.WARNING)
    logging.getLogger('httpx').setLevel(logging.WARNING)
    
    root_logger.info(f"日志配置完成 - 控制台: INFO, 文件: DEBUG")
    root_logger.info(f"日志文件: {log_file}")

def get_logger(name: str) -> logging.Logger:
    """获取模块的日志记录器实例"""
    return logging.getLogger(name)