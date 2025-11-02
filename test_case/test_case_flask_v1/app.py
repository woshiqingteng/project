import sys
import os
import json
import threading
import logging
import shutil
from datetime import datetime
from pathlib import Path
from flask import Flask, render_template, request, redirect, url_for, flash, send_file, jsonify

# 导入项目核心模块
from src.config.settings import Settings
from src.core.data_loader import DataLoaderFactory
from src.core.data_processor import DataProcessor
from src.core.file_writer import FileWriterFactory
from src.llm.client import LLMClientFactory
from src.llm.prompt_manager import PromptManager
from src.util.logger import setup_logging, get_logger

def resource_path(relative_path):
    try:
        base_path = sys._MEIPASS
    except AttributeError:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

def user_data_path(relative_path):
    if getattr(sys, 'frozen', False):
        base_path = os.path.dirname(sys.executable)
    else:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

app = Flask(__name__, template_folder='template')
app.config['SECRET_KEY'] = 'your-secret-key-change-in-production'
app.config['UPLOAD_INPUT_FOLDER'] = user_data_path('upload/input')
app.config['UPLOAD_PROMPT_FOLDER'] = user_data_path('upload/prompt')
app.config['OUTPUT_FOLDER'] = user_data_path('output')
app.config['PROMPT_FOLDER'] = user_data_path('prompt')
app.config['LOG_FOLDER'] = user_data_path('log')
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16MB
app.config['MAX_FILES_COUNT'] = 100  # 最多保存100个文件

# 确保必要的目录存在
def ensure_directories():
    """确保必要的目录存在"""
    directories = [
        app.config['UPLOAD_INPUT_FOLDER'],
        app.config['UPLOAD_PROMPT_FOLDER'],
        app.config['OUTPUT_FOLDER'], 
        app.config['PROMPT_FOLDER'],
        app.config['LOG_FOLDER']  # 日志目录
    ]
    
    for directory in directories:
        Path(directory).mkdir(parents=True, exist_ok=True)
        print(f"确保目录存在: {directory}")

# 在应用启动时调用
ensure_directories()

# 存储处理状态和日志
processing_status = {}
processing_results = {}
job_logs = {}

class WebLogger:
    """Web应用日志记录器"""
    def __init__(self, job_id):
        self.job_id = job_id
        if job_id not in job_logs:
            job_logs[job_id] = []
    
    def info(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] INFO: {message}"
        job_logs[self.job_id].append(log_entry)
        logging.info(f"[{self.job_id}] {message}")
    
    def error(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] ERROR: {message}"
        job_logs[self.job_id].append(log_entry)
        logging.error(f"[{self.job_id}] {message}")
    
    def warning(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] WARNING: {message}"
        job_logs[self.job_id].append(log_entry)
        logging.warning(f"[{self.job_id}] {message}")

def allowed_file(filename, allowed_extensions=None):
    if allowed_extensions is None:
        allowed_extensions = {'xlsx', 'xls', 'md', 'txt'}
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in allowed_extensions

def get_config_path():
    return Path(resource_path('config.json'))

def load_config():
    """加载配置文件"""
    config_path = get_config_path()
    if config_path.exists():
        with open(config_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    return {}

def save_config(config_data):
    """保存配置文件"""
    config_path = get_config_path()
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(config_data, f, ensure_ascii=False, indent=4)

def cleanup_old_files(folder, max_files=100):
    """清理旧文件，保持最多max_files个文件"""
    try:
        path = Path(folder)
        if not path.exists():
            return
        
        # 获取所有文件并按修改时间排序
        files = [f for f in path.glob('*') if f.is_file()]
        files.sort(key=lambda x: x.stat().st_mtime)
        
        # 如果文件数量超过限制，删除最旧的文件
        if len(files) > max_files:
            files_to_delete = files[:len(files) - max_files]
            for file_path in files_to_delete:
                try:
                    file_path.unlink()
                    print(f"已清理旧文件: {file_path}")
                except Exception as e:
                    print(f"清理文件失败 {file_path}: {e}")
    except Exception as e:
        print(f"清理文件时出错: {e}")

def save_uploaded_file(file_storage, target_folder, filename):
    """安全保存上传的文件"""
    try:
        # 确保目标目录存在
        Path(target_folder).mkdir(parents=True, exist_ok=True)
        
        # 构建完整路径
        filepath = Path(target_folder) / filename
        
        # 立即保存文件内容到内存，避免文件句柄问题
        file_content = file_storage.read()
        
        # 写入文件
        with open(filepath, 'wb') as f:
            f.write(file_content)
        
        # 验证文件是否成功保存
        if filepath.exists() and filepath.stat().st_size > 0:
            # 清理旧文件（只有在超过限制时才清理）
            cleanup_old_files(target_folder, app.config['MAX_FILES_COUNT'])
            return filepath
        else:
            raise ValueError("文件保存失败或文件为空")
            
    except Exception as e:
        raise ValueError(f"保存文件失败: {str(e)}")

def save_prompt_files_sync(prompt_files, prompt_folder):
    """在主线程中同步保存提示词文件"""
    saved_paths = {}
    for prompt_type, file_storage in prompt_files.items():
        if file_storage and file_storage.filename:
            try:
                # 为提示词文件生成唯一名称
                timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                original_name = Path(file_storage.filename).stem
                target_filename = f"{prompt_type}_{timestamp}_{original_name}.md"
                target_path = save_uploaded_file(
                    file_storage, 
                    prompt_folder, 
                    target_filename
                )
                saved_paths[prompt_type] = target_path
                print(f"已保存提示词文件: {target_filename}")
            except Exception as e:
                print(f"保存提示词文件 {prompt_type} 失败: {e}")
                # 继续处理，使用默认提示词
    return saved_paths

def process_excel_task(job_id, excel_path, prompt_files, config_data):
    """后台处理任务"""
    logger = WebLogger(job_id)
    
    try:
        processing_status[job_id] = {'status': 'processing', 'message': '开始处理...', 'progress': 10}
        logger.info(f"开始处理Excel文件: {excel_path}")
        
        # 验证文件是否存在且可读
        if not excel_path.exists():
            raise FileNotFoundError(f"Excel文件不存在: {excel_path}")
        
        # 保存配置
        save_config(config_data)
        
        # 如果用户上传了提示词文件，创建临时配置使用上传的提示词
        temp_config_data = config_data.copy()
        if prompt_files:
            # 创建临时配置文件，使用上传的提示词
            temp_config_path = Path(f"temp_config_{job_id}.json")
            
            # 更新配置使用上传的提示词文件
            if 'test_point' in prompt_files:
                temp_config_data['file']['test_point_prompt_file'] = str(prompt_files['test_point'])
            if 'test_case' in prompt_files:
                temp_config_data['file']['test_case_prompt_file'] = str(prompt_files['test_case'])
            
            # 保存临时配置
            with open(temp_config_path, 'w', encoding='utf-8') as f:
                json.dump(temp_config_data, f, ensure_ascii=False, indent=4)
            
            config_to_use = temp_config_path
        else:
            config_to_use = get_config_path()
        
        # 加载配置
        logger.info("加载配置...")
        settings = Settings(config_to_use)
        
        # 初始化组件
        logger.info("初始化AI组件...")
        processing_status[job_id].update({'message': '初始化AI组件...', 'progress': 20})
        
        prompt_manager = PromptManager(settings)
        llm_client = LLMClientFactory.create(settings=settings)
        data_processor = DataProcessor(llm_client, prompt_manager, settings)
        
        processing_status[job_id].update({'message': '加载Excel数据...', 'progress': 30})
        logger.info(f"加载Excel数据: {excel_path}")
        
        # 验证Excel文件
        if not excel_path.exists():
            raise FileNotFoundError(f"Excel文件不存在: {excel_path}")
        
        # 加载数据 - 使用用户上传的文件
        data_loader = DataLoaderFactory.create(settings=settings)
        raw_data = data_loader.load(excel_path)
        
        if not raw_data:
            raise ValueError("没有找到有效数据")
        
        logger.info(f"成功加载数据，共 {len(raw_data)} 个sheet")
        processing_status[job_id].update({'message': '生成测试用例...', 'progress': 50})
        
        # 处理数据
        processed_data = {}
        total_cases = 0
        
        for sheet_name, sheet_data in raw_data.items():
            logger.info(f"处理Sheet: {sheet_name}，共 {len(sheet_data)} 行数据")
            processed_sheet = data_processor.process_batch(sheet_data, sheet_name)
            processed_data[sheet_name] = processed_sheet
            total_cases += len(processed_sheet)
            
            progress = 50 + (len(processed_data) / len(raw_data)) * 40
            processing_status[job_id].update({
                'message': f'已处理 {len(processed_data)}/{len(raw_data)} 个sheet',
                'progress': min(90, progress)
            })
        
        processing_status[job_id].update({'message': '生成输出文件...', 'progress': 90})
        logger.info("生成输出Excel文件...")
        
        # 生成输出文件 - 使用配置中的输出文件名模板
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_template = settings.get("file.output_file")
        output_path_template = Path(output_template)
        output_filename = f"{output_path_template.stem}_{timestamp}{output_path_template.suffix}"
        output_path = Path(app.config['OUTPUT_FOLDER']) / output_filename
        
        excel_writer = FileWriterFactory.create(settings=settings)
        success = excel_writer.write(processed_data, output_path)
        
        if success:
            logger.info(f"处理完成！生成 {total_cases} 个测试用例")
            processing_results[job_id] = {
                'status': 'completed',
                'output_file': output_filename,
                'total_cases': total_cases,
                'message': f'成功生成 {total_cases} 个测试用例'
            }
            processing_status[job_id].update({
                'status': 'completed', 
                'message': f'处理完成！生成 {total_cases} 个测试用例',
                'progress': 100
            })
            
            # 清理临时文件
            try:
                # 清理临时配置文件
                if prompt_files:
                    temp_config_path = Path(f"temp_config_{job_id}.json")
                    if temp_config_path.exists():
                        temp_config_path.unlink()
                
                # 清理output目录的旧文件
                cleanup_old_files(app.config['OUTPUT_FOLDER'], app.config['MAX_FILES_COUNT'])
            except Exception as e:
                logger.warning(f"清理临时文件失败: {e}")
                
        else:
            raise ValueError("Excel文件生成失败")
            
    except Exception as e:
        error_msg = f"处理失败: {str(e)}"
        logger.error(error_msg)
        processing_status[job_id] = {
            'status': 'error', 
            'message': error_msg,
            'progress': 100
        }
        processing_results[job_id] = {
            'status': 'error',
            'message': error_msg
        }

@app.route('/')
def index():
    """首页"""
    return render_template('index.html')

@app.route('/config', methods=['GET', 'POST'])
def config_management():
    """配置管理页面"""
    if request.method == 'POST':
        try:
            config_data = load_config()
            
            # 更新模型配置
            config_data.setdefault('model', {})
            config_data['model']['name'] = request.form.get('model_name', 'qwen3-coder-480b-a35b')
            config_data['model']['base_url'] = request.form.get('model_base_url', '')
            config_data['model']['api_key'] = request.form.get('model_api_key', '')
            config_data['model']['temperature'] = float(request.form.get('model_temperature', 0))
            config_data['model']['max_tokens'] = int(request.form.get('model_max_tokens', 8192))
            config_data['model']['request_timeout'] = int(request.form.get('model_timeout', 300))
            config_data['model']['max_retries'] = int(request.form.get('model_retries', 3))
            
            # 更新文件配置 - 移除了输入文件名配置
            config_data.setdefault('file', {})
            # 保留原有的输入文件名，不从表单获取
            if 'input_file' not in config_data['file']:
                config_data['file']['input_file'] = '功能清单-SNHA.xlsx'
            config_data['file']['output_file'] = request.form.get('output_file', 'IVC_test_case.xlsx')
            config_data['file']['encoding'] = request.form.get('encoding', 'utf-8')
            # 设置提示词文件路径为内置的prompts文件夹
            config_data['file']['test_point_prompt_file'] = 'prompt/test_point.md'
            config_data['file']['test_case_prompt_file'] = 'prompt/test_case.md'
            
            # 更新处理配置
            config_data.setdefault('input_excel_processing', {})
            config_data['input_excel_processing']['default_threads'] = int(request.form.get('default_threads', 4))
            config_data['input_excel_processing']['header_rows'] = int(request.form.get('header_rows', 2))
            config_data['input_excel_processing']['data_start_row'] = int(request.form.get('data_start_row', 3))
            
            target_sheets = request.form.get('target_sheets', '云服务')
            config_data['input_excel_processing']['target_sheets'] = [sheet.strip() for sheet in target_sheets.split(',')]
            
            # 处理列范围
            column_start = int(request.form.get('column_start', 1))
            column_end = int(request.form.get('column_end', 4))
            config_data['input_excel_processing']['column_range'] = [column_start, column_end]
            
            # 更新输出样式配置
            config_data.setdefault('output_excel_style', {})
            config_data['output_excel_style']['font_name'] = request.form.get('font_name', '微软雅黑')
            config_data['output_excel_style']['font_size'] = int(request.form.get('font_size', 9))
            config_data['output_excel_style']['first_column_width'] = int(request.form.get('first_column_width', 12))
            config_data['output_excel_style']['other_columns_width'] = int(request.form.get('other_columns_width', 36))
            
            # 保留原有的对齐样式配置
            if 'header_row_style' not in config_data['output_excel_style']:
                config_data['output_excel_style']['header_row_style'] = {
                    "horizontal": "center",
                    "vertical": "center"
                }
            if 'first_column_style' not in config_data['output_excel_style']:
                config_data['output_excel_style']['first_column_style'] = {
                    "horizontal": "center",
                    "vertical": "center"
                }
            if 'other_columns_style' not in config_data['output_excel_style']:
                config_data['output_excel_style']['other_columns_style'] = {
                    "horizontal": "left",
                    "vertical": "center"
                }
            
            save_config(config_data)
            flash('配置已保存成功！', 'success')
            
        except Exception as e:
            flash(f'保存配置失败: {str(e)}', 'error')
    
    config_data = load_config()
    return render_template('config.html', config=config_data)

@app.route('/upload', methods=['GET', 'POST'])
def upload_files():
    """文件上传页面"""
    if request.method == 'POST':
        # 检查Excel文件
        if 'excel_file' not in request.files:
            flash('请选择Excel文件', 'error')
            return redirect(request.url)
        
        excel_file = request.files['excel_file']
        test_point_file = request.files.get('test_point_file')
        test_case_file = request.files.get('test_case_file')
        
        # 验证Excel文件
        if not excel_file or not allowed_file(excel_file.filename, {'xlsx', 'xls'}):
            flash('请上传有效的Excel文件 (.xlsx, .xls)', 'error')
            return redirect(request.url)
        
        # 验证提示词文件
        prompt_files = {}
        if test_point_file and test_point_file.filename:
            if not allowed_file(test_point_file.filename, {'md', 'txt'}):
                flash('测试点提示词文件格式不正确 (.md, .txt)', 'error')
                return redirect(request.url)
            prompt_files['test_point'] = test_point_file
        
        if test_case_file and test_case_file.filename:
            if not allowed_file(test_case_file.filename, {'md', 'txt'}):
                flash('测试用例提示词文件格式不正确 (.md, .txt)', 'error')
                return redirect(request.url)
            prompt_files['test_case'] = test_case_file
        
        try:
            # 保存Excel文件到upload/input目录
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            original_name = Path(excel_file.filename).stem
            excel_filename = f"input_{timestamp}_{original_name}.xlsx"
            excel_path = save_uploaded_file(
                excel_file, 
                app.config['UPLOAD_INPUT_FOLDER'], 
                excel_filename
            )
            
            # 在主线程中同步保存提示词文件到upload/prompt目录
            saved_prompt_files = {}
            if prompt_files:
                saved_prompt_files = save_prompt_files_sync(prompt_files, app.config['UPLOAD_PROMPT_FOLDER'])
            
            # 创建处理任务
            job_id = datetime.now().strftime("%Y%m%d_%H%M%S")
            config_data = load_config()
            
            # 启动后台线程
            thread = threading.Thread(
                target=process_excel_task,
                args=(job_id, excel_path, saved_prompt_files, config_data)
            )
            thread.daemon = True
            thread.start()
            
            flash('文件上传成功，开始处理...', 'success')
            return redirect(url_for('processing_result', job_id=job_id))
            
        except Exception as e:
            flash(f'文件上传失败: {str(e)}', 'error')
            return redirect(request.url)
    
    return render_template('upload.html')

@app.route('/result/<job_id>')
def processing_result(job_id):
    """处理结果页面"""
    status = processing_status.get(job_id, {'status': 'unknown'})
    return render_template('result.html', job_id=job_id, status=status)

@app.route('/api/status/<job_id>')
def api_status(job_id):
    """API接口：获取处理状态"""
    status = processing_status.get(job_id, {'status': 'unknown'})
    result = processing_results.get(job_id)
    
    response = {
        'job_id': job_id,
        'status': status.get('status', 'unknown'),
        'message': status.get('message', ''),
        'progress': status.get('progress', 0)
    }
    
    if result:
        response.update(result)
    
    return jsonify(response)

@app.route('/api/logs/<job_id>')
def api_logs(job_id):
    """API接口：获取处理日志"""
    logs = job_logs.get(job_id, [])
    return jsonify({'logs': logs})

@app.route('/download/<job_id>')
def download_result(job_id):
    """下载结果文件"""
    result = processing_results.get(job_id)
    if not result or result['status'] != 'completed':
        flash('文件尚未处理完成或处理失败', 'error')
        return redirect(url_for('index'))
    
    output_file = result['output_file']
    output_path = Path(app.config['OUTPUT_FOLDER']) / output_file
    
    if not output_path.exists():
        flash('输出文件不存在', 'error')
        return redirect(url_for('index'))
    
    return send_file(
        output_path,
        as_attachment=True,
        download_name=output_file
    )

if __name__ == '__main__':
    setup_logging()
    app.run(debug=False, host='0.0.0.0', port=5000)