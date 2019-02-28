::############################################################################
::# Install OpenVR
::############################################################################
IF NOT EXIST openvr\ (
	mkdir openvr
	cd openvr
	set URL="https://raw.githubusercontent.com/ValveSoftware/openvr/master/headers/openvr.h"
	appveyor DownloadFile %URL% -FileName openvr.h
	IF "%BUILD_TYPE%" == "64bit" (set WIN=win64) ELSE (set WIN=win32)
	set URL="https://raw.githubusercontent.com/ValveSoftware/openvr/master/lib/%WIN%/openvr_api.lib"
	appveyor DownloadFile %URL% -FileName openvr_api.lib
	set URL="https://raw.githubusercontent.com/ValveSoftware/openvr/master/bin/%WIN%/openvr_api.dll"
	appveyor DownloadFile %URL% -FileName openvr_api.dll
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
	set URL="https://warehouse.leapmotion.com/apps/4183/download"
	appveyor DownloadFile %URL% -FileName leap.zip
	7z x leap.zip > nul
	move LeapDeveloperKit* Leap
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
	set URL="https://sourceforge.net/projects/pythonqt/files/pythonqt/PythonQt-3.2/PythonQt3.2.zip/download"
	appveyor DownloadFile %URL% -FileName pythonqt.zip
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
	cd ..\..
)
set PYTHONQT_INCLUDE_DIRS=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/src
set PYTHONQT_LIBRARIES=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/lib/PythonQt-Qt5-Python373.lib
set PYTHONQT_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\pythonqt\PythonQt3.2\lib\PythonQt-Qt5-Python373.dll
set PYTHONQT_QTALL_INCLUDE_DIRS=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/extensions/PythonQt_QtAll
set PYTHONQT_QTALL_LIBRARIES=%APPVEYOR_BUILD_FOLDER%/deps/pythonqt/PythonQt3.2/lib/PythonQt_QtAll-Qt5-Python373.lib
set PYTHONQT_QTALL_SHARED=%APPVEYOR_BUILD_FOLDER%\deps\pythonqt\PythonQt3.2\lib\PythonQt_QtAll-Qt5-Python373.dll
