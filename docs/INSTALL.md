## Installation

### Requirements

- Linux 
- Python 3.0 + 
- GCC ( G++ ) 4.8.5 or higher
- [Miniconda Linux 64-bit](https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh)

We have tested the following versions of OS and softwares:

- OS:  CentOS Linux release 7.5.1804 (Core)
- GCC ( G++ ): 4.8.5
- AWS EC2 Instance: 1.5.0

### Install Samples(DigitRec) on AWS

a. Install HeteroCL from my repo using the aws branch.

```shell
git clone -b aws https://github.com/ybai62868/heterocl
```

b. Clone the aws-fpga tool repository.

```shell
git clone https://github.com/aws/aws-fpga.git 
```

c. Install SDAccel development environment 

```shell
source envir.sh
```

**Note:** You should put the envir.sh outside the aws-fpga folder.

d. Clone the DigitRec Lab repository.

```shell
git clone https://github.com/ybai62868/OpenCL_Xilinx-Intel_HeteroCL/tree/master/aws/lab_digitrec_aws
```

**Note:** If your folder structure is different, you may need to change the correpsonding paths in some files. But I recommend you use this structure.

```
lab_digitrec_aws
├── harness
├── solution
│   ├── digitrec
│   │   ├── digitrec_main.py
│   │   ├── digitrec_data.py
│   │   ├── digitrec_aws.py
│   |		├── data
│   │   |		├── testing_set.dat
│   │   |		├── training_set_0.dat
│   │   |		├── training_set_1.dat
│   │   |		├── training_set_2.dat
│   │   |		├── training_set_3.dat
│   │   |		├── training_set_4.dat
│   │   |		├── training_set_5.dat
│   │   |		├── training_set_6.dat
│   │   |		├── training_set_7.dat
│   │   |		├── training_set_8.dat
│   │   |		├── training_set_9.dat
│   │   ├── digitrec_main.py
│   │   ├── digitrec_data.py
│   │   ├── digitrec_aws.py
│   ├── src
│   │   ├── host 
│   │   |		├── utils.cpp
│   │   |		├── utils.h
│   |		├── kernel
│   ├── Makefile
│   ├── run_hw.sh
│   ├── run_sw.sh
│   ├── README.md
```





