//==========================================================================
// digitrec.cpp
//==========================================================================
// @brief: A k-nearest-neighbor kernel for digit recognition 

// As for sw_emu, it only copy digitrec.cpp to a separate directory and only search in it...
// That why I'm using the absolute path...
// Please change these two absolute paths to pass the sw_emu
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/host/typedefs.h"
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/functions.cpp"
//#include "./functions.cpp"

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
  void DigitRec( digit* training_data, digit* testing_data, bit4_t* results) 
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
    bit6_t knn_set[10 * K_CONST];

    // for each of the test data
    L180: for ( int k = 0 ; k < NUM_TEST; k++){
      digit testing_instance = testing_data[k];
      // Initialize the knn set
      for ( int i = 0; i < 10 * K_CONST; i++ )
          // Note that the max distance is 49
          knn_set[i] = 50; 

      // for each of the training data
      L1800: for ( int i = 0; i < NUM_TRAINING; i++ ){
        L10: for ( int j = 0; j < 10; j++ ){
        digit training_instance =  training_data[j* NUM_TRAINING + i];
        // Update the KNN set
        update_knn( testing_instance, training_instance, &knn_set[j * K_CONST] );
        }
      } 
      // collect the results
      results[k] = knn_vote(knn_set);
    }
  }

}
