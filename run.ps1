# Build OOGL submodule .a (library) file
Set-Location include/OOGL
make
Set-Location ../..

# Build smartcar
New-Item ./build -ItemType Directory
Set-Location build
cmake .. -G "MinGW Makefiles"
make

# Run smartcar
./SmartCarMain.exe
Set-Location ..