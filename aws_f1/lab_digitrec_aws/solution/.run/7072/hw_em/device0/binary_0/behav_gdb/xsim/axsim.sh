#!/bin/bash

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/opt/Xilinx/Vivado/2018.2.op2258646/patches/AR71275/vivado/data/../lib/lnx64.o:/opt/Xilinx/Vivado/2018.2.op2258646/patches/AR71275/vivado/data/../lib/lnx64.o/Default:/opt/Xilinx/Vivado/2018.2.op2258646/data/../lib/lnx64.o:/opt/Xilinx/Vivado/2018.2.op2258646/data/../lib/lnx64.o/Default

xsim.dir/emu_wrapper_behav/axsim $@

if [ $? -ne 0 ] ; then
  echo "FATAL ERROR: Simulation exited unexpectantly"
fi
