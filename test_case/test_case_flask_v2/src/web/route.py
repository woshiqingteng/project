"""
路由处理函数
"""

import json
import threading
from datetime import datetime
from pathlib import Path
from flask import render_template, request, redirect, url_for, flash, send_file, jsonify

from .blueprint import api_blueprint, config_blueprint, upload_blueprint, result_blueprint
from ..core.dependency_injector import get_container
from ..util.logger_util import get_logger

logger = get_logger(__name__)

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
        logger.info(f"[{self.job_id}] {message}")
    
    def error(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] ERROR: {message}"
        job_logs[self.job_id].append(log_entry)
        logger.error(f"[{self.job_id}] {message}")

def allowed_file(filename, allowed_extensions=None):
    """检查文件扩展名是否允许"""
    if allowed_extensions is None:
        allowed_extensions = {'xlsx', 'xls', 'md', 'txt'}
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in allowed_extensions

def process_excel_task(job_id, excel_path, prompt_files, config_data):
    """后台处理任务"""
    container = get_container()
    logger = WebLogger(job_id)
    
    try:
        processing_status[job_id] = {'status': 'processing', 'message': '开始处理...', 'progress': 10}
        logger.info(f"开始处理Excel文件: {excel_path}")
        
        if not excel_path.exists():
            raise FileNotFoundError(f"Excel文件不存在: {excel_path}")
        
        processing_status[job_id].update({'message': '加载Excel数据...', 'progress': 30})
        logger.info(f"加载Excel数据: {excel_path}")
        
        data_loader = container.data_loader
        raw_data = data_loader.load(excel_path)
        
        if not raw_data:
            raise ValueError("没有找到有效数据")
        
        logger.info(f"成功加载数据，共 {len(raw_data)} 个sheet")
        
        processing_status[job_id].update({'message': '生成测试用例...', 'progress': 50})
        
        data_processor = container.data_processor
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
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_template = container.config.get("file.output_file")
        output_path_template = Path(output_template)
        output_filename = f"{output_path_template.stem}_{timestamp}{output_path_template.suffix}"
        
        output_dir = container.config.get_file_path("output_dir")
        output_path = output_dir / output_filename
        
        excel_writer = container.file_writer
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

# 配置管理路由
@config_blueprint.route('/config', methods=['GET', 'POST'])
def config_management():
    """配置管理页面"""
    if request.method == 'POST':
        try:
            container = get_container()
            config_data = container.config._config.copy()
            
            config_data.setdefault('model', {})
            config_data['model']['name'] = request.form.get('model_name', 'qwen3-coder-480b-a35b')
            config_data['model']['base_url'] = request.form.get('model_base_url', '')
            config_data['model']['api_key'] = request.form.get('model_api_key', '')
            config_data['model']['temperature'] = float(request.form.get('model_temperature', 0))
            config_data['model']['max_tokens'] = int(request.form.get('model_max_tokens', 8192))
            config_data['model']['request_timeout'] = int(request.form.get('model_timeout', 300))
            config_data['model']['max_retries'] = int(request.form.get('model_retries', 3))
            
            config_data.setdefault('file', {})
            config_data['file']['output_file'] = request.form.get('output_file', 'IVC_test_case.xlsx')
            config_data['file']['encoding'] = request.form.get('encoding', 'utf-8')
            config_data['file']['upload_dir'] = request.form.get('upload_dir', 'upload')
            config_data['file']['output_dir'] = request.form.get('output_dir', 'output')
            
            with open(container.config._config_path, 'w', encoding='utf-8') as f:
                json.dump(config_data, f, ensure_ascii=False, indent=4)
            
            flash('配置已保存成功！', 'success')
            
        except Exception as e:
            flash(f'保存配置失败: {str(e)}', 'error')
    
    container = get_container()
    config_data = container.config._config
    return render_template('config.html', config=config_data)

# 文件上传路由
@upload_blueprint.route('/upload', methods=['GET', 'POST'])
def upload_file():
    """文件上传页面"""
    if request.method == 'POST':
        container = get_container()
        
        if 'excel_file' not in request.files:
            flash('请选择Excel文件', 'error')
            return redirect(request.url)
        
        excel_file = request.files['excel_file']
        
        if not excel_file or not allowed_file(excel_file.filename, {'xlsx', 'xls'}):
            flash('请上传有效的Excel文件 (.xlsx, .xls)', 'error')
            return redirect(request.url)
        
        try:
            upload_dir = container.config.get_file_path("upload_dir")
            input_dir = upload_dir / "input"
            input_dir.mkdir(parents=True, exist_ok=True)
            
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            original_name = Path(excel_file.filename).stem
            excel_filename = f"input_{timestamp}_{original_name}.xlsx"
            excel_path = input_dir / excel_filename
            excel_file.save(excel_path)
            
            job_id = datetime.now().strftime("%Y%m%d_%H%M%S")
            config_data = container.config._config
            
            thread = threading.Thread(
                target=process_excel_task,
                args=(job_id, excel_path, {}, config_data)
            )
            thread.daemon = True
            thread.start()
            
            flash('文件上传成功，开始处理...', 'success')
            return redirect(url_for('result.processing_result', job_id=job_id))
            
        except Exception as e:
            flash(f'文件上传失败: {str(e)}', 'error')
            return redirect(request.url)
    
    return render_template('upload.html')

# 结果查看路由
@result_blueprint.route('/result/<job_id>')
def processing_result(job_id):
    """处理结果页面"""
    status = processing_status.get(job_id, {'status': 'unknown'})
    return render_template('result.html', job_id=job_id, status=status)

# API 路由
@api_blueprint.route('/status/<job_id>')
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

@api_blueprint.route('/logs/<job_id>')
def api_logs(job_id):
    """API接口：获取处理日志"""
    logs = job_logs.get(job_id, [])
    return jsonify({'logs': logs})

@result_blueprint.route('/download/<job_id>')
def download_result(job_id):
    """下载结果文件"""
    container = get_container()
    result = processing_results.get(job_id)
    
    if not result or result['status'] != 'completed':
        flash('文件尚未处理完成或处理失败', 'error')
        return redirect(url_for('upload.upload_file'))
    
    output_file = result['output_file']
    output_dir = container.config.get_file_path("output_dir")
    output_path = output_dir / output_file
    
    if not output_path.exists():
        flash('输出文件不存在', 'error')
        return redirect(url_for('upload.upload_file'))
    
    return send_file(
        output_path,
        as_attachment=True,
        download_name=output_file
    )