/*===============================================================*/
/*                                                               */
/*                       testing_data.h                          */
/*                                                               */
/*              Constant array for test instances.               */
/*                                                               */
/*===============================================================*/


#ifndef TESTING_DATA_H
#define TESTING_DATA_H

#include "typedefs.h"

// testing data
const digit testing_data[NUM_TEST] = {
  #include "data/testing_set.dat"
};

// expected labels for testing data
const bit4_t expected[NUM_TEST] = {
  #include "data/expected.dat"
};

#endif
