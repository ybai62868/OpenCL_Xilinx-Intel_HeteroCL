//==========================================================================
// functions.cpp
//==========================================================================
// @brief: the two functions that you need to implement: update_knn and knn_vote

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// include the VIVADO_HLS ap_int header
#include "ap_int.h"

typedef unsigned char bit4_t;  // label type
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
    // Compute the difference using XOR
    digit diff = test_inst ^ train_inst;
    // Count the number of set bits
    // Note that we use ap_uint 64 to store the training data
    // but only 49 bits are used.
    bit6_t dist = 0;
    for ( int i = 0; i < 49; i++ ) { 
      dist += diff[i];
    }
    // Find the max distance and its index
    bit6_t max_dist = 0;
    int max_dist_id = K_CONST+1; 
    for ( int k = 0; k < K_CONST; k++ ) {
      if ( min_distances[k] > max_dist ) {
        max_dist = min_distances[k];
        max_dist_id = k;
      }
    }
    // Replace the max entry with the new min distance
    if ( dist < max_dist )
      min_distances[max_dist_id] = dist;
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
    // This array keeps the occurences of each label(votes)
    int score[10]; 
    // Initialize score array  
    for ( int i = 0; i < 10; i++ )
        score[i] = 0; 

    // Find the K nearest neighbors
    for ( int k = 0; k < K_CONST; k++ ) { 
      bit6_t min_dist = 50;
      bit4_t min_dist_id = 10;
      int  min_dist_record = K_CONST + 1;
      // find the min distance in knn_set[10 * K_CONST]
      for ( int i = 0; i < 10; i++ ) {
        for (int j = 0; j < K_CONST; j++ ) {
          if ( knn_set[i* K_CONST + j] < min_dist ) {
            min_dist = knn_set[i* K_CONST + j];
            min_dist_id = i;
            min_dist_record = j;
          }
        }
      }
      // record this neighbor's label
      score[min_dist_id]++;
      // Erase the minimum difference entry once it's recorded
      knn_set[min_dist_id * K_CONST + min_dist_record] = 50;
    }

    // Calculate the maximum score
    int max_score = 0; 
    bit4_t min_index = 0;
    for ( int i = 0; i < 10; ++i ) {
      if ( score[i] > max_score ) {
        max_score = score[i];
        min_index = i;
      }
    }
    return min_index;
  }
