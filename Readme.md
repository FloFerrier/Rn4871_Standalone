# RN4871-DRIVER-DEV
Dev environment for testing RN4871 library.
A virtual module is available to allow testing without a hardware material.
## Startup
VSCode IDE with CMake extension (or in command line).
Unity is the framework used for testing.
The first step is to fetch RN4871-driver from Github
```bash
$ git submodule update --init
```
## Organization
```bash
.
├── bin
├── build
├── CMakeLists.txt
├── cmake-variants.yaml
├── lib
│   ├── CMakeLists.txt
│   └── rn4871-driver
│       ├── gatt.h
│       ├── rn4871.c
│       ├── rn4871_defs.h
│       ├── rn4871.h
│       ├── virtual_module.c
│       └── virtual_module.h
├── Readme.md
├── src
│   ├── CMakeLists.txt
│   └── main.c
└── test
    ├── CMakeLists.txt
    └── test_main.c
```
## Build and Launch binary file
### With VSCode
* Select your build variant : Test/Debug/Release
* Build your selected target
* Run the selected target on the terminal
### With Command Line
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=<Release Test Debug>
make
./../bin/<release debug test>
```
## Roadmap
- Transparent UART
- GATT

## Utilization
A RN4871 module can be plug on the desktop with a converter UART/tty.
Or you can choose to use the virtual module.
For using the hardware module :
```bash
$ sudo apt install picocom
$ sudo picocom -b 115200 /dev/ttyUSB0
```

## Generate HTML page for test coverage
Build on Test mode and run the following command on the project root (<PROJECT_ROOT>=~/Projects/rn4871-driver-dev):
```bash
mkdir test-coverage && cd test-coverage
geninfo <PROJECT_ROOT>/build/lib/rn4871-driver/CMakeFiles/librn4871-driver.dir -b <PROJECT_ROOT>/build/lib/rn4871-driver -o ./coverage.info
genhtml coverage.info -o generate-html
```