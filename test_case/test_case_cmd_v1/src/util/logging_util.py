import logging
import sys
from pathlib import Path
from datetime import datetime

def setup_logging():
    """配置日志 - 文件输出DEBUG级别，控制台输出INFO级别"""
    # 创建log目录
    log_dir = Path("log")
    if not log_dir.exists():
        log_dir.mkdir(parents=True, exist_ok=True)
    
    # 生成带时间戳的日志文件名
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_filename = f"main_{timestamp}.log"
    log_filepath = log_dir / log_filename
    
    # 获取根日志记录器
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)  # 设置为最低级别DEBUG
    
    # 清除现有处理器，避免重复
    for handler in root_logger.handlers[:]:
        root_logger.removeHandler(handler)
    
    # 文件日志格式 - 包含线程信息
    file_formatter = logging.Formatter(
        '%(asctime)s [Thread-%(thread)d] %(levelname)s: %(message)s',
        datefmt="%y%m%d-%H:%M:%S"
    )
    
    # 控制台日志格式 - 不包含线程信息
    console_formatter = logging.Formatter(
        '%(asctime)s %(levelname)s: %(message)s',
        datefmt="%y%m%d-%H:%M:%S"
    )
    
    # 文件处理器 - DEBUG级别，包含所有详细信息
    file_handler = logging.FileHandler(log_filepath, encoding='utf-8')
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(file_formatter)
    
    # 控制台处理器 - INFO级别，只显示重要信息
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(console_formatter)
    
    # 添加处理器到根日志记录器
    root_logger.addHandler(file_handler)
    root_logger.addHandler(console_handler)
    
    # 设置特定库的日志级别
    logging.getLogger('openai').setLevel(logging.WARNING)
    logging.getLogger('urllib3').setLevel(logging.WARNING)
    logging.getLogger('http.client').setLevel(logging.WARNING)
    logging.getLogger('httpx').setLevel(logging.WARNING)
    
    # 记录日志初始化信息
    root_logger.info(f"日志系统初始化完成 - 控制台级别: INFO, 文件级别: DEBUG")
    root_logger.info(f"日志文件路径: {log_filepath}")

def get_logger(name: str) -> logging.Logger:
    """获取日志记录器"""
    return logging.getLogger(name)