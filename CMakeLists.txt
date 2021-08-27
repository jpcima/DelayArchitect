cmake_minimum_required(VERSION 3.7)
project(DelayArchitect VERSION "0.0.1" LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(BuildType)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
  add_definitions("-DNOMINMAX=1")
  add_definitions("-D_USE_MATH_DEFINES=1")
endif()
if(MSVC)
  add_compile_options("/utf-8")
endif()

find_package(PkgConfig)
add_subdirectory("thirdparty/JUCE" EXCLUDE_FROM_ALL)

option(GD_BENCHMARKS "Build benchmarks" OFF)
set(GD_PITCH_SHIFTER_TYPE "SuperCollider" CACHE STRING "Pitch shifter implementation to use")
set_property(CACHE GD_PITCH_SHIFTER_TYPE PROPERTY STRINGS "SuperCollider" "SoundTouch" "Simple")

juce_add_plugin(DelayArchitect
  PLUGIN_CODE Gdly
  PLUGIN_MANUFACTURER_CODE Jpci
  PRODUCT_NAME "Delay Architect"
  COMPANY_NAME "Jean Pierre Cimalando"
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

target_compile_definitions(DelayArchitect
  PUBLIC
  "JUCE_WEB_BROWSER=0"
  "JUCE_USE_CURL=0"
  "JUCE_VST3_CAN_REPLACE_VST2=0"
  "JUCE_DISPLAY_SPLASH_SCREEN=0")

target_include_directories(DelayArchitect
  PRIVATE
  "sources")

target_sources(DelayArchitect
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
  "sources/editor/parts/AdvancedTooltipWindow.h"
  "sources/editor/parts/AdvancedTooltipWindow.cpp"
  "sources/editor/attachments/TapParameterAttachment.h"
  "sources/editor/attachments/TapParameterAttachment.cpp"
  "sources/editor/attachments/GridParameterAttachment.h"
  "sources/editor/attachments/GridParameterAttachment.cpp"
  "sources/editor/attachments/AutomaticComboBoxParameterAttachment.h"
  "sources/editor/attachments/AutomaticComboBoxParameterAttachment.cpp"
  "sources/editor/attachments/InvertedButtonParameterAttachment.h"
  "sources/editor/attachments/InvertedButtonParameterAttachment.cpp"
  "sources/editor/attachments/SliderParameterAttachmentWithTooltip.h"
  "sources/editor/attachments/SliderParameterAttachmentWithTooltip.cpp"
  "sources/editor/importer/Importer.cpp"
  "sources/editor/importer/Importer.h"
  "sources/editor/importer/ImporterPST.cpp"
  "sources/editor/importer/ImporterPST.h"
  "sources/editor/utility/FunctionalTimer.h"
  "sources/utility/AutoDeletePool.h"
  "sources/utility/AutoDeletePool.hpp")

juce_add_binary_data(DelayArchitectResources
  SOURCES
  "resources/fonts/LiberationSans-Regular.ttf")

target_link_libraries(DelayArchitect
  PRIVATE
  DelayArchitectResources
  juce::juce_audio_processors
  juce::juce_opengl
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)

if(MSVC)
  target_compile_options(DelayArchitect PRIVATE "/fp:fast")
else()
  target_compile_options(DelayArchitect PRIVATE "-ffast-math")
endif()

###
add_library(Gd STATIC
  "sources/gd/Gd.cpp"
  "sources/gd/Gd.h"
  "sources/gd/GdDefs.cpp"
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
  "sources/gd/shifters/GdShifterSimple.cpp"
  "sources/gd/shifters/GdShifterSimple.h"
  "sources/gd/shifters/GdShifterSimple.hpp"
  "sources/gd/shifters/GdShifterSoundTouch.cpp"
  "sources/gd/shifters/GdShifterSoundTouch.h"
  "sources/gd/shifters/GdShifterSuperCollider.cpp"
  "sources/gd/shifters/GdShifterSuperCollider.h"
  "sources/gd/utility/LinearSmoother.cpp"
  "sources/gd/utility/LinearSmoother.h"
  "sources/gd/utility/Clamp.h"
  "sources/gd/utility/NextPowerOfTwo.h"
  "sources/gd/utility/Volume.h"
  "sources/gd/utility/Locale.cpp"
  "sources/gd/utility/Locale.h"
  "sources/gd/utility/Locale.hpp")
set_source_files_properties(
  "sources/gd/shifters/GdShifterSimple.cpp"
  "sources/gd/shifters/GdShifterSoundTouch.cpp"
  "sources/gd/shifters/GdShifterSuperCollider.cpp"
  PROPERTIES
  HEADER_FILE_ONLY ON)
target_include_directories(Gd
  PUBLIC
  "sources/gd")

###
target_link_libraries(DelayArchitect
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
  PUBLIC
  "CMAKE"
  "SOUNDTOUCH_FLOAT_SAMPLES=1"
  "SOUNDTOUCH_USE_NEON=1")
target_include_directories(SoundTouch
  PUBLIC
  "thirdparty/SoundTouch/include")

###
if(GD_PITCH_SHIFTER_TYPE STREQUAL "SoundTouch")
  target_compile_definitions(Gd
    PRIVATE
    "GD_USE_SOUNDTOUCH_PITCH_SHIFTER=1")
  target_link_libraries(Gd
    PRIVATE
    SoundTouch)
elseif(GD_PITCH_SHIFTER_TYPE STREQUAL "SuperCollider")
  target_compile_definitions(Gd
    PRIVATE
    "GD_USE_SUPERCOLLIDER_PITCH_SHIFTER=1")
elseif(GD_PITCH_SHIFTER_TYPE STREQUAL "Simple")
  target_compile_definitions(Gd
    PRIVATE
    "GD_USE_SIMPLE_PITCH_SHIFTER=1")
else()
  message(FATAL_ERROR "Invalid value for pitch shifter")
endif()

###
if(GD_BENCHMARKS)
  pkg_check_modules(benchmark "benchmark" REQUIRED IMPORTED_TARGET)
  add_executable(GdBenchmarkLinearSmoother "benchmarks/LinearSmoother.cpp")
  target_link_libraries(GdBenchmarkLinearSmoother PRIVATE Gd PkgConfig::benchmark simde)
endif()
