"""
LLM客户端模块
处理与语言模型的通信
"""

from langchain_openai import ChatOpenAI
from langchain_core.output_parsers import StrOutputParser

from ..core.interface import ILLMClient
from ..core.exception import LLMException
from ..config.setting import get_config
from ..util.logger_util import get_logger

logger = get_logger(__name__)

class LLMClient(ILLMClient):
    """用于与语言模型交互的客户端"""
    
    def __init__(self):
        """使用配置服务初始化客户端"""
        self._config = get_config()
        self._model_config = self._config.get_model_config()
        self._llm = self._init_llm()
    
    def _init_llm(self) -> ChatOpenAI:
        """使用配置服务初始化LLM"""
        api_key = self._model_config.get('api_key')
        base_url = self._model_config.get('base_url')
        
        if not api_key or api_key == "your_api_key_here":
            raise LLMException("API密钥未配置")
        
        if not base_url or base_url == "xxx":
            raise LLMException("基础URL未配置")
        
        return ChatOpenAI(
            model=self._model_config.get('name'),
            base_url=base_url,
            api_key=api_key,
            temperature=self._model_config.get('temperature', 0),
            max_tokens=self._model_config.get('max_tokens', 8192),
            request_timeout=self._model_config.get('request_timeout', 300),
            max_retries=self._model_config.get('max_retries', 3)
        )
    
    def invoke(self, prompt: str) -> str:
        """使用提示调用LLM"""
        try:
            chain = self._llm | StrOutputParser()
            response = chain.invoke(prompt)
            return response.strip()
        except Exception as e:
            logger.error(f"LLM调用失败: {e}")
            raise LLMException(f"LLM调用失败: {e}")