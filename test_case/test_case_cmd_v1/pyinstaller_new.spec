# -*- mode: python ; coding: utf-8 -*-

import sys
import os
from pathlib import Path

block_cipher = None

# 获取当前工作目录
current_dir = Path(os.getcwd())

# 设置输出目录为默认的dist目录（不自定义，使用PyInstaller默认设置）
# distpath = current_dir / 'dist'  # PyInstaller默认，无需显式设置

# 设置构建目录为默认的build目录（不自定义，使用PyInstaller默认设置）  
# workpath = current_dir / 'build'  # PyInstaller默认，无需显式设置

# 主程序入口
main_script = 'src/main.py'

# 应用程序名称
app_name = 'excel_ai_processor'

# Python路径 - 添加项目根目录和src目录
pathex = [
    os.getcwd(),  # 项目根目录
    os.path.join(os.getcwd(), 'src')  # src目录
]

# 二进制文件 - 为空，不包含特殊二进制文件
binaries = []

# 数据文件 - 为空，不包含任何配置或资源文件
datas = []

# 隐藏导入 - 根据项目依赖添加
hiddenimports = [
    'pandas',
    'openpyxl',
    'langchain_openai',
    'langchain_core',
    'tenacity'
]

# Hook路径
hookspath = []

# Hook配置
hooksconfig = {}

# 运行时Hook
runtime_hooks = []

# 排除模块 - 减少打包体积
excludes = [
    'matplotlib',  # 图形库，不需要
    'scipy',       # 科学计算，不需要  
    'tkinter',     # GUI库，不需要
    'pytest',      # 测试框架
    'unittest',    # 测试框架
    'html',        # HTML处理
    'jsonschema',  # JSON模式验证
]

# Windows特定配置
win_no_prefer_redirects = False
win_private_assemblies = False

# 加密 - 不加密
cipher = block_cipher

# 不打包为归档
noarchive = False

# 分析阶段 - 设置console=True显示控制台，便于查看错误信息
a = Analysis(
    [main_script],
    pathex=pathex,
    binaries=binaries,
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=hookspath,
    hooksconfig=hooksconfig,
    runtime_hooks=runtime_hooks,
    excludes=excludes,
    win_no_prefer_redirects=win_no_prefer_redirects,
    win_private_assemblies=win_private_assemblies,
    cipher=cipher,
    noarchive=noarchive
)

# PYZ（Python Zlib）
pyz = PYZ(a.pure, a.zipped_data, cipher=cipher)

# EXE配置 - 单文件模式
exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    name=app_name,
    debug=False,  # 不调试模式
    bootloader_ignore_signals=False,
    strip=False,  # 不剥离符号
    upx=True,     # 使用UPX压缩
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,  # 显示控制台窗口
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None
)