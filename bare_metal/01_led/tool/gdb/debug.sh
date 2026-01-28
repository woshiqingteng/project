#!/bin/bash

## openocd
$OPENOCD -f $OPENOCD_CFG &
OCPID=$!

## wait
sleep 1

$GDB -q -x $GDBINIT $ELF_FILE

## kill
kill $OCPID