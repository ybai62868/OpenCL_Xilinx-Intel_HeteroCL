#include <ap_int.h>
#include <ap_fixed.h>
#include <math.h>

void default_function(ap_uint<64> test_image, ap_uint<64> train_images[10][1800], ap_uint<6> knn_mat[10][3]) {
  for (ap_int<32> x = 0; x < 10; ++x) {
    for (ap_int<32> y = 0; y < 3; ++y) {
      knn_mat[x][y] = (ap_uint<6>)50;
    }
  }
  ap_uint<49> knn_update;
  for (ap_int<32> y1 = 0; y1 < 1800; ++y1) {
    for (ap_int<32> x1 = 0; x1 < 10; ++x1) {
    #pragma HLS pipeline
      ap_uint<6> dist;
      ap_uint<49> diff;
      diff = (train_images[x1][y1] ^ test_image);
      ap_uint<6> out;
      out = (ap_uint<6>)0;
      for (ap_int<32> i = 0; i < 49; ++i) {
        out = ((ap_uint<6>)(((ap_uint<50>)out) + ((ap_uint<50>)diff[i])));
      }
      dist = out;
      ap_uint<49> max_id;
      max_id = (ap_uint<49>)0;
      for (ap_int<32> i1 = 0; i1 < 3; ++i1) {
        if (knn_mat[((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) / (ap_int<49>)3)][((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) % (ap_int<49>)3)] < knn_mat[x1][i1]) {
          max_id = ((ap_uint<49>)i1);
        }
      }
      if (dist < knn_mat[((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) / (ap_int<49>)3)][((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) % (ap_int<49>)3)]) {
        knn_mat[((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) / (ap_int<49>)3)][((((ap_int<49>)max_id) + ((ap_int<49>)(x1 * 3))) % (ap_int<49>)3)] = dist;
      }
    }
  }
}

