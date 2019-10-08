/*
    Yang.Bai
    yb269@cornell.edu
*/

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <math.h>

#define M 10
#define N 10
#define k 10



int main(int argc, char* argv[])
{
#if defined(SDX_PLATFORM) && !defined(TARGET_DEVICE)
  #define STR_VALUE(arg) #arg
  #define GET_STRING(name) STR_VALUE(name)
  #define TARGET_DEVICE GET_STRING(SDX_PLATFORM)
#endif
    
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];

    // size_t vector_size_bytes = sizeof(int) * LENGTH * LENGTH;
    size_t vector_size_bytes1 = sizeof(int) * M * K;
    size_t vector_size_bytes2 = sizeof(int) * K * N;
    size_t vector_size_bytes3 = sizeof(int) * M * N;



    //Source Memories
    std::vector<int> mat_a(M * K);
    std::vector<int> mat_b(K * N);
    std::vector<int> result_sim(M * N);
    std::vector<int> result_krnl(M * N);

    for (int i = 0;i < M;i++) {
        for (int j = 0;j < K;j++) {
            mat_a[j + i * M] = i;
        }
    }
    for (int i = 0;i < K;i++) {
        for (int j = 0;j < N;j++) {
            mat_b[j + i * K] = i * 2;
        }
    }

    for (int i = 0;i < M;i++) {
        for (int j = 0;j < N;j++) {
            int temp = 0;
            for ( int k = 0;k < K;k++) {
                temp += mat_a[k + i * M] * mat_b[j + k*K];
            }
            result_sim[j + i * M] = temp;
        }
    }


// OPENCL HOST CODE AREA START

    //Getting First Platform
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];
    std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    //Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
    cl::Device device = devices[0];
    std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

    //Creating Context and Command Queue for selected Device
    cl::Context context(device);
    cl::CommandQueue q(context, device);

    //Loading XCL Bin into char buffer
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg (0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg (0, bin_file.beg);
    char *buf = new char [nb];
    bin_file.read(buf, nb);

    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    devices.resize(1);
    cl::Program program(context, devices, bins);

    //Creating Kernel and Functor of Kernel
    int err1;
    cl::Kernel kernel(program, "default_function", &err1);
    // cl::Kernel kernel(program, "krnl_gemm", &err1);
    // std::cout << err1 << std::endl;
    if (err1 != CL_SUCCESS)
    {
         std::cout << "Error: Failed to create compute kernel!" << std::endl;
         std::cout << "Test failed" << std::endl;
         return EXIT_FAILURE;
    }
    auto default_function = cl::KernelFunctor<cl::Buffer&, cl::Buffer&, cl::Buffer&, cl::Buffer&>(kernel);


    //Creating Buffers inside Device
    cl::Buffer buffer_a(context, CL_MEM_READ_ONLY,  vector_size_bytes1);
    cl::Buffer buffer_b(context, CL_MEM_READ_ONLY,  vector_size_bytes2);
    cl::Buffer buffer_c(context, CL_MEM_WRITE_ONLY, vector_size_bytes3);


    //Copying input data to Device buffer from host memory
    q.enqueueWriteBuffer(buffer_a, CL_TRUE, 0, vector_size_bytes1, mat_a.data());
    q.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, vector_size_bytes2, mat_b.data());


    //Running Kernel
    default_function (cl::EnqueueArgs(q, cl::NDRange(1,1,1), cl::NDRange(1,1,1)),
                    buffer_a, buffer_b, buffer_c);

    q.finish();

    //Copying Device result data to Host memory
    q.enqueueReadBuffer(buffer_c, CL_TRUE, 0, vector_size_bytes3, result_krnl.data());
    int krnl_match = 0;
    for (int i = 0;i < M;i++) {
        for (int j = 0;j < N;j++) {
            if (result_sim[j + i * M] != result_krnl[j + i * M]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " CPU result = " << result_sim[j + i * M] << " Krnl result = " << result_krnl[j + i * M] << std::endl;
                krnl_match = 1;
                break;
            } else {
                std::cout << "Result match: i = " << i << " CPU result = " << result_sim[j + i * M] << " Krnl result = " << result_krnl[j + i * M] << std::endl;
            }
        }

    }




    return 0;
}
