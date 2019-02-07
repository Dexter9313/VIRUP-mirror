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

This new branch will be used to pull updates from this repo :

	git checkout -b update-source

The any time you want to update :

	git pull source master
	git branch -u source/master #only first time
	
Then from any other branch in the project (master for example with git checkout master) :

	git merge update-source
	
Don't rebase anything in update-source if you want to be able to pull.


This branch will now have an updated version of this template. Subsequent eninge updates will only need the pull/merge from update-source steps.


For initial setup, make sure you :

* Erase and replace this README by a new one.
* Change .travis.yml, .appveyor.yml and CMakeLists.txt PROJECT_NAME variables.
* If you deploy on Github, add the API_KEY secure variable in the Travis CI project and replace the encrypted key in .appveyor.yml.
* Change the PROJECT_NAME in CMakeLists.txt.
* Change the CPack parameters in CMakeLists.txt (descriptions, dependencies, etc...).
* Change innosetup/config.iss defines.
* Change build status from Travis and Appveyor to your own.
* Inherit from AbstractMainWin to draw (replace all of the MainWin class code which serves as an example).

You can also :
* Add settings to the SettingsWidget constructor.
* Add assets to the data/ directory. The working directory will always contain data (so you can always reach data relatively from this path : "data/"). It will be packaged with the rest of the project.

The project name defined as CMakeLists.txt PROJECT_NAME will be accessible in C++ code as the PROJECT_NAME macro. It is a C-style string constant.

All used classes starting with Q (ex: QSettings) belongs to the Qt framework. The engine didn't wrap anything from Qt, so you will have to use Qt a least a little.

## Example releases

You can see releases for Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [HydrogenVR Releases](https://github.com/Dexter9313/HydrogenVR-mirror/releases).


# Example of things that should be in your README

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
        mkdir build && cd build
        cmake ..
        make -j
        sudo make install

Optionally, you can generate a deb package to make installation managing easier if you are on a debian-based system. The package name will be "hydrogenvr".

        cd $HydrogenVR_ROOT_DIR
        mkdir build && cd build
        cmake ..
        make -j package
        sudo dpkg -i ./*.deb

## Usage

## Uninstall

If the deb method for installation was used :

        sudo apt-get autoremove hydrogenvr

If the make install method for installation was used, uninstallation can only be done if the build directory has been left untouched since installation (at least the install_manifest.txt file within it) :

        cd $HydrogenVR_ROOT_DIR
        cd build
        sudo make uninstall

As of now, this method can leave some empty directories in your file system though... Check the file content yourself if you want to clean everything properly.
