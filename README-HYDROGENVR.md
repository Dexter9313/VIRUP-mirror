[![Build Status](https://travis-ci.org/Dexter9313/HydrogenVR-mirror.svg?branch=master)](https://travis-ci.org/Dexter9313/HydrogenVR-mirror)
[![Build status](https://ci.appveyor.com/api/projects/status/i44acm08ah869xdg/branch/master?svg=true)](https://ci.appveyor.com/project/Dexter9313/hydrogenvr-mirror/branch/master)

# HydrogenVR

A C++ lightweight 3D rendering engine that tightly integrates VR at its core. It is based on OpenGL, Qt, OpenVR and the Leap Motion SDK.
This engine is used as a template right now, you have to write your code on top of it (but it's easy, follow the guide). This engine is not only a library but also provides CMake, Travis CI and Appveyor scripts to build and deploy your project, that's why it wouldn't be enough to only link to it.
Strong OpenGL knowledge is required as the GLHandler class is merely a convenient wrapper you will use to draw. Most of the higher level concepts wrapped are close to the OpenGL low-level concepts (for example, a Mesh is only a struct containing GLuint pointing to vertex or element buffers).

## To use the template

Make sure your new project is an empty repository without any commit...

Add the repo as the source remote (for example):

	git remote add source https://gitlab.com/Dexter9313/hydrogenvr.git

Then any time you want to update :

	git pull source master

Don't rebase anything from source if you want to be able to pull.


For initial setup, make sure you :

* Write your own README.md (see bellow for informations to put in it)
* Change variables in build.conf
* If you deploy on Github, add the API_KEY secure variable in the Travis CI project and replace the encrypted key in .appveyor.yml.
* Create a project directory. We will call it "projectdir" in this README but you can name it whatever you want as long as build.conf is set accordingly. All your source code should be within your project directory. Don't change anything at root level unless specified as safe here, unless you want engine update problems (see data/ and example/ directories for an example project).
* Create a MainWin class that inherits from AbstractMainWin to draw.
* Don't write a main() function, HydrogenVR already provides one.
* Don't remove the examples, they are light and won't be included within your project packages. This is to ensure you can update HydrogenVR without any problem.

You can also :
* Add CMake dependencies in projectdir/cmake/Dependencies.cmake
* Add files to install with CMake install() in projectdir/cmake/Install.cmake
* Add a custom install script for Travis CI as projectdir/ci/travis/install.sh
* Add a custom install script for Appveyor as projectdir/ci/appveyor/install.bat
* Add files to the final archive via a script for Appveyor as projectdir/ci/appveyor/before_archive.bat
* Add settings to the SettingsWidget constructor.
* Add assets to your data/projectdir directory. The working directory will always contain data/ (so you can always reach data relatively from this path; ex : "data/core/shaders/default.vert" is a valid path). The last example is what is called an "absolute data path" because you need to provide the "data/core" prefix to reach the data (if it is in data/core). The preferred way of accessing data is by using the utils.hpp:getAbsoluteDataPath() function and provide a "relative data path" to it (for last example it would have been "shaders/default.vert"). The function will figure in which data/ subdirectory is your data file and give back an "absolute data path" (for last example : "data/core/shaders/default.vert"). Your subdirectory has priority over core/, so you can override core/ data files by providing other versions in your data/projectdir. The data directory will automatically be packaged with your project releases (only the relevant subdirs).

The project name defined in build.conf as PROJECT_NAME is accessible in C++ code as the PROJECT_NAME macro, and so is PROJECT_DIRECTORY. They are C-style string constants.

All used classes starting with Q (ex: QSettings) belong to the Qt framework. The engine didn't wrap anything from Qt, so you will have to use Qt a least a little.

## Example releases

You can see releases for Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [HydrogenVR Releases](https://github.com/Dexter9313/HydrogenVR-mirror/releases).


# Example of things that should be in your README

Put build status on top.

## Installation and requirements

### Microsoft Windows

Simply run the HydrogenVR-VERSION-windows-ARCH_setup.exe setup wizard, where VERSION is the desired version and ARCH your system architecture.

If you want to run HydrogenVR from a portable zip archive instead, you will need to manually install this (if you don't have it on your system already) :
* [Microsoft Visual C++ 2015](https://www.microsoft.com/en-US/download/details.aspx?id=48145) (zip installation only; it is shipped with the setup wizard)

### GNU/Linux binaries

You will need the following requirements :
* OpenGL
* Qt5 Core and Gui (libqt5core5a and libqt5gui5 packages on Ubuntu)

You can then install the DEB package you want from the Releases page or use the portable zip version.

### GNU/Linux building from source

You will need the following requirements :
* A C++ compiler (g++ for example)
* CMake
* OpenGL dev
* Qt Core and Gui (qtbase5-dev packages on Ubuntu)
* [OpenVR](https://github.com/ValveSoftware/openvr)
* (Optional) [Leap Motion SDK 2.3.1](https://developer.leapmotion.com/sdk/v2)


Then clone this repository. We now suppose the root directory of the repository is stored in the $HydrogenVR_ROOT_DIR variable.

        cd $HydrogenVR_ROOT_DIR
        ./build-linux.sh
        cd build
        sudo make install

Optionally, you can generate a deb package to make installation managing easier if you are on a debian-based system. The package name will be "hydrogenvr".

        cd $HydrogenVR_ROOT_DIR
        ./build-linux.sh package
        sudo dpkg -i ./build/*.deb

## Usage

## Uninstall

If the deb method for installation was used :

        sudo apt-get autoremove hydrogenvr

If the make install method for installation was used, uninstallation can only be done if the build directory has been left untouched since installation (at least the install_manifest.txt file within it) :

        cd $HydrogenVR_ROOT_DIR
        cd build
        sudo make uninstall

As of now, this method can leave some empty directories in your file system though... Check install_manifest.txt content yourself if you want to clean everything properly.
