[![Build Status](https://travis-ci.org/Dexter9313/VIRUP-mirror.svg?branch=master)](https://travis-ci.org/Dexter9313/VIRUP-mirror)
[![Build status](https://ci.appveyor.com/api/projects/status/st325kf8l81eq9kd/branch/master?svg=true)](https://ci.appveyor.com/project/Dexter9313/virup-mirror/branch/master)

# VIRUP

The VIrtual Reality Universe Project uses astrophysical data to propose a virtual reality visualization of it.

## Releases

You can download releases for GNU/Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [VIRUP Releases](https://github.com/Dexter9313/VIRUP-mirror/releases).

## Installation and requirements

### Microsoft Windows

Simply run the VIRUP-VERSION-windows-ARCH_setup.exe setup wizard, where VERSION is the desired version and ARCH your system architecture.

If you want to run VIRUP from a portable zip archive instead, you will need to manually install this (if you don't have it on your system already) :
* [Microsoft Visual C++ 2015](https://www.microsoft.com/en-US/download/details.aspx?id=48145) (zip installation only; it is shipped with the setup wizard)

### GNU/Linux binaries

You will need the following requirements :
* [liboctree](https://gitlab.com/Dexter9313/octree-file-format/blob/master/liboctree/)
* OpenGL
* Qt5 Core and Gui (libqt5core5a and libqt5gui5 packages on Ubuntu)

You can then install the DEB package you want from the Releases page or use the portable zip version.

### GNU/Linux building from source

You will need the following requirements :
* A C++ compiler (g++ for example)
* CMake
* [liboctree](https://gitlab.com/Dexter9313/octree-file-format/blob/master/liboctree/)
* OpenGL dev
* Qt Core and Gui (qtbase5-dev packages on Ubuntu)
* [OpenVR](https://github.com/ValveSoftware/openvr)
* (Optional) [Leap Motion SDK 2.3.1](https://developer.leapmotion.com/sdk/v2)


Then clone this repository. We now suppose the root directory of the repository is stored in the $VIRUP_ROOT_DIR variable.

        cd $VIRUP_ROOT_DIR
        mkdir build && cd build
        cmake ..
        make -j
        sudo make install

Optionally, you can generate a deb package to make installation managing easier if you are on a debian-based system. The package name will be "virup".

        cd $VIRUP_ROOT_DIR
        mkdir build && cd build
        cmake ..
        make -j package
        sudo dpkg -i ./*.deb

## Usage

$ virup-prototype [method] [number of points] [seed for random generation]

method:

	--base : draws every point as one pixel

	--basetex : draws every point as a dust texture

	--treelod : draws most visible points only at every frame as a subset of pixels

	--treetex : same as --treelod but points are rendered as dust textures

If number of points isn't supplied, will load octree files supplied in params.txt .

Use your keyboard's left and right arrow to rotate the cube. Up and down arrows to move towards or outwards the center. PageUp and PageDown to increase or decrease the transparency of points.
Escape key to quit.


## Uninstall

If the deb method for installation was used :

        sudo apt-get autoremove virup

If the make install method for installation was used, uninstallation can only be done if the build directory has been left untouched since installation (at least the install_manifest.txt file within it) :

        cd $VIRUP_ROOT_DIR
        cd build
        sudo make uninstall

As of now, this method can leave some empty directories in your file system though... Check the file content yourself if you want to clean everything properly.
