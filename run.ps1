New-Item ./build -ItemType Directory
Set-Location build
cmake .. -G "MinGW Makefiles"
make
./SmartCarMain.exe  