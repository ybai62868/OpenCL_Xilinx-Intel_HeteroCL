# Getting Started

This page provides basic tutorial about the usage of HeteroCL DigitRec sample.

For installation instructions, please see [INSTALL.md](INSTALL.md).

- [x] Generate Kernel file automatically.
- [x] Generate Host file automatically.
- [x] Generate Wrapper file automatically.
- [x] Runtime system.

---

### The whole pipeline

You can use the following HeteroCL code to start the whole pipeline.Here is an example of building the KNN-Based Digit Recognition. You can name it as digitrec_main.py

```python
import heterocl as hcl
import time
import numpy as np
import math
from digitrec_data import read_digitrec_data

# Declare some constants and data types. For images, we need unsigned 49-bit
# integers, while for knn matrices, we need unsigned 6-bit integers.
N = 7 * 7
max_bit = int(math.ceil(math.log(N, 2)))
data_size = (10, 1800)

# HeteroCL provides users with a set of bit-accurate data types, which include
# unsigned/signed arbitrary-bit integers and unsigned/signed fixed-points.
# Here we use `UInt(N)` for an N-bit unsigned integer.
dtype_image = hcl.UInt(N)
dtype_knnmat = hcl.UInt(max_bit)

# We can initialize a HeteroCL environment with default data type by using
# `hcl.init(dtype)`. Here we set the default data type of each variable to
# the unsigned integer with the maximum bitwidth.
hcl.init(dtype_image)

##############################################################################
# Top Function Offloaded to FPGA
# ==============================
# Following we show the code first. For each code block, you can find a
# corresponding explanation at the end of the top function.

def top(target=None):

    # Algorithm definition (§1)
    def knn(test_image, train_images):

        # Imperative programming and bit operations (§2)
        def popcount(num):
            out = hcl.local(0, "out")
            with hcl.for_(0, train_images.type.bits) as i:
                # Bit selection operation
                out[0] += num[i]
            return out[0]

        # This function update the candidates, i.e., `knn_mat`. Here we mutate
        # through the shape of tensor `dist`. For each `dist` value, if it is
        # smaller than the maximum candidate, we replace it.
        def update_knn(dist, knn_mat, i, j):
            max_id = hcl.local(0, "max_id")
            with hcl.for_(0, 3) as k:
                with hcl.if_(knn_mat[i][k] > knn_mat[i][max_id[0]]):
                    max_id[0] = k
            with hcl.if_(dist[i][j] < knn_mat[i][max_id[0]]):
                knn_mat[i][max_id[0]] = dist[i][j]

        # Main algorithm (§3)
        # Fist step: XOR (§3.1)
        diff = hcl.compute(train_images.shape,
                           lambda x, y: train_images[x][y] ^ test_image,
                           "diff")

        # Second step: popcount (§3.2)
        dist = hcl.compute(diff.shape,
                           lambda x, y: popcount(diff[x][y]),
                           "dist")


        # Third step: initialize the candidates (§3.3)
        knn_mat = hcl.compute((10, 3), lambda x, y: 50, "knn_mat")


        # Fourth step: update the candidates (§3.4)
        hcl.mutate(dist.shape,
                        lambda x, y: update_knn(dist, knn_mat, x, y),
                        "knn_update")

        # Final step: return the candidates (§3.5)
        return knn_mat

    # Inputs/Outputs definition (§4)
    # Scalars (§4.1)
    test_image = hcl.placeholder((), "test_image")
    # Tensors (§4.2)
    train_images = hcl.placeholder(data_size, "train_images")

    # Data type customization (§5.1)
    scheme = hcl.create_scheme([test_image, train_images], knn)
    scheme.downsize([knn.dist, knn.dist.out, knn.knn_mat], dtype_knnmat)

    # Compute customization (§5.2)
    s = hcl.create_schedule_from_scheme(scheme)

    diff = knn.diff
    dist = knn.dist
    knn_update = knn.knn_update

    # Merge loop nests
    s[diff].compute_at(s[dist], dist.axis[1])
    s[dist].compute_at(s[knn_update], knn_update.axis[1])

    # Reorder loop to expose more parallelism
    s[knn_update].reorder(knn_update.axis[1], knn_update.axis[0])

    # Parallel outer loop and pipeline inner loop
    # s[knn_update].parallel(knn_update.axis[1])
    s[knn_update].unroll(knn_update.axis[1])
    s[knn_update].pipeline(knn_update.axis[0])

    # At the end, we build the whole offloaded function.
    return hcl.build(s, target=target)

###############################################################################
# Voting algorithm
# ----------------
# This function implements the voting algorithm. We first sort for each digit.
# After that, we compare the values of the first place in each digit. The digit
# with the shortest value get one point. Similarly, we give the point to digits
# according to their ranking for the second place and third place. Finally, we
# take the digit with the highest point as our prediction label.
def knn_vote(knn_mat):
    knn_mat.sort(axis = 1)
    knn_score = np.zeros(10)

    for i in range(0, 3):
        min_id = np.argmin(knn_mat, axis = 0)[i]
        knn_score[min_id] += 1

    return np.argmax(knn_score)


###############################################################################
# Get the Results
# ---------------

# Data preparation
train_images, _, test_images, test_labels = read_digitrec_data()

# Classification and testing
correct = 0.0

# We have 180 test images
total_time = 0
for i in range(0, 180):

    # Prepare input data to offload function
    # To load the tensors into the offloaded function, we must first cast it to
    # the correct data type.
    hcl_train_images = hcl.asarray(train_images, dtype_image)
    hcl_knn_mat = hcl.asarray(np.zeros((10, 3)), dtype_knnmat)

    # Execute the offload function and collect the candidates
    start = time.time()
    offload(test_images[i], hcl_train_images, hcl_knn_mat)
    total_time = total_time + (time.time() - start)

    # Convert back to a numpy array
    knn_mat = hcl_knn_mat.asnumpy()

    # Feed the candidates to the voting algorithm and compare the labels
    if knn_vote(knn_mat) == test_labels[i]:
        correct += 1

print("Average kernel time (s): {:.2f}".format(total_time/180))
print("Accuracy (%): {:.2f}".format(100*correct/180))

# for testing
assert (correct >= 150.0)
```



Examples:

```shell
python digitrec/digitrec_main.py
```

You can get the final results on CPU simulation and the output is expected to be like the following.

```
Average kernel time (s): 0.00
Accuracy (%): 94.44
```

**Note:** You should put *digitrec_data.py* and *data* folder in this example and then you can get this structure.

```
├── digitrec
│   ├── digitrec_main.py
│   ├── digitrec_data.py
│   ├── digitrec_aws.py
│   ├── data
```

You can name another file named digitrec_aws.py and put it in the same directary (digitrec_main.py)

Here is the component of digitrec_aws.py

```python
from digitrec_main import *

f = top('aws_csim')

train_images, _, test_images, test_labels = read_digitrec_data()

correct = 0.0

total_time = 0
for i in range(0, 180):
    hcl_train_images = hcl.asarray(train_images, dtype_image)
    hcl_knn_mat = hcl.asarray(np.zeros((10, 3)), dtype_knnmat)

    start = time.time()
    f(test_images[i], hcl_train_images, hcl_knn_mat)
    total_time = total_time + (time.time() - start)

    knn_mat = hcl_knn_mat.asnumpy()
    if knn_vote(knn_mat) == test_labels[i]:
        correct += 1

print("Average kernel time (s): {:.2f}".format(total_time/180))
print("Accuracy (%): {:.2f}".format(100*correct/180))
```

Examples:

```shell
python digitrec/digitrec_aws.py
```

A jupyter notebook demo can be found in [demo]().

### Generate Kernel file

You can use different backend in HeteroCL to generate the kernel code you want. The operation is so easy, you only change the argument of function *top()*

```python
offload = top() # e.g., 'vhls', 'aws', 'aocl'
```

**Note:** HeteroCL can support these backend now:

`FPGA_TARGETS = ['merlinc', 'soda', 'soda_xhls', 'vhls', 'ihls', 'vhls_csim',                		  'sdaccel', 'sdaccel_csim', 'aocl', 'aocl_csim', 'aws', 'aws_csim']`

And then you should add this snippet after the `offload = top('aws')`:

```python
with open('knn_aws.cpp', 'w') as fin:
  fin.write(code)
```

###Generate Wrapper file 

In order to support some features that Xilinx OpenCL doesn't have, we add the wrapper file in the whole pipeline to make sure the Vivado HLS C++ can be embeded into host file sucessfully.

Here is an example of wrapper file:

```C++
#include <stdio.h>
#include "/home/centos/src/project_data/lab_digitrec_aws/solution/src/kernel/knn_aws.cpp"

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
  default_function(source_wrapper_0[0], source_wrapper_temp_1, source_wrapper_temp_2);
  for ( int i0 = 0; i0 < 10; i0++) {
    for ( int i1 = 0; i1 < 3; i1++) {
      source_wrapper_2[i1 + i0*3 ] = source_wrapper_temp_2[i0][i1];
    }
  }
}
}
```

Arguments information:

- `source_wrapper_0`: this argument represents the input argument (test_image label)
- `source_wrapper_1`: this argument represents another input argument (training_image data)
- `source_wrapper_2`: this argument represts the results (knn_mat)

**Note**: 

1. Including the Vivado HLS C++ code first.

2. More details about how to process or transform the data can be found in this file digitrec_data.py. And image & label are the basic knowledge in machine learning.

3. The int datatype in the function of DigitRec() should be the integer power of 2. e.g., 64, 8, 49 is not allowed. 

   

### Generate Host file

The host file of OpenCL maybe a bit complex. Don't worry! I will teach you how to write it and HeteroCL can help us generate it automatically.

In this example, some API in host file are based on Rosetta. If you don't know it, you can figure it out through this [repo](https://github.com/cornell-zhang/rosetta).

According to the different hardware vendors, I complete the following three runtime systems:

- Intel: based on OpenCL C API (AOCL)
- Xilinx: based on OpenCL C++ API (SDAccel)
- AWS: Rosetta

But the key concept for each host file are same, you will find that they follow the same pipline and structure. I will give some samples of various types of host file ( OpenCL C, OpenCL C++ and Rosetta).

Examples:

1. [sample1](../demo/host_cplusplus.cpp) based on OpenCL C API (AOCL).
2. [sample2](../demo/host_c.cpp) based on OpenCL C++ API (SDAccel).
3. [sample3](../demo/host_rosetta.cpp) based on  Rosetta.

I basically categorize the whole host file into these parts:

- Getting First Platform.
- Getting ACCELERATOR Devices and selecting it.
- Creating Context and Command Queue for selected Device.
- Creating Kernel and Functor of Kernel.
- Creating Buffers inside Device
- Copying input data to Device buffer from host memory
- Running Kernel
- Copying Device result data to Host memory
- Cleaning up the memory

Actually, if you can finish all of these parts in host file, and then combine with the kernel code. You can run any task successfully on heterogeneous platforms (CPUs + GPUs/FPGAs).

---

### Develop new components

I basically categorize the whole pipeline into 3 steps:

- Imperative Code: Python-based domain-specific language.
- Heterogeneous Backend: the component provides a fully automated compilation flow from DSL to heterogeneous platforms. e.g., CPUs and FPGAs.
- Runtime System: the component for specific tasks. e.g., CORDIC, KNN-Based Digit Recognition, GEMM, K-Means, LeNet Inference, Smith-Waterman Sequencing.

For more information on how it works, you can refer to [TECHNICAL_DETAILS.md](./docs/TECHNICAL_DETAILS.md).

---

### How does it work?

I don't want any mannual part occurs to the whole pipeline. You know, it will violate the principle that designing automation for high-performance and energy-efficient computer systems. **: )**

As mentioned above, the whole pipeline can be split into three important parts.

1. Imperative Code (Python).
2. Kernel Code Generation (Computing Platforms).
3. Host Code Generation (Runtime System).

The simlpest way is to use [shared memory](https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm) to **glue** data memory from a HeteroCL program to heterogeneous computing platforms.

For more details about how to use shared memory in this pipeline, you can refer to [TECHNICAL_DETAILS.md](./docs/TECHNICAL_DETAILS.md).