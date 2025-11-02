"""
主应用程序模块
编排测试用例生成工作流
"""

import argparse
import sys
import time
from pathlib import Path

from config.settings import Settings, get_base_dir
from core.data_loader import DataLoaderFactory
from core.data_processor import DataProcessor
from core.file_writer import FileWriterFactory
from llm.client import LLMClientFactory
from llm.prompt_manager import PromptManager
from util.logger import setup_logging, get_logger


logger = get_logger(__name__)


class Application:
    """主应用程序类"""
    
    def __init__(self, config_path: Path):
        """使用配置初始化应用程序"""
        self._settings = Settings(config_path)
        setup_logging()
        
        self._prompt_manager = PromptManager(self._settings)
        self._llm_client = LLMClientFactory.create(settings=self._settings)
        self._data_processor = DataProcessor(self._llm_client, self._prompt_manager, self._settings)
        
        logger.info(f"应用程序已使用配置初始化: {config_path}")
    
    def run(self) -> None:
        """执行主应用程序工作流"""
        start_time = time.time()
        
        try:
            # 设置路径
            input_path = Path(self._settings.get("file.input_file"))
            output_path = self._prepare_output_path()
            
            # 加载数据
            data_loader = DataLoaderFactory.create(settings=self._settings)
            raw_data = data_loader.load(input_path)
            
            if not raw_data:
                logger.error("未找到有效数据")
                return
            
            # 处理数据
            processed_data = self._process_all_sheets(raw_data)
            
            # 写入输出
            excel_writer = FileWriterFactory.create(settings=self._settings)
            success = excel_writer.write(processed_data, output_path)
            
            if success:
                elapsed = time.time() - start_time
                self._log_completion(input_path, output_path, processed_data, elapsed)
            else:
                logger.error("输出生成失败")
                
        except Exception as e:
            logger.error(f"应用程序执行失败: {e}")