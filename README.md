# Smart Car Project

OpenGL based self-driving car (with DQN algorithm LibTorch neural network)

## Installation

**Make sure you have (Graphics part)**

1. *Git*
2. *Powershell*
3. *OpenGL*

**Make sure you have (Neural network part)**

1. *[CUDA](https://developer.nvidia.com/cuda-downloads) (~3.5 GB)*
2. *[cuDNN](https://developer.nvidia.com/rdp/cudnn-download) (~700 MB)*
3. *[NVIDIA Nsight Graphics](https://developer.nvidia.com/nsight-graphics) (~1 GB)*
4. *[LibTorch](https://pytorch.org/get-started/locally) (~2 GB)*

current version works fine with CUDA 12.1

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
