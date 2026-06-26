# Granular reverb<br>
This granular reverb is based on my [audio reverb](https://github.com/nilsjc/AudioReverb)<br>
It will be much more experimental. Right now it isn´t working.
Work in progress...


## Some build instructions
### How to build this project with mingw32 and cmake on Windows.<br>

step 1: create build folder<br>
step 2: cd build<br>
step 3: cmake -S .. -B . -G "MinGW Makefiles"<br>
step 4: mingw32-make.exe<br>
<br>

### How to install wxWidgets on Windows with MinGW compiler:<br>
step 1: download wxwidgets as an install.exe file<br>
step 2: run the exe. wxwidgets will create a folder on c: root, like: **C:/wxWidgets-3.2.5**<br>
step 3: build wxwidgets, [follow these instructions](https://wiki.wxwidgets.org/Compiling_wxWidgets_with_MinGW)<br>
Note that these instructions is for MinGW. If you use other compilers the build process is different.<br>
This will take about 20 mins on my machine to build.<br>
step 4: add this path to windows enviroment variables: **C:\wxWidgets-3.2.5\lib\gcc_dll** <br>
<br>
Now it should work. BTW, here is a minimum CMAKE project using wxWidgets:
<br>
`cmake_minimum_required(VERSION 3.29.2)`<br>
`project(wxtest)`<br>
`SET(wxWidgets_USE_LIBS)`<br>
`#for using RichTextCtrl`<br>
`FIND_PACKAGE(wxWidgets REQUIRED richtext aui adv html core xml net base)` <br>
`add_executable(${PROJECT_NAME} WIN32 main.cpp)`<br>
`TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${wxWidgets_LIBRARIES})`<br>
<br>
info about wxwidgets is based on this stackoverflow thread:
https://stackoverflow.com/questions/63977690/how-wxwidgets-cmake-find-in-window
