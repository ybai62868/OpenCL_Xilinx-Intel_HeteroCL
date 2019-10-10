//==========================================================================
// digitrec.cpp
//==========================================================================
// @brief: A k-nearest-neighbor kernel for digit recognition 

// As for sw_emu, it only copy digitrec.cpp to a separate directory and only search in it...
// That why I'm using the absolute path...
// Please change these two absolute paths to pass the sw_emu
// #include "/home/centos/src/project_data/aws_ap6/AWS_Heterocl/solution/src/host/typedefs.h"
// #include "/home/centos/src/project_data/aws_ap6/AWS_Heterocl/solution/src/kernel/functions.cpp"
// #include "/home/centos/src/project_data/aws_ap6/AWS_Heterocl/solution/src/kernel/default_function.cpp"
// #include "/home/centos/src/project_data/aws_ap6/AWS_Heterocl/solution/src/kernel/vhls.cpp"
// #include "./vhls.cpp"
// #include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/default_function.cpp"
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/knn_vhls.cpp"
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/functions.cpp"
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/host/typedefs.h"
#include <stdio.h>

extern "C" 
{
  //----------------------------------------------------------
  // Digitrec
  //----------------------------------------------------------
  // The is the top function of the kernel, reads in the training 
  // data and testing data, return an array of the recognized labels
  //
  // @param[in] : input - the testing instance
  // @return : the recognized digit (0~9)
  void DigitRec( ap_uint<64>* testing_data, ap_uint<64>* training_data, ap_uint<6>* results) 
  {  
    // connection settings:
    // students DO NOT need to understand this
    #pragma HLS INTERFACE m_axi port=training_data offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=testing_data offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=results offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=training_data bundle=control
    #pragma HLS INTERFACE s_axilite port=testing_data bundle=control
    #pragma HLS INTERFACE s_axilite port=results bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
    // This array stores K minimum distances per training set
    ap_uint<6> knn_mat[10][3];
    ap_uint<64> training_instance[10][NUM_TRAINING];

    // printf("%lu\n", (long int)testing_data[0]);
    // for training data
    for ( int i = 0;i < 10; i++ ) {
      for ( int j = 0;j < NUM_TRAINING; j++ ) {
	training_instance[i][j] = training_data[i * NUM_TRAINING + j];
      }
    }
    // for knn_mat
    for ( int i = 0;i < 10; i++ ) {
	for ( int j = 0;j < K_CONST; j++ ) {
	  knn_mat[i][j] = results[i * K_CONST + j];		
	}
    }
    default_function(testing_data[0], training_instance, knn_mat);
//    printf("after calculating, knn_mat(2d)\n");   
    for (int i = 0;i < 10;i++ ) {
	for (int j = 0;j < K_CONST;j++ ) {
		results[i * K_CONST + j] = knn_mat[i][j];
//		printf("%d ", (int)knn_mat[i][j]);
		}
//		printf("\n");
	}
//    printf("after calculating, results(1d)\n");   
//    for (int i = 0;i < 10;i++ ) {
//	for (int j = 0;j < K_CONST;j++ ) {
//		printf("%d ", (int)results[i * K_CONST + j]);
//		}
//		printf("\n");
//	}  
  }

}
