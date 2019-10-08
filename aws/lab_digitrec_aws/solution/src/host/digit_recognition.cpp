/*===============================================================*/
/*                                                               */
/*                    digit_recognition.cpp                      */
/*                                                               */
/*   Main host function for the Digit Recognition application.   */
/*                                                               */
/*===============================================================*/

// standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <time.h>
#include <sys/time.h>


// opencl harness headers
#include "CLWorld.h"
#include "CLKernel.h"
#include "CLMemObj.h"
// harness namespace
using namespace rosetta;


// other headers
#include "utils.h"
#include "typedefs.h"
#include "check_result.h"

// data
#include "training_data.h"
#include "testing_data.h"

int main(int argc, char ** argv) 
{
  printf("Digit Recognition Application\n");
  printf("K_CONST is set to %d\n", K_CONST);

  // parse command line arguments for opencl version
  std::string kernelFile("");
  parse_sdaccel_command_line_args(argc, argv, kernelFile);

  // timers
  struct timeval start, end;

  // create space for the result
  bit4_t* results = new bit4_t[NUM_TEST];

  // create OpenCL world
  CLWorld digit_rec_world = CLWorld(TARGET_DEVICE, CL_DEVICE_TYPE_ACCELERATOR);

  // add the bitstream file
  digit_rec_world.addProgram(kernelFile);

  // create kernels
  CLKernel DigitRec(digit_rec_world.getContext(), digit_rec_world.getProgram(), "DigitRec", digit_rec_world.getDevice());

  // create mem objects
  CLMemObj training_mem ( (void*)training_data,  sizeof(digit),  NUM_TRAINING * 10 , CL_MEM_READ_ONLY);
  CLMemObj testing_mem  ( (void*)testing_data ,  sizeof(digit),  NUM_TEST          , CL_MEM_READ_ONLY);
  CLMemObj result_mem   ( (void*)results      ,  sizeof(bit4_t), NUM_TEST          , CL_MEM_WRITE_ONLY);

  // start timer
  gettimeofday(&start, 0);

  // add them to the world
  // added in correct order, each of them can be referenced by an index
  digit_rec_world.addMemObj(training_mem);
  digit_rec_world.addMemObj(testing_mem);
  digit_rec_world.addMemObj(result_mem);

  // set work size
  int global_size[3] = {1, 1, 1};
  int local_size[3] = {1, 1, 1};
  DigitRec.set_global(global_size);
  DigitRec.set_local(local_size);

  // add them to the world
  digit_rec_world.addKernel(DigitRec);

  // set kernel arguments
  digit_rec_world.setMemKernelArg(0, 0, 0);
  digit_rec_world.setMemKernelArg(0, 1, 1);
  digit_rec_world.setMemKernelArg(0, 2, 2);

  // run
  digit_rec_world.runKernels();

  // read the data back
  digit_rec_world.readMemObj(2);

  // end timer
  gettimeofday(&end, 0);

  // check results
  printf("Checking results:\n");
  check_results( results, expected, NUM_TEST );
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  digit_rec_world.releaseWorld();
  delete []results;

  return EXIT_SUCCESS;

}
