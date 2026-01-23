#!/bin/bash

## openocd
$OPENOCD -f $OPENOCD_CFG &
OCPID=$!

## wait
sleep 1

$GDB -batch -x $GDBINIT $ELF_FILE

## kill
kill $OCPID