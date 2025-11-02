"""
自定义异常定义
"""

class AppException(Exception):
    """应用基础异常"""
    
    def __init__(self, message: str, code: str = None, detail: dict = None):
        super().__init__(message)
        self.message = message
        self.code = code or "APP_ERROR"
        self.detail = detail or {}

class ConfigException(AppException):
    """配置异常"""
    
    def __init__(self, message: str, detail: dict = None):
        super().__init__(message, "CONFIG_ERROR", detail)

class LLMException(AppException):
    """LLM相关异常"""
    
    def __init__(self, message: str, detail: dict = None):
        super().__init__(message, "LLM_ERROR", detail)

class DataProcessingException(AppException):
    """数据处理异常"""
    
    def __init__(self, message: str, detail: dict = None):
        super().__init__(message, "DATA_PROCESSING_ERROR", detail)

class FileOperationException(AppException):
    """文件操作异常"""
    
    def __init__(self, message: str, detail: dict = None):
        super().__init__(message, "FILE_OPERATION_ERROR", detail)

class ValidationException(AppException):
    """数据验证异常"""
    
    def __init__(self, message: str, detail: dict = None):
        super().__init__(message, "VALIDATION_ERROR", detail)