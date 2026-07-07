# Granular reverb<br>
![Alt text](reverb2.PNG?raw=true "Work of progress")<br>
This granular reverb is based on my [audio reverb](https://github.com/nilsjc/AudioReverb)<br>
It is much more experimental. The GUI is maybe not super inspiring but that's OK because it does its job. My plan is anyway just to experiment with sound processing.<br>
To try this out, connect live sound into your computer mic input connector and hit the loop record button. The tape loop recording simulator will record sound depending on the setting of the bottom slider to right. But it will always be a loop of four seconds. So if you extend the loop end after a recordin of just one second, you will get extra three seconds of silence. This is good for trying out a reverb. No visual will tell you when the record is in progress and done, but it will record sound.<br> After recording sound press play loop, and adjust the pitch slider, the bottom slider to left and the loop length.<br>
The reverb have a lot of parameters and it can be a interesting chorus as well. But at the moment i dont like the sound - i think it lacks the warmth of Dattorro's reverb. But it have potential and i will try to improve it in the future.<br>
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
