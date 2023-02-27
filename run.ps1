# Pull changes from remote repository
git pull

# Build OOGL submodule .a (library) file
Set-Location include/OOGL
make
Set-Location ../..

# Build smartcar
New-Item ./build -ItemType Directory -ErrorAction Ignore
Set-Location build
cmake .. -G "MinGW Makefiles"
make

# Run smartcar
./SmartCarMain.exe
Set-Location ..