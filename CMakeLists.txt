cmake_minimum_required(VERSION 3.29.2)
project(reverb)

if(GIT_FOUND AND EXISTS ".git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

SET(wxWidgets_USE_LIBS)
#for using RichTextCtrl
FIND_PACKAGE(wxWidgets REQUIRED richtext aui adv html core xml net base) 
IF(wxWidgets_FOUND)
    INCLUDE("${wxWidgets_USE_FILE}")
    add_subdirectory(external)
    add_executable(${PROJECT_NAME} WIN32 main.cpp 
        AudioStream.h
        AudioStream.cpp
        SynthManager.h
        SynthManager.cpp
        PlateReverb.h
        PlateReverb.cpp
        FdnReverb.h
        FdnReverb.cpp)

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} PortAudio)
ELSE(wxWidgets_FOUND)
    MESSAGE("wxWidgets not found!")
ENDIF(wxWidgets_FOUND)


#if(NOT EXISTS ${CMAKE_PROJECT_NAME}/sound/external/CMakeLists.txt)
#    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
#endif()
