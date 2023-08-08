# Smart Car Project

OpenGL based self-driving car (with DQN algorithm LibTorch neural network)

## Installation

**Make sure you have (Graphics part)**

1. *Git*
2. *Powershell*
3. *OpenGL*

**Make sure you have (Neural network part)**

1. *[CUDA](https://developer.nvidia.com/cuda-downloads) (~3.5 GB) [Tested version 12.2.1_536.67 on Windows 11]*
2. *[cuDNN](https://developer.nvidia.com/rdp/cudnn-download) (~700 MB) [Tested version 8.9.3.28 for CUDA 12 on Windows 11]*
3. *[NVIDIA Nsight Graphics](https://developer.nvidia.com/nsight-graphics) (~1 GB) [Tested version 2023.2.1.23178 on Windows 11]*
4. *[LibTorch for C++](https://pytorch.org/get-started/locally) (~3 GB) [Tested **DEBUG** version 2.0.1 for CUDA 11.8  on Windows 11]*

**and copied all files from**

    C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\<version>\extras\visual_studio_integration\MSBuildExtensions

**to**

    C:\Program Files (x86)\Microsoft Visual Studio\<version>\<product>\MSBuild\Microsoft\VC\<version>\BuildCustomizations

where *\<product\>* is the MSVC product you use: *{ BuildTools, Enterprise, Community, etc. }*

## Execution

    git clone https://github.com/BiscuitsLayer/SmartCarProject.git
    cd ./SmartCarProject
    git submodule update --init --recursive
    ./run.ps1

Sport car model: [link](https://sketchfab.com/3d-models/concept-sport-car-566075bdb499404b908895a5f4dc6aa0)

Road model: [link](https://sketchfab.com/3d-models/parking-garage-free-download-5310b7d77b70427d936ec4253fff679c)

Checked flag model: [link](https://sketchfab.com/3d-models/checkered-racing-flag-4301c9957d414343af361a7e1528a283)

# Useful sources:

[LibTorch RL implementation](https://github.com/navneet-nmk/Pytorch-RL-CPP)

[DQN Theory](https://jaromiru.com/2016/09/27/lets-make-a-dqn-theory/)

[DQN Basics](https://tomroth.com.au/dqn-basics/)

[DQN for Continuous Control Tasks](https://medium.com/analytics-vidhya/naf-normalized-advantage-function-dqn-for-continuous-control-tasks-b9dcb6ebeab8)