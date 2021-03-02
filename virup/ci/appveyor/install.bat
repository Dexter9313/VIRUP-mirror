::############################################################################
::# Install liboctree
::############################################################################
mkdir octree
cd octree
set URL="https://github.com/Dexter9313/octree-file-format-mirror/releases/download/1.13.1/liboctree-1.13.1-windows-%BUILD_TYPE%.zip"
appveyor DownloadFile %URL% -FileName octree.zip
7z x octree.zip > nul
move liboctree* liboctree
set OCTREE_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/octree/
set OCTREE_LIBRARY=%APPVEYOR_BUILD_FOLDER%/deps/octree/liboctree/octree.lib
set OCTREE_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\octree\liboctree\octree.dll
cd ..

set BOOST_ROOT=C:/Libraries/boost_1_65_1
