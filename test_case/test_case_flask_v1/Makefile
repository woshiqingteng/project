VENV_DIR := venv
REQ_FILE := requirements.txt
PYTHON = python
PYTHON_VENV = $(VENV_DIR)/Scripts/python
PIP = $(VENV_DIR)/Scripts/pip
SCRIPT = app.py
PYINSTALLER = $(VENV_DIR)/Scripts/pyinstaller
SPEC = pyinstaller.spec
CLEAN_DIR = build

.PHONY: all venv install freeze run build clean

all: venv install run

venv: $(VENV_DIR)/Scripts/activate
$(VENV_DIR)/Scripts/activate:
	@echo "[VENV] Creating Python virtual environment..."
	@$(PYTHON) -m venv $(VENV_DIR)

install: venv
	@echo "[INSTALL] Installing dependencies..."
	@$(PYTHON_VENV) -m pip install --upgrade pip
	@$(PIP) install -r $(REQ_FILE)

freeze:
	@echo "[FREEZE] Freezing dependencies to $(REQ_FILE)..."
	@$(PIP) freeze > $(REQ_FILE)

run:
	@echo "[RUN] Executing scripts..."
	@$(PYTHON_VENV) $(SCRIPT)

build:
	@echo "[Build] Build scripts..."
	@$(PYINSTALLER) $(SPEC) --log-level=WARN

clean:
	@echo "[CLEAN] Removing virtual environment..."
	@rm -rf $(CLEAN_DIR)