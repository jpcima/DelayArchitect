cmake_minimum_required(VERSION 3.7)
project(GroovyDelay VERSION "0.0.1" LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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
  "sources/editor/Editor.cpp"
  "sources/editor/LookAndFeel.h"
  "sources/editor/LookAndFeel.cpp"
  "sources/editor/parts/MainComponent.h"
  "sources/editor/parts/MainComponent.cpp"
  "sources/editor/parts/TapEditScreen.h"
  "sources/editor/parts/TapEditScreen.cpp"
  "sources/editor/parts/TapSlider.h"
  "sources/editor/parts/TapSlider.cpp"
  "sources/editor/utility/FunctionalTimer.h")

juce_add_binary_data(GroovyDelayResources
  SOURCES
  "resources/fonts/LiberationSans-Regular.ttf")

target_link_libraries(GroovyDelay
  PRIVATE
  GroovyDelayResources
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
  "sources/gd/GdFilter.cpp"
  "sources/gd/GdFilter.h"
  "sources/gd/GdFilter.hpp"
  "sources/gd/GdShifter.cpp"
  "sources/gd/GdShifter.h"
  "sources/gd/GdShifter.hpp"
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

###
add_library(SoundTouch STATIC EXCLUDE_FROM_ALL
  "thirdparty/SoundTouch/source/SoundTouch/AAFilter.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/BPMDetect.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/cpu_detect_x86.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/FIFOSampleBuffer.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/FIRFilter.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/InterpolateCubic.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/InterpolateLinear.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/InterpolateShannon.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/mmx_optimized.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/PeakFinder.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/RateTransposer.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/SoundTouch.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/sse_optimized.cpp"
  "thirdparty/SoundTouch/source/SoundTouch/TDStretch.cpp")
target_compile_definitions(SoundTouch
  PRIVATE
  "CMAKE"
  "SOUNDTOUCH_FLOAT_SAMPLES=1"
  "SOUNDTOUCH_USE_NEON=1")
target_include_directories(SoundTouch
  PUBLIC
  "thirdparty/SoundTouch/include")

###
target_link_libraries(Gd
  PRIVATE
  SoundTouch)
