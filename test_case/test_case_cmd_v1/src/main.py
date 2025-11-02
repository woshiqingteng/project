import sys
import argparse
from pathlib import Path
import time

# 添加项目根目录到Python路径
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import Settings
from src.core.data_loader import DataLoaderFactory
from src.core.data_processor import DataProcessor
from src.core.file_writer import FileWriterFactory
from src.llm.api_client import LLMClientFactory
from src.llm.prompt_manager import PromptManager
from src.util.logging_util import setup_logging, get_logger

logger = get_logger(__name__)

class Application:
    """应用程序"""
    
    def __init__(self, config_path: Path):
        # 使用指定的配置文件创建Settings实例
        self.settings = Settings(config_path)
        setup_logging()
        
        # 初始化组件时传入settings
        self.prompt_manager = PromptManager(self.settings)
        self.llm_client = LLMClientFactory.create_llm_client(settings=self.settings)
        self.data_processor = DataProcessor(self.llm_client, self.prompt_manager, self.settings)
        
        logger.info(f"应用程序初始化完成，使用配置文件: {config_path}")
    
    def execute(self):
        """执行应用程序"""
        try:
            start_time = time.time()
            
            # 获取输入输出文件路径
            input_file = self.settings.get_config_value("file.input_file")
            input_path = Path(input_file)
            
            # 验证输入文件
            if not input_path.exists():
                raise FileNotFoundError(f"输入文件不存在: {input_path}")
            
            # 生成输出文件名 - 使用配置文件中的完整路径
            output_file_template = self.settings.get_config_value("file.output_file")
            timestamp = time.strftime("%Y%m%d_%H%M%S")
            
            # 确保使用配置文件中的目录结构
            output_path = Path(output_file_template)
            output_filename = f"{output_path.stem}_{timestamp}{output_path.suffix}"
            final_output_path = output_path.parent / output_filename
            
            # 创建数据加载器
            data_loader = DataLoaderFactory.create_data_loader("excel", settings=self.settings)
            
            # 加载数据
            raw_data_dict = data_loader.load_data(input_path)
            
            if not raw_data_dict:
                logger.error("没有找到有效数据，程序结束")
                return
            
            # 处理数据
            logger.info("开始处理数据...")
            processed_data_dict = {}
            total_rows = 0
            
            for sheet_name, raw_data in raw_data_dict.items():
                logger.info(f"处理表格: {sheet_name}，共 {len(raw_data)} 行数据")
                processed_data = self.data_processor.process_batch_data(raw_data, sheet_name)
                processed_data_dict[sheet_name] = processed_data
                total_rows += len(processed_data)
            
            # 输出Excel文件
            excel_writer = FileWriterFactory.create_file_writer("excel", settings=self.settings)
            excel_success = excel_writer.write_data(processed_data_dict, final_output_path)
            
            if excel_success:
                elapsed_time = time.time() - start_time
                logger.info(f"处理完成! 总耗时: {elapsed_time:.2f}秒")
                logger.info(f"处理总行数: {total_rows}")
                logger.info(f"输入文件: {input_path}")
                logger.info(f"输出文件: {final_output_path}")
            else:
                logger.error("Excel文件生成失败")
            
        except Exception as e:
            logger.error(f"应用程序执行失败: {e}")
            raise

def get_default_config_path():
    """获取默认配置文件路径"""
    # 如果是打包后的exe，默认配置文件在exe同目录下
    if getattr(sys, 'frozen', False):
        base_dir = Path(sys.executable).parent
        # 打包后，尝试在exe同目录下查找配置文件
        config_path = base_dir / "config.json"
        if config_path.exists():
            return config_path
        else:
            # 如果不存在，返回一个默认路径，但会报错
            return base_dir / "config.json"
    else:
        # 开发环境，配置文件在项目根目录下的config目录中
        base_dir = Path(__file__).parent.parent
        config_path = base_dir / "config" / "config.json"
        return config_path

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Excel AI测试用例生成工具')
    parser.add_argument('--config', help='配置文件路径（可选，如不指定则使用默认配置）')
    
    args = parser.parse_args()
    
    # 确定配置文件路径
    if args.config:
        config_path = Path(args.config)
    else:
        config_path = get_default_config_path()
        print(f"未指定配置文件，使用默认路径: {config_path}")
    
    # 检查配置文件是否存在
    if not config_path.exists():
        print(f"错误: 配置文件不存在: {config_path}")
        print("请使用 --config 参数指定配置文件路径")
        sys.exit(1)
    
    try:
        app = Application(config_path)
        app.execute()
    except Exception as e:
        logger.error(f"程序执行失败: {e}")
        print(f"错误: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()