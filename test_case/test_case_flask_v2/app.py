"""
主应用程序模块
仅负责初始化Flask应用、注册蓝图、加载配置、启动服务
"""

import os
import sys
from pathlib import Path
from flask import Flask, redirect, url_for

from src.core.dependency_injector import init_container
from src.web.blueprint import api_blueprint, config_blueprint, upload_blueprint, result_blueprint
from src.web.error_handler import register_error_handler
from src.util.logger_util import setup_logging, get_logger

logger = get_logger(__name__)

def _get_base_dir() -> Path:
    """获取基础目录路径，支持打包环境"""
    if getattr(sys, 'frozen', False):
        # 打包后的环境，使用可执行文件所在目录
        return Path(sys.executable).parent
    else:
        # 开发环境，使用当前工作目录
        return Path(os.path.abspath("."))

def create_app():
    """创建Flask应用"""
    app = Flask(__name__, template_folder='template')
    app.config['SECRET_KEY'] = os.getenv('SECRET_KEY', 'your-secret-key-change-in-production')
    app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024
    
    # 获取基础目录
    base_dir = _get_base_dir()
    
    # 配置文件路径
    config_path = base_dir / 'config.json'
    
    # 初始化依赖注入容器
    container = init_container(config_path)
    
    # 从配置服务获取目录设置
    config_service = container.config
    upload_dir = config_service.get_file_path("upload_dir")
    output_dir = config_service.get_file_path("output_dir")
    prompt_dir = config_service.get_file_path("prompt_dir")
    
    # 确保必要的目录存在
    ensure_directories([
        upload_dir / "input", 
        upload_dir / "prompt", 
        output_dir, 
        prompt_dir, 
        base_dir / "log"
    ])
    
    # 配置日志
    setup_logging()
    
    # 注册全局异常处理器
    register_error_handler(app)
    
    # 注册蓝图
    register_blueprints(app)
    
    # 根路径重定向到上传页面
    @app.route('/')
    def index():
        return redirect(url_for('upload.upload_file'))
    
    logger.info(f"Flask应用初始化完成，基础目录: {base_dir}")
    return app

def ensure_directories(directories):
    """确保必要的目录存在"""
    for directory in directories:
        directory.mkdir(parents=True, exist_ok=True)

def register_blueprints(app):
    """注册所有蓝图"""
    app.register_blueprint(api_blueprint)
    app.register_blueprint(config_blueprint)
    app.register_blueprint(upload_blueprint)
    app.register_blueprint(result_blueprint)

if __name__ == '__main__':
    app = create_app()
    app.run(debug=True, host='0.0.0.0', port=5000)