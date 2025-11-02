"""
API 蓝图定义
"""

from flask import Blueprint

# API 蓝图
api_blueprint = Blueprint('api', __name__, url_prefix='/api')

# 配置管理蓝图
config_blueprint = Blueprint('config', __name__)

# 文件上传蓝图
upload_blueprint = Blueprint('upload', __name__)

# 结果查看蓝图
result_blueprint = Blueprint('result', __name__)

# 导入路由处理函数
from . import route

