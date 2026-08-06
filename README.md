### Build via CMake
* Generate build files
cmake -B build -G Ninja
    * -B creates the build files inside the build dir (will create the dir, if it does not exist)
    * also -S . would specify location of CMakeLists.txt (source dir)
    * -G specifies the build tool, Ninja cause it gens the compile_commands.json
* Compilation of executable
cmake --build build --config Debug
    * calls your build tool (specific compiler) targeting dir build
    * --config will specify to build the Debug build
* Run via 
./build/AlgoVis.exe

* Remove build (on Win10)
Remove-Item -Recurse -Force build

* build all
cmake --build build --config Debug

### Komentář
Nutno dodat že projekt byl psán na Win10. Remove-Item není linux cmd, ale cmake by měl fungovat.

Následuje úvod, který má demonstrovat o co se vlastně snažím.

VisualVector je kolekce, která se používá při psaní algoritmu k vizualizaci. Postpuně vytváří instrukce pro zobrazení. Nijak nepoužívá SFML.

BarChart je jeden (momentálně jediný) způsob jakým lze VisualVector visualizovat s použitím knihovny SFML. Skládá se z pole třídy Bar a pracuje výhradně s instrukcemi generovanými VisualVector, k samotnému vektoru skrytému pod VisualVector by se neměla vůbec dostat.

Dále bych chtěl, krom rozšíření kolekcí a zobrazení, přidat způsob ovládání programu skrze nějaké jednoduché menu. SFML podporuje vložení jednoho okna do druhého. A přidat možnost zobrazování v realném čase s komunikací mezi dvěmi vlákny. Jedno které generuje instrukce a hned je posílá do druhého, které podle nich upravuje zobrazení.


