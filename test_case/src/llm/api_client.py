from langchain_openai import ChatOpenAI
from langchain_core.output_parsers import StrOutputParser
from src.util.logging_util import get_logger

logger = get_logger(__name__)

class LLMClient:
    """LLM客户端"""
    
    def __init__(self, settings):  # 添加settings参数
        self.settings = settings
        self.model_config = settings.get_config_value("model")
        self.llm = self._initialize_llm()
    
    def _initialize_llm(self):
        """初始化LLM"""
        api_key = self.model_config.get('api_key')
        base_url = self.model_config.get('base_url')
        
        if not api_key or api_key == "your_api_key_here":
            raise ValueError("请在配置文件中设置有效的API密钥")
        
        if not base_url or base_url == "xxx":
            raise ValueError("请在配置文件中设置有效的base_url")
        
        return ChatOpenAI(
            model=self.model_config.get('name'),
            base_url=base_url,
            api_key=api_key,
            temperature=self.model_config.get('temperature'),
            max_tokens=self.model_config.get('max_tokens'),
            request_timeout=self.model_config.get('request_timeout'),
            max_retries=self.model_config.get('max_retries')
        )
    
    def invoke_llm(self, prompt: str) -> str:
        """调用LLM"""
        try:
            chain = self.llm | StrOutputParser()
            response = chain.invoke(prompt)
            return response.strip()
        except Exception as e:
            logger.error(f"API调用失败: {e}")
            raise

class LLMClientFactory:
    """LLM客户端工厂"""
    
    @staticmethod
    def create_llm_client(client_type: str = "openai", settings=None, **kwargs):
        """创建LLM客户端"""
        if client_type == "openai":
            return LLMClient(settings=settings, **kwargs)
        else:
            raise ValueError(f"不支持的LLM客户端类型: {client_type}")