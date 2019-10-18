#include <stdio.h>
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/knn_vhls.cpp"


extern "C" 
{
  void DigitRec( ap_uint<64>* source_wrapper_0, ap_uint<64>* source_wrapper_1, ap_uint<6>* source_wrapper_2 ) {


  #pragma HLS INTERFACE m_axi port= source_wrapper_0 offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port= source_wrapper_1 offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port= source_wrapper_2 offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port= source_wrapper_0 bundle=control
#pragma HLS INTERFACE s_axilite port= source_wrapper_1 bundle=control
#pragma HLS INTERFACE s_axilite port= source_wrapper_2 bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control


  ap_uint<64> source_wrapper_temp_1[10][1800];
  ap_uint<6> source_wrapper_temp_2[10][3];
  for ( int i0 = 0; i0 < 10; i0++) {
    for ( int i1 = 0; i1 < 1800; i1++) {
      source_wrapper_temp_1[i0][i1] = source_wrapper_1[i1+ i0*1800];
    }
  }
  for ( int i0 = 0; i0 < 10; i0++) {
    for ( int i1 = 0; i1 < 3; i1++) {
      source_wrapper_temp_2[i0][i1] = source_wrapper_1[i1+ i0*3];
    }
  }


  default_function( source_wrapper_0[0], source_wrapper_temp_1, source_wrapper_temp_2);


  for ( int i0 = 0; i0 < 10; i0++) {
    for ( int i1 = 0; i1 < 3; i1++) {
      source_wrapper_2[i1 + i0*3 ] = source_wrapper_temp_2[i0][i1];
    }
  }
}
}
