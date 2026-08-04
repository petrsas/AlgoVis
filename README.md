### Build via CMake
* generate build files
cmake -B build -G Ninja
    * -B creates the build files inside the build dir (will create the dir, if it does not exist)
    * also -S . would specify location of CMakeLists.txt (source dir)
    * -G specifies the build tool, Ninja cause it gens the compile_commands.json
* compilation of executable
cmake --build build --config Debug
    * calls your build tool (specific compiler) targeting dir build
    * --config will specify to build the Debug build
* run via 
./build/Debug/AlgoVis.exe

* remove build
Remove-Item -Recurse -Force build

* build only AlgoVis
cmake --build build --target AlgoVis

* build only InstrGen
cmake --build build --target InstrGen

* build all
cmake --build build --config Debug
