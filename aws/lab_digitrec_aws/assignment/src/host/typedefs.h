/*===============================================================*/
/*                                                               */
/*                          typedefs.h                           */
/*                                                               */
/*           Constant definitions and typedefs for host.         */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// include the VIVADO_HLS ap_int header
#include "ap_int.h"

typedef ap_uint<4> bit4_t;  // label type
typedef ap_uint<6> bit6_t;  // distance type
// data type, i.e. training_data and testing_data
// we only use 49 bits for the data, however the 
// AXI Interface does not support data type whose
// length is 49, therefore we are using a 64 bit
// ap_uint to store the data
typedef ap_uint<64> digit; 

// training size and testing size
#define NUM_TRAINING 1800
#define NUM_TEST 180

#include <string>
// target device
// ECE Server: OCL_DEVICE=xilinx:adm-pcie-7v3:1ddr:3.0
const std::string TARGET_DEVICE = "xilinx_aws-vu9p-f1-04261818_dynamic_5_0";


#endif
