[![Build Status](https://travis-ci.org/Dexter9313/HydrogenVR-mirror.svg?branch=master)](https://travis-ci.org/Dexter9313/HydrogenVR-mirror)
[![Build status](https://ci.appveyor.com/api/projects/status/i44acm08ah869xdg/branch/master?svg=true)](https://ci.appveyor.com/project/Dexter9313/hydrogenvr-mirror/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/b6520a9f88a14fa3b8aa9c23ac4411c3)](https://www.codacy.com/app/Dexter9313/HydrogenVR-mirror?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Dexter9313/HydrogenVR-mirror&amp;utm_campaign=Badge_Grade)

# HydrogenVR

A C++ lightweight 3D rendering engine that tightly integrates VR at its core. It is based on OpenGL, Qt, OpenVR and the Leap Motion SDK.
This engine is used as a template right now, you have to write your code on top of it (but it's easy, follow the guide). This engine is not only a library but also provides CMake, Travis CI and Appveyor scripts to build and deploy your project, that's why it wouldn't be enough to only link to it.
Strong OpenGL knowledge is required as the GLHandler class is merely a convenient wrapper you will use to draw. Most of the higher level concepts wrapped are close to the OpenGL low-level concepts (for example, a Mesh is only a struct containing GLuint pointing to vertex or element buffers).

## Documentation

Documentation can be read here : [HydrogenVR Documentation](https://dexter9313.gitlab.io/hydrogenvr/).

## To use the template

Make sure your new project is not empty if you don't want a dirty git history right off the bat (make an initial commit with a README.md file for example).

Add the repo as the "hydrogenvr" remote:

	git remote add hydrogenvr https://gitlab.com/Dexter9313/hydrogenvr.git

Then squash HydrogenVR's history into one commit. For the first time you should do this manually using the following code :

Linux :

	git fetch --all
	HASH=$(git ls-remote hydrogenvr -h refs/heads/master | cut -f1)
	git pull --squash -X theirs hydrogenvr master --allow-unrelated-histories
	git commit -m "Update HydrogenVR to $HASH" -e

If you only want to get HydrogenVR up to one specific commit hash, the procedure is a bit more tedious :

Linux :

	git fetch --all
	HASH="abcdefghij..." # replace with desired hash !
	CURRENT_BRANCH=$(git branch | grep \* | cut -d ' ' -f2)
	git checkout --orphan update_hydrogenvr
	git reset --hard
	git pull hydrogenvr master
	git reset --hard $HASH
	git checkout $CURRENT_BRANCH
	git merge --squash -X theirs update_hydrogenvr --allow-unrelated-histories
	git branch -D update_hydrogenvr
	git commit -m "Update HydrogenVR to $HASH" -e

Then anytime you want to update HydrogenVR, run the update-hydrogenvr.sh script with an optional specific commit hash as argument. This script basically does the same as the commands above.


For initial setup, make sure you :

*  Write your own README.md (see bellow for informations to put in it)
*  Copy build.conf.example as build.conf, then change variables values in build.conf accordingly.
*  If you deploy on Github, add the API_KEY secure variable in the Travis CI project and replace the encrypted key in .appveyor.yml.
*  Create a project directory. We will call it "projectdir" in this README but you can name it whatever you want as long as build.conf is set accordingly. All your source code should be within your project directory. Don't change anything at root level unless specified as safe here, unless you want engine update problems (see data/ and example/ directories for an example project).
*  Create a MainWin class in projectdir/include/MainWin.hpp that inherits from AbstractMainWin to draw. It can do nothing if you don't want to do anything (you only have to implement the pure virtual methods, but you can make them do nothing).
*  Create a Launcher class in projectdir/include/Launcher.hpp that inherits from BaseLauncher to instanciate a Launcher. It can do nothing if you don't want to do anything.
*  Don't write a main() function, HydrogenVR already provides one.
*  Don't remove the examples, they are light and won't be included within your project packages. This is to ensure you can update HydrogenVR without any problem.

You can also :
*  Add CMake dependencies in projectdir/cmake/Dependencies.cmake
*  Add files to install with CMake install() in projectdir/cmake/Install.cmake
*  Add a custom install script for Travis CI as projectdir/ci/travis/install.sh
*  Add a custom install script for Appveyor as projectdir/ci/appveyor/install.bat
*  Add files to the final archive via a script for Appveyor as projectdir/ci/appveyor/before_archive.bat
*  Add settings to the SettingsWidget constructor.
*  Add assets to your data/projectdir directory. The working directory will always contain data/ (so you can always reach data relatively from this path; ex : "data/core/shaders/default.vert" is a valid path). The last example is what is called an "absolute data path" because you need to provide the "data/core" prefix to reach the data (if it is in data/core). The preferred way of accessing data is by using the utils.hpp:getAbsoluteDataPath() function and provide a "relative data path" to it (for last example it would have been "shaders/default.vert"). The function will figure in which data/ subdirectory is your data file and give back an "absolute data path" (for last example : "data/core/shaders/default.vert"). Your subdirectory has priority over core/, so you can override core/ data files by providing other versions in your data/projectdir. The data directory will automatically be packaged with your project releases (only the relevant subdirs).

The project name defined in build.conf as PROJECT_NAME is accessible in C++ code as the PROJECT_NAME macro, and so is PROJECT_DIRECTORY. They are C-style string constants.

All used classes starting with Q (ex: QSettings) belong to the Qt framework. The engine didn't wrap anything from Qt, so you will have to use Qt a least a little.

## Libraries

You can add HydrogenVR libraries to your project. Add them in projectdir/thirdparty. For example, you will have the following : projectdir/thirdparty/mylib/include and projecteddir/thirdparty/mylib/src. How you add them is up to you (submodules, subtrees, hard copy). For now, include files are not prefixed, be careful of file name collisions ! A good practice would be to add subfolders in your include directories to avoid that. The content of libraries directories is structured the same way as projects and will soon be able to have the same type of content (install scripts, cmake scripts, etc...). For now only the include and src directories are used.

## Example releases

You can see releases for Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [HydrogenVR Releases](https://github.com/Dexter9313/HydrogenVR-mirror/releases).

## Example of things that should be in your README

Put build status on top.

### Installation and requirements

#### Microsoft Windows

Simply run the HydrogenVR-VERSION-windows-ARCH_setup.exe setup wizard, where VERSION is the desired version and ARCH your system architecture.

If you want to run HydrogenVR from a portable zip archive instead, you will need to manually install this (if you don't have it on your system already) :
*  [Microsoft Visual C++ 2015](https://www.microsoft.com/en-US/download/details.aspx?id=48145) (zip installation only; it is shipped with the setup wizard)

#### GNU/Linux binaries

You will need the following requirements :
*  OpenGL
*  Qt5 Core and Gui (libqt5core5a and libqt5gui5 packages on Ubuntu)

You can then install the DEB package you want from the Releases page or use the portable zip version.

#### GNU/Linux building from source

You will need the following requirements :
*  A C++ compiler (g++ for example)
*  CMake
*  OpenGL dev
*  Qt Core and Gui (qtbase5-dev packages on Ubuntu)
*  [OpenVR](https://github.com/ValveSoftware/openvr)
*  (Optional) [Leap Motion SDK 2.3.1](https://developer.leapmotion.com/sdk/v2)

Then clone this repository. We now suppose the root directory of the repository is stored in the $HydrogenVR_ROOT_DIR variable.

        cd $HydrogenVR_ROOT_DIR
        ./build-linux.sh
        cd build
        sudo make install

Optionally, you can generate a deb package to make installation managing easier if you are on a debian-based system. The package name will be "hydrogenvr".

        cd $HydrogenVR_ROOT_DIR
        ./build-linux.sh package
        sudo dpkg -i ./build/*.deb

### Usage

### Uninstall

If the deb method for installation was used :

        sudo apt-get autoremove hydrogenvr

If the make install method for installation was used, uninstallation can only be done if the build directory has been left untouched since installation (at least the install_manifest.txt file within it) :

        cd $HydrogenVR_ROOT_DIR
        cd build
        sudo make uninstall

As of now, this method can leave some empty directories in your file system though... Check install_manifest.txt content yourself if you want to clean everything properly.
