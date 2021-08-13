cmake_minimum_required(VERSION 3.7)
project(GroovyDelay VERSION "0.0.1" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory("thirdparty/JUCE" EXCLUDE_FROM_ALL)

juce_add_plugin(GroovyDelay
  PLUGIN_CODE Gdly
  PLUGIN_MANUFACTURER_CODE Jpci
  PRODUCT_NAME "Groovy Delay"
  COMPANY_NAME "Groovy inc."
  FORMATS AU VST3
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  VST3_CATEGORIES "Fx Delay"
  AU_MAIN_TYPE "kAudioUnitType_MusicEffect"
  COPY_PLUGIN_AFTER_BUILD FALSE
  NEEDS_CURL FALSE
  NEEDS_WEB_BROWSER FALSE)

target_compile_definitions(GroovyDelay
  PUBLIC
  "JUCE_WEB_BROWSER=0"
  "JUCE_USE_CURL=0"
  "JUCE_VST3_CAN_REPLACE_VST2=0"
  "JUCE_DISPLAY_SPLASH_SCREEN=0")

target_include_directories(GroovyDelay
  PRIVATE
  "sources")

target_sources(GroovyDelay
  PRIVATE
  "sources/processor/Processor.h"
  "sources/processor/Processor.cpp"
  "sources/editor/Editor.h"
  "sources/editor/Editor.cpp")

target_link_libraries(GroovyDelay
  PRIVATE
  juce::juce_audio_processors
  juce::juce_opengl
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)

if(MSVC)
  target_compile_options(GroovyDelay PRIVATE "/fp:fast")
else()
  target_compile_options(GroovyDelay PRIVATE "-ffast-math")
endif()

###
add_library(Gd STATIC
  "sources/gd/Gd.cpp"
  "sources/gd/Gd.h"
  "sources/gd/GdDefs.h"
  "sources/gd/GdLine.cpp"
  "sources/gd/GdLine.h"
  "sources/gd/GdNetwork.cpp"
  "sources/gd/GdNetwork.h"
  "sources/gd/GdTapFx.cpp"
  "sources/gd/GdTapFx.h"
  "sources/gd/utility/LinearSmoother.cpp"
  "sources/gd/utility/LinearSmoother.h"
  "sources/gd/utility/NextPowerOfTwo.h"
  "sources/gd/utility/Volume.h")
target_include_directories(Gd
  PUBLIC
  "sources/gd")

###
target_link_libraries(GroovyDelay
  PRIVATE
  Gd)

###
add_library(simde INTERFACE)
target_include_directories(simde
  INTERFACE
  "thirdparty/simde")

###
target_link_libraries(Gd
  PRIVATE
  simde)

###
find_package(OpenMP)
if(OPENMP_FOUND)
    add_library(openmp INTERFACE)
    # OpenMP flags are provided as a space-separated string, we need a list
    if(NOT CMAKE_VERSION VERSION_LESS 3.9)
        separate_arguments(OpenMP_C_OPTIONS NATIVE_COMMAND "${OpenMP_C_FLAGS}")
        separate_arguments(OpenMP_CXX_OPTIONS NATIVE_COMMAND "${OpenMP_CXX_FLAGS}")
    elseif(CMAKE_HOST_WIN32)
        separate_arguments(OpenMP_C_OPTIONS WINDOWS_COMMAND "${OpenMP_C_FLAGS}")
        separate_arguments(OpenMP_CXX_OPTIONS WINDOWS_COMMAND "${OpenMP_CXX_FLAGS}")
    else()
        separate_arguments(OpenMP_C_OPTIONS UNIX_COMMAND "${OpenMP_C_FLAGS}")
        separate_arguments(OpenMP_CXX_OPTIONS UNIX_COMMAND "${OpenMP_CXX_FLAGS}")
    endif()
    target_compile_options(openmp INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${OpenMP_C_OPTIONS}>
        $<$<COMPILE_LANGUAGE:CXX>:${OpenMP_CXX_OPTIONS}>)
endif()

###
target_link_libraries(Gd
  PRIVATE
  openmp)
