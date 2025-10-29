# -*- mode: python ; coding: utf-8 -*-

import os
import sys
from pathlib import Path

block_cipher = None

# 获取当前工作目录
current_dir = Path(os.getcwd())

# 设置输出目录为当前目录下的release目录
distpath = current_dir / 'release'

# 设置构建目录为当前目录下的build目录
workpath = current_dir / 'build'

# 主程序入口
main_script = 'src/main.py'

# 应用程序名称
app_name = 'excel_ai_processor'

# Python路径
pathex = [os.getcwd()]

# 二进制文件
binaries = []

# 数据文件 - 不包含任何文件
datas = []

# 隐藏导入
hiddenimports = [
    'openpyxl',
    'langchain_openai', 
    'langchain_core',
    'tenacity',
    'pandas',
    'config.settings'
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
cipher = block_cipher

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
    workpath=str(workpath)
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
    distpath=str(distpath)
)