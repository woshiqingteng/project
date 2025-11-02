// 主JavaScript文件 - 与本地Bootstrap集成
class FluentUI {
    static initFileUpload() {
        const fileInputs = document.querySelectorAll('input[type="file"]');
        
        fileInputs.forEach(input => {
            const container = input.closest('.file-upload-area');
            if (!container) return;
            
            const label = container.querySelector('.file-upload-label');
            
            // 点击区域触发文件选择
            container.addEventListener('click', (e) => {
                if (e.target !== input) {
                    input.click();
                }
            });
            
            // 拖放功能
            container.addEventListener('dragover', (e) => {
                e.preventDefault();
                container.classList.add('dragover');
            });
            
            container.addEventListener('dragleave', () => {
                container.classList.remove('dragover');
            });
            
            container.addEventListener('drop', (e) => {
                e.preventDefault();
                container.classList.remove('dragover');
                input.files = e.dataTransfer.files;
                this.updateFileLabel(input, label);
            });
            
            // 文件选择变化
            input.addEventListener('change', () => {
                this.updateFileLabel(input, label);
            });
        });
    }
    
    static updateFileLabel(input, label) {
        if (input.files.length > 0) {
            const file = input.files[0];
            label.textContent = `已选择: ${file.name}`;
            label.style.color = '#107c10';
        } else {
            label.textContent = label.dataset.originalText || '点击或拖拽文件到这里';
            label.style.color = '';
        }
    }
    
    static initTooltips() {
        // 初始化Bootstrap工具提示
        const tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
        const tooltipList = tooltipTriggerList.map(function (tooltipTriggerEl) {
            return new bootstrap.Tooltip(tooltipTriggerEl);
        });
    }
}

// 处理状态监控类
class JobMonitor {
    constructor(jobId) {
        this.jobId = jobId;
        this.statusInterval = null;
        this.logInterval = null;
    }
    
    startMonitoring() {
        this.updateStatus();
        this.updateLogs();
        
        this.statusInterval = setInterval(() => this.updateStatus(), 2000);
        this.logInterval = setInterval(() => this.updateLogs(), 1500);
    }
    
    stopMonitoring() {
        if (this.statusInterval) clearInterval(this.statusInterval);
        if (this.logInterval) clearInterval(this.logInterval);
    }
    
    async updateStatus() {
        try {
            const response = await fetch(`/api/status/${this.jobId}`);
            const data = await response.json();
            
            this.updateProgressBar(data.progress);
            this.updateStatusMessage(data.message);
            
            if (data.status === 'completed' || data.status === 'error') {
                this.handleCompletion(data);
            }
        } catch (error) {
            console.error('获取状态失败:', error);
        }
    }
    
    async updateLogs() {
        try {
            const response = await fetch(`/api/logs/${this.jobId}`);
            const data = await response.json();
            
            this.updateConsole(data.logs);
        } catch (error) {
            console.error('获取日志失败:', error);
        }
    }
    
    updateProgressBar(progress) {
        const progressBar = document.getElementById('progress-bar');
        if (progressBar) {
            progressBar.style.width = `${progress}%`;
            const progressText = document.getElementById('progress-text');
            if (progressText) {
                progressText.textContent = `${progress}%`;
            }
        }
    }
    
    updateStatusMessage(message) {
        const statusElement = document.getElementById('status-message');
        if (statusElement) {
            statusElement.textContent = message;
        }
    }
    
    updateConsole(logs) {
        const consoleElement = document.getElementById('console-output');
        if (!consoleElement) return;
        
        // 只有当有新日志时才更新，避免频繁重绘
        if (logs.length === consoleElement.childElementCount) {
            return;
        }
        
        consoleElement.innerHTML = '';
        
        logs.forEach(log => {
            const line = document.createElement('div');
            line.className = 'console-line';
            
            if (log.includes('ERROR')) {
                line.className += ' console-error';
            } else if (log.includes('WARNING')) {
                line.className += ' console-warning';
            } else if (log.includes('INFO')) {
                line.className += ' console-info';
            } else {
                line.className += ' console-success';
            }
            
            line.textContent = log;
            consoleElement.appendChild(line);
        });
        
        // 自动滚动到底部
        consoleElement.scrollTop = consoleElement.scrollHeight;
    }
    
    handleCompletion(data) {
        this.stopMonitoring();
        
        const resultElement = document.getElementById('result-info');
        const errorElement = document.getElementById('error-info');
        
        if (data.status === 'completed') {
            if (resultElement) {
                resultElement.style.display = 'block';
                const completionMessage = document.getElementById('completion-message');
                if (completionMessage) {
                    completionMessage.textContent = data.message;
                }
                const downloadLink = document.getElementById('download-link');
                if (downloadLink) {
                    downloadLink.href = `/download/${this.jobId}`;
                }
            }
        } else if (data.status === 'error') {
            if (errorElement) {
                errorElement.style.display = 'block';
                const errorMessage = document.getElementById('error-message');
                if (errorMessage) {
                    errorMessage.textContent = data.message;
                }
            }
        }
    }
}

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', function() {
    FluentUI.initFileUpload();
    FluentUI.initTooltips();
    
    // 如果有任务ID，启动监控
    const jobIdElement = document.getElementById('job-id');
    if (jobIdElement) {
        const jobId = jobIdElement.value;
        const monitor = new JobMonitor(jobId);
        monitor.startMonitoring();
    }
    
    // 表单验证
    const forms = document.querySelectorAll('form');
    forms.forEach(form => {
        form.addEventListener('submit', function(e) {
            const requiredInputs = form.querySelectorAll('[required]');
            let valid = true;
            
            requiredInputs.forEach(input => {
                if (!input.value.trim()) {
                    valid = false;
                    input.style.borderColor = '#d13438';
                    
                    // 添加错误提示
                    let errorElement = input.parentNode.querySelector('.form-error');
                    if (!errorElement) {
                        errorElement = document.createElement('div');
                        errorElement.className = 'form-error text-danger mt-1';
                        errorElement.textContent = '此字段为必填项';
                        input.parentNode.appendChild(errorElement);
                    }
                } else {
                    input.style.borderColor = '';
                    
                    // 移除错误提示
                    const errorElement = input.parentNode.querySelector('.form-error');
                    if (errorElement) {
                        errorElement.remove();
                    }
                }
            });
            
            if (!valid) {
                e.preventDefault();
            }
        });
    });
    
    // 实时表单验证
    const inputs = document.querySelectorAll('input[required]');
    inputs.forEach(input => {
        input.addEventListener('blur', function() {
            if (!this.value.trim()) {
                this.style.borderColor = '#d13438';
            } else {
                this.style.borderColor = '';
            }
        });
    });
});