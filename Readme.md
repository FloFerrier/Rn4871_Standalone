# RN4871-DRIVER-DEV
Dev environment for testing RN4871 library.
A virtual module is available to allow testing without a hardware material.
## Startup
VSCode IDE with CMake extension.
Unity is the framework used for testing.
The first step is to fetch RN4871-driver from Github
```bash
$ git submodule update
```
## Organization
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
## Utilization
* Select your build variant : Test/Debug/Release
* Build your selected target
* Run the selected target on the terminal