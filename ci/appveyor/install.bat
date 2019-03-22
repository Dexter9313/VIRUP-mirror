::############################################################################
::# Install OpenVR
::############################################################################
IF NOT EXIST openvr\ (
	mkdir openvr
	cd openvr
	appveyor DownloadFile https://raw.githubusercontent.com/ValveSoftware/openvr/master/headers/openvr.h -FileName openvr.h
	IF "%BUILD_TYPE%" == "64bit" (
		appveyor DownloadFile https://raw.githubusercontent.com/ValveSoftware/openvr/master/lib/win64/openvr_api.lib -FileName openvr_api.lib
		appveyor DownloadFile https://raw.githubusercontent.com/ValveSoftware/openvr/master/bin/win64/openvr_api.dll -FileName openvr_api.dll
	) ELSE (
		appveyor DownloadFile https://raw.githubusercontent.com/ValveSoftware/openvr/master/lib/win32/openvr_api.lib -FileName openvr_api.lib
		appveyor DownloadFile https://raw.githubusercontent.com/ValveSoftware/openvr/master/bin/win32/openvr_api.dll -FileName openvr_api.dll
	)
	echo "OpenVR installed..."
	cd ..
)
set OPENVR_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/openvr/
set OPENVR_LIBRARY=%APPVEYOR_BUILD_FOLDER%/deps/openvr/openvr_api.lib
set OPENVR_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\openvr\openvr_api.dll

::############################################################################
::# Install Leap Motion SDK
::############################################################################
IF NOT EXIST leap\ (
	mkdir leap
	cd leap
	appveyor DownloadFile https://warehouse.leapmotion.com/apps/4183/download -FileName leap.zip
	7z x leap.zip > nul
	move LeapDeveloperKit* Leap
	echo "LeapMotionSDK installed..."
	cd ..
)
IF "%BUILD_TYPE%" == "64bit" (set ARCH=x64) ELSE (set ARCH=x86)
set LEAPMOTION_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/leap/Leap/LeapSDK/include
set LEAPMOTION_LIBRARY=%APPVEYOR_BUILD_FOLDER%/deps/leap/Leap/LeapSDK/lib/%ARCH%/Leap.lib
set LEAPMOTION_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\leap\Leap\LeapSDK\lib\%ARCH%\Leap.dll

::############################################################################
::# Install PythonQt3.2
::############################################################################
if NOT EXIST pythonqt\ (
	mkdir pythonqt
	cd pythonqt
	appveyor DownloadFile https://sourceforge.net/projects/pythonqt/files/pythonqt/PythonQt-3.2/PythonQt3.2.zip/download -FileName pythonqt.zip
	7z x pythonqt.zip > nul
	cd PythonQt3.2
	del PythonQt.pro
	del build\python.prf
	del build\PythonQt.prf
	del build\PythonQt_QtAll.prf
	move %APPVEYOR_BUILD_FOLDER%\misc\PythonQt.pro .
	move %APPVEYOR_BUILD_FOLDER%\misc\python.prf .\build\python.prf
	move %APPVEYOR_BUILD_FOLDER%\misc\PythonQt.prf .\build\PythonQt.prf
	move %APPVEYOR_BUILD_FOLDER%\misc\PythonQt_QtAll.prf .\build\PythonQt_QtAll.prf
	IF "%BUILD_TYPE%" == "64bit" (set ARCH=amd64) ELSE (set ARCH=x86)
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %ARCH%
	qmake
	nmake
	echo "PythonQt installed..."
	cd ..\..
)
set PYTHONQT_INCLUDE_DIRS=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/src
set PYTHONQT_LIBRARIES=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/lib/PythonQt-Qt5-Python373.lib
set PYTHONQT_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\pythonqt\PythonQt3.2\lib\PythonQt-Qt5-Python373.dll
set PYTHONQT_QTALL_INCLUDE_DIRS=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/extensions/PythonQt_QtAll
set PYTHONQT_QTALL_LIBRARIES=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/lib/PythonQt_QtAll-Qt5-Python373.lib
set PYTHONQT_QTALL_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\pythonqt\PythonQt3.2\lib\PythonQt_QtAll-Qt5-Python373.dll

::############################################################################
::# Install ASSIMP 4.1
::############################################################################
if NOT EXIST assimp\ (
	git clone https://github.com/assimp/assimp.git --branch=v4.1.0
	cd assimp
	mkdir build
	cd build
	cmake .. -G %GENERATOR%
	cmake --build . --config Release
	cd ..\..
)

set ASSIMP_INCLUDE_DIRS=%APPVEYOR_BUILD_FOLDER%/deps/assimp/include;%APPVEYOR_BUILD_FOLDER%/deps/assimp/build/include
set ASSIMP_LIBRARIES=%APPVEYOR_BUILD_FOLDER%/deps/assimp/build/code/Release/assimp-vc140-mt.lib
set ASSIMP_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\assimp\build\code\Release\assimp-vc140-mt.dll
