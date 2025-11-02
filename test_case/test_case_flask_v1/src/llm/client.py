"""
LLM客户端模块
处理与语言模型的通信
"""

from langchain_openai import ChatOpenAI
from langchain_core.output_parsers import StrOutputParser

from ..util.logger import get_logger


logger = get_logger(__name__)


class LLMClient:
    """用于与语言模型交互的客户端"""
    
    def __init__(self, settings):
        """使用模型配置初始化客户端"""
        self._settings = settings
        self._model_config = settings.get("model")
        self._llm = self._init_llm()
    
    def _init_llm(self) -> ChatOpenAI:
        """使用配置初始化LLM"""
        api_key = self._model_config.get('api_key')
        base_url = self._model_config.get('base_url')
        
        if not api_key or api_key == "your_api_key_here":
            raise ValueError("API密钥未配置")
        
        if not base_url or base_url == "xxx":
            raise ValueError("基础URL未配置")
        
        return ChatOpenAI(
            model=self._model_config.get('name'),
            base_url=base_url,
            api_key=api_key,
            temperature=self._model_config.get('temperature'),
            max_tokens=self._model_config.get('max_tokens'),
            request_timeout=self._model_config.get('request_timeout'),
            max_retries=self._model_config.get('max_retries')
        )
    
    def invoke(self, prompt: str) -> str:
        """使用提示调用LLM
        
        Args:
            prompt: 输入提示文本
            
        Returns:
            LLM响应文本
            
        Raises:
            Exception: 如果API调用失败
        """
        try:
            chain = self._llm | StrOutputParser()
            response = chain.invoke(prompt)
            return response.strip()
        except Exception as e:
            logger.error(f"LLM调用失败: {e}")
            raise


class LLMClientFactory:
    """LLM客户端工厂"""
    
    @staticmethod
    def create(client_type: str = "openai", settings=None, **kwargs):
        """创建LLM客户端实例
        
        Args:
            client_type: 要创建的客户端类型
            settings: 配置设置
            **kwargs: 附加参数
            
        Returns:
            LLM客户端实例
            
        Raises:
            ValueError: 如果客户端类型不受支持
        """
        if client_type == "openai":
            return LLMClient(settings=settings, **kwargs)
        else:
            raise ValueError(f"不支持的客户端类型: {client_type}")