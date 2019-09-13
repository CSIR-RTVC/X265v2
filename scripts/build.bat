cd ..\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install x265
if not exist ..\build mkdir ..\build
cd ..\build
cmake ..
cmake --build . --config Release
cmake --build . --config Debug
