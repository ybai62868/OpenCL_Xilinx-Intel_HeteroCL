#include <sys/ipc.h>
#include <sys/shm.h>


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


//other headers
#include "utils.h"
int main(int argc, char ** argv) {
  uint64_t arg_0 = (uint64_t)277042299904;
  uint64_t arg_top_0 = (uint64_t)(arg_0);
  uint64_t fool_0[1] = { arg_top_0 };


  uint64_t* arg_1 = (uint64_t*)shmat(1966140, nullptr, 0);
  uint64_t arg_top_1[10 * 1800];
  for (size_t i0 = 0; i0 < 10; i0++) {
    for (size_t i1 = 0; i1 < 1800; i1++) {
      arg_top_1[i1 + i0*1800] = (uint64_t)(arg_1[i1 + i0*1800]);
    }
  }


  uint8_t* arg_2 = (uint8_t*)shmat(1998909, nullptr, 0);
  uint8_t arg_top_2[10 * 3];
  for (size_t i0 = 0; i0 < 10; i0++) {
    for (size_t i1 = 0; i1 < 3; i1++) {
      arg_top_2[i1 + i0*3] = (uint8_t)(arg_2[i1 + i0*3]);
    }
  }


  printf("Digit Recognition Application\n");


  // parse command line arguments for opencl version
  std::string kernelFile("");
  parse_sdaccel_command_line_args(argc, argv, kernelFile);




  // create OpenCL world
  CLWorld digit_rec_world = CLWorld(TARGET_DEVICE, CL_DEVICE_TYPE_ACCELERATOR);


  // add the bitstream file
  digit_rec_world.addProgram(kernelFile);


  // create kernels
  CLKernel DigitRec(digit_rec_world.getContext(), digit_rec_world.getProgram(), "DigitRec", digit_rec_world.getDevice());


  // create mem objects
  CLMemObj source_0((void*)fool_0, sizeof(uint64_t), 1, CL_MEM_READ_WRITE);
  CLMemObj source_1((void*)arg_top_1, sizeof(uint64_t), 10 * 1800, CL_MEM_READ_WRITE);
  CLMemObj source_2((void*)arg_top_2, sizeof(uint8_t), 10 * 3, CL_MEM_READ_WRITE);


  // add them to the world
  digit_rec_world.addMemObj(source_0);
  digit_rec_world.addMemObj(source_1);
  digit_rec_world.addMemObj(source_2);


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
  for (size_t i0 = 0; i0 < 10; i0++) {
    for (size_t i1 = 0; i1 < 1800; i1++) {
      arg_1[i1 + i0*1800] = (uint64_t)(arg_top_1[i1 + i0*1800]);
    }
  }
  shmdt(arg_1);
  for (size_t i0 = 0; i0 < 10; i0++) {
    for (size_t i1 = 0; i1 < 3; i1++) {
      arg_2[i1 + i0*3] = (uint8_t)(arg_top_2[i1 + i0*3]);
    }
  }
  shmdt(arg_2);


  }
