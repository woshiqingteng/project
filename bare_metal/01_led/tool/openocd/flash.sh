#!/bin/bash

$OPENOCD -f $OPENOCD_CFG -c "program $ELF_FILE verify reset exit"