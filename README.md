[![Build Status](https://travis-ci.org/Dexter9313/HydrogenVR-mirror.svg?branch=master)](https://travis-ci.org/Dexter9313/HydrogenVR-mirror)
[![Build status](https://ci.appveyor.com/api/projects/status/i44acm08ah869xdg/branch/master?svg=true)](https://ci.appveyor.com/project/Dexter9313/hydrogenvr-mirror/branch/master)

# HydrogenVR

A C++ lightweight 3D rendering engine that tightly integrates VR at its core. It is based on OpenGL, Qt, OpenVR and the Leap Motion SDK.
This engine is used as a template right now, you have to write your code on top of it (but it's easy, follow the guide). This engine is not only a library but also provides CMake, Travis CI and Appveyor scripts to build and deploy your project, that's why it wouldn't be enough to only link to it.
Strong OpenGL knowledge is required as the GLHandler class is merely a convenient wrapper you will use to draw. Most of the higher level concepts wrapped are close to the OpenGL low-level concepts (for example, a Mesh is only a struct containing GLuint pointing to vertex or element buffers).

## To use the template

Make sure your new project is an empty repository... (It will be easier for initial merge.)

Add an empty commit to master and push it to origin so that master branch exists.

    git checkout -b master #if needed
    git commit --allow-empty
    git push origin master

Add the repo as the source remote (for example):

	git remote add source https://gitlab.com/Dexter9313/hydrogenvr.git

This new branch will be used to pull updates from this repo :

	git checkout -b update-source

First pull will need an extra parameter to be allowed by git (both repos don't share any history) :

	git pull source master --allow-unrelated-histories
	git branch -u source/master # only once
	
Rebase everything needed to keep a clear git log in your project.

Then from any other branch in the project (master for example with git checkout master) :

	git merge update-source

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
* Add settings to utils.cpp:initSettings().
* Add assets to the data/ directory. The working directory will always contain data (so you can always reach data relatively from this path : "data/"). It will be packaged with the rest of the project.

The project name defined as CMakeLists.txt PROJECT_NAME will be accessible in C++ code as the PROJECT_NAME macro. It is a C-style string constant.

All used classes starting with Q (ex: QSettings) belongs to the Qt framework. The engine didn't wrap anything from Qt, so you will have to use Qt a least a little.

## Example releases

You can see releases for Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [HydrogenVR Releases](https://github.com/Dexter9313/HydrogenVR-mirror/releases).
