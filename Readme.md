# RN4871 STANDALONE
## Introduction
The main goal of this repository is to test and develop the RN4871-driver. The final target of this driver is embedded platform. However, developers can use this environment for testing RN4871 module API with the real hardware module or with the virtual module contained on the driver.
## Requirements
If you would like test on a real RN4871 module, you must have :
- RN4871 hardware module by Microship (with hardware integration of course).
- Connector UART/TTL
Else, you can use the virtual module.
- Linux desktop or laptop
- VSCode IDE with CMake extension (you can also use the command line mode)
## Startup
The first step is to fetch RN4871-driver from Github
```bash
git submodule update --init
```
## Build binary file
### With VSCode
* Select your build variant : Test/Debug/Release
* Build your selected target
* Run the selected target on the terminal
### With Command Line
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=<Release or Test or Debug>
cmake --build . --target all
```
To clean the CMake cache, run this following command :
```bash
cmake --build . --target clean
```
## Run Rn4871 Test suite
If you are selecting build type Test, CMake will build a binary with driver test suite. You can also run it :
```bash
./bin/Rn4871_Test
```
## Use binary file and Python script
Open two terminal, one to run C binary file (communication with RN4871 module) and another to start Python script (Host bluetooth client).
```bash
./bin/Rn4871_<Debug or Release>
```
Warning, we must to use bluetoothctl before to launch the Python script for enabling bluetooth connection between Host and the RN4871 module.
```bash
sudo bluetoothctl agent on
sudo bluetoothctl power on
sudo bluetoothctl connect <RN4871_MAC_ADDRESS>
./scripts/client_ble.py
```
> **Notes:** Stop script or executable with CTRL+C.