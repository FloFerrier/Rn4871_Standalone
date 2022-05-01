# framework-test-c Project
## Unity framework for testing c
- Version : v2.5.2
Install unity as a submodule
```bash
$ git submodule update --init
$ git submodule update --remote --merge
```
## lib
The target to test is locating on lib folder, you can put yours files directly here or
use a submodule for having a different repo.
Modify the lib/CMakeLists.txt for building your own sources.
## Use
VSCode
* Select your build variant : Test/Debug/Release
* Build your selected target
* Run the selected target on the terminal