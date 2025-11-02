"""
全局异常处理器
"""

import traceback
from flask import jsonify, render_template, request

from ..core.exception import AppException
from ..util.logger_util import get_logger

logger = get_logger(__name__)

def register_error_handler(app):
    """注册全局异常处理器"""
    
    @app.errorhandler(AppException)
    def handle_app_exception(error: AppException):
        error_data = {
            'error': error.code,
            'message': error.message,
            'detail': error.detail
        }
        
        logger.warning(f"应用异常: {error.code} - {error.message}")
        
        if request.path.startswith('/api/'):
            return jsonify(error_data), 400
        else:
            return render_template('error.html', error=error), 400
    
    @app.errorhandler(404)
    def handle_not_found(error):
        error_data = {
            'error': 'NOT_FOUND',
            'message': '请求的资源不存在'
        }
        
        if request.path.startswith('/api/'):
            return jsonify(error_data), 404
        else:
            app_exception = AppException("页面未找到", "NOT_FOUND")
            return render_template('error.html', error=app_exception), 404
    
    @app.errorhandler(500)
    def handle_internal_error(error):
        logger.error(f"服务器内部错误: {str(error)}")
        
        error_data = {
            'error': 'INTERNAL_SERVER_ERROR',
            'message': '服务器内部错误，请稍后重试'
        }
        
        if request.path.startswith('/api/'):
            return jsonify(error_data), 500
        else:
            app_exception = AppException("服务器内部错误，请稍后重试", "INTERNAL_SERVER_ERROR")
            return render_template('error.html', error=app_exception), 500
    
    @app.errorhandler(Exception)
    def handle_generic_exception(error: Exception):
        logger.error(f"未捕获的异常: {str(error)}")
        
        error_data = {
            'error': 'UNKNOWN_ERROR',
            'message': '发生未知错误，请稍后重试'
        }
        
        if request.path.startswith('/api/'):
            return jsonify(error_data), 500
        else:
            app_exception = AppException("发生未知错误，请稍后重试", "UNKNOWN_ERROR")
            return render_template('error.html', error=app_exception), 500