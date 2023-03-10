# Smart Car Project

OpenGL based self-driving car (with DQN algorithm LibTorch neural network)

## Installation

**Make sure you have:**

1. *Git*
2. *Powershell*
3. *GNU Make for Windows*
4. *OpenGL*
5. *CUDA + cuDNN + NVIDIA Nsight Graphics*
6. *LibTorch*

**and copied all files from**

    C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\<version>\extras\visual_studio_integration\MSBuildExtensions

**to**

    C:\Program Files (x86)\Microsoft Visual Studio\<version>\<product>\Microsoft\VC\<version>\BuildCustomizations

where *\<product\>* is the MSVC product you use: *{ BuildTools, Enterprise, Community, etc. }*

## Execution

    git clone https://github.com/BiscuitsLayer/SmartCarProject.git
    cd ./SmartCarProject
    git submodule update --init --recursive
    ./run.ps1

Sport car model: [link](https://sketchfab.com/3d-models/concept-sport-car-566075bdb499404b908895a5f4dc6aa0)

Road model: [link](https://sketchfab.com/3d-models/parking-garage-free-download-5310b7d77b70427d936ec4253fff679c)
