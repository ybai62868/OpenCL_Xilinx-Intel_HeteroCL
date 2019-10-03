# Modify the following few settings according to your installation.
export ALTERA_PATH=/work/zhang-x1/common/tools/altera-opencl/intelFPGA_pro
export AOCL_VERSION=18.0
export BOARD=a10_ref

#Usually no need to change this, except when you use a different opencl platform for Altera FPGA
export AOCL_PLATFORM_NAME="Intel(R) FPGA SDK for OpenCL(TM)"

# Altera OpenCL related setting
export ALTERAOCLSDKROOT=$ALTERA_PATH/$AOCL_VERSION/hld
export INTELFPGAOCLSDKROOT=$ALTERAOCLSDKROOT
export AOCL_BOARD_PACKAGE_ROOT=$ALTERAOCLSDKROOT/board/${BOARD}
source $ALTERAOCLSDKROOT/init_opencl.sh
export AOCL_BOARD_LIB=altera_${BOARD}_mmd
export AOCL_SO=$ALTERAOCLSDKROOT/host/linux64/lib/libalteracl.so
export AOCL_BOARD_SO=$ALTERAOCLSDKROOT/board/${BOARD}/linux64/lib/libaltera_${BOARD}_mmd.so
export QSYS_ROOTDIR=$ALTERAOCLSDKROOT/../qsys/bin
export QUARTUS_ROOTDIR_OVERRIDE=$ALTERAOCLSDKROOT/../quartus
export LD_LIBRARY_PATH=$HALIDE_ROOT/bin:$ALTERAOCLSDKROOT/host/linux64/lib:$AOCL_BOARD_PACKAGE_ROOT/linux64/lib:$LD_LIBRARY_PATH
export PATH=$HALIDE_ROOT/bin:$QUARTUS_ROOTDIR_OVERRIDE/bin:$ALTERAOCLSDKROOT/bin:$PATH
unset CL_CONTEXT_EMULATOR_DEVICE_ALTERA
set CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1
