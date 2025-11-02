"""
Web API 模块
"""

from .blueprint import api_blueprint, config_blueprint, upload_blueprint, result_blueprint

__all__ = ['api_blueprint', 'config_blueprint', 'upload_blueprint', 'result_blueprint']