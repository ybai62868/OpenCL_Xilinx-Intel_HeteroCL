//==========================================================================
// functions.cpp
//==========================================================================
// @brief: the two functions that you need to implement: update_knn and knn_vote

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

  //-----------------------------------------------------------------------
  // update_knn function
  //-----------------------------------------------------------------------
  // Given a test instance and a training instance, this
  // function maintains/updates an array of K minimum
  // distances per training set.

  // @param[in] : test_inst - the testing instance
  // @param[in] : train_inst - the training instance
  // @param[in/out] : min_distances[K_CONST] - the array that stores the current
  //                  K_CONST minimum distance values per training set

  void update_knn( digit test_inst, digit train_inst, bit6_t min_distances[K_CONST] )
  {
    // -----------------------------
    // YOUR CODE GOES HERE
    // -----------------------------
  }


  //-----------------------------------------------------------------------
  // knn_vote function
  //-----------------------------------------------------------------------
  // Given 10 * K minimum distance values, this function 
  // finds the actual K nearest neighbors and determines the
  // final output based on the most common digit represented by 
  // these nearest neighbors (i.e., a vote among KNNs). 
  //
  // @param[in] : knn_set - 10 * K_CONST min distance values
  // @return : the digit label with most votes
  bit4_t knn_vote( bit6_t knn_set[10 * K_CONST] )
  {
    // -----------------------------
    // YOUR CODE GOES HERE
    // -----------------------------
  }
