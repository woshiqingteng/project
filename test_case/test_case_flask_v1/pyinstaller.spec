# -*- mode: python ; coding: utf-8 -*-

import os
import sys
from pathlib import Path

# 获取当前工作目录
current_dir = Path(os.getcwd())

# 主程序入口
main_script = 'app.py'

# 应用程序名称
app_name = 'excel_ai_processor'

# Python路径
pathex = [os.getcwd()]

# 二进制文件
binaries = []

# 数据文件
datas = [
    ('config.json', '.'),
    ('prompt', 'prompt'),
    ('static', 'static'),
    ('template', 'template')
]

# 隐藏导入
hiddenimports = [
    'pandas',
    'openpyxl',
    'langchain_openai', 
    'langchain_core',
    'flask'
]

# Hook路径
hookspath = []

# Hook配置
hooksconfig = {}

# 运行时Hook
runtime_hooks = []

# 排除模块
excludes = []

# Windows特定配置
win_no_prefer_redirects = False
win_private_assemblies = False

# 加密
cipher = None

# 不打包为归档
noarchive = False

# 分析
a = Analysis(
    [main_script],
    pathex=pathex,
    binaries=binaries,
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=hookspath,
    hooksconfig=hookspath,
    runtime_hooks=runtime_hooks,
    excludes=excludes,
    win_no_prefer_redirects=win_no_prefer_redirects,
    win_private_assemblies=win_private_assemblies,
    cipher=cipher,
    noarchive=noarchive,
)

# PYZ
pyz = PYZ(a.pure, a.zipped_data, cipher=cipher)

# EXE配置
exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    name=app_name,
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)