cmake_minimum_required(VERSION 3.15)

project(Hera VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HERA_DESIGNER "Display the user interface design panel" OFF)
option(HERA_FORCE_DEBUG "Enable debug mode regardless of build configuration" OFF)

add_subdirectory("Thirdparty/JUCE" EXCLUDE_FROM_ALL)

if(HERA_FORCE_DEBUG)
  target_compile_definitions(juce_core
    INTERFACE
    "JUCE_FORCE_DEBUG=1")
endif()

juce_add_plugin(Hera
  PLUGIN_CODE Hera
  PLUGIN_MANUFACTURER_CODE AcEl
  PRODUCT_NAME "Hera"
  COMPANY_NAME "A/C Electronics"
  FORMATS AU VST3
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(Hera
  PRIVATE
  "Source/Hera.h"
  "Source/Hera.cpp"
  "Source/HeraEnvelope.h"
  "Source/HeraEnvelope.cpp"
  "Source/HeraLFO.h"
  "Source/HeraLFO.cpp"
  "Source/HeraLFOWithEnvelope.h"
  "Source/HeraLFOWithEnvelope.cpp"
  "Source/HeraVCF.h"
  "Source/HeraTables.h"
  "Source/HeraTables.cpp"
  "Source/HeraProgram.h"
  "Source/HeraProgram.cpp"
  "Source/PluginEditor.cpp"
  "Source/PluginEditor.h"
  "Source/PluginProcessor.cpp"
  "Source/PluginProcessor.h"
  "Source/Pak.cpp"
  "Source/Pak.h"
  "Source/bbd_filter.cpp"
  "Source/bbd_filter.h"
  "Source/bbd_line.cpp"
  "Source/bbd_line.h"
  "Source/VCF/DangeloVCF.h"
  "Source/VCF/PirkleVCF.h"
  "Source/VCF/PirkleVCF.cpp")

target_include_directories(Hera
  PRIVATE
  "Source")

target_compile_definitions(Hera
  PUBLIC
  "JUCE_WEB_BROWSER=0"
  "JUCE_USE_CURL=0"
  "JUCE_VST3_CAN_REPLACE_VST2=0"
  "JUCE_DISPLAY_SPLASH_SCREEN=0")

target_link_libraries(Hera
  PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags)
  #juce::juce_recommended_warning_flags

###
juce_add_binary_data(HeraData
  SOURCES
  "Resources/presets.pak")

set_target_properties(HeraData PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Resources")

file(GLOB PRESET_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "Presets/*.xml")

find_program(RDMD "rdmd")
if(NOT RDMD)
  message(STATUS "The program \"rdmd\" is not available, the presets will not be rebuilt.")
else()
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Resources/presets.pak"
    COMMAND "${RDMD}" "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/pak.d" create "${CMAKE_CURRENT_SOURCE_DIR}/Resources/presets.pak" ${PRESET_FILES}
    DEPENDS ${PRESET_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

target_link_libraries(Hera
  PRIVATE
  HeraData)

###
juce_add_modules(
  ALIAS_NAMESPACE foleys
  "Thirdparty/Modules/foleys_gui_magic")

if(NOT HERA_DESIGNER)
  target_compile_definitions(foleys_gui_magic
    INTERFACE
    "FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0")
endif()

###
target_link_libraries(Hera
  PRIVATE
  foleys::foleys_gui_magic)

###
add_library(hiir INTERFACE)
target_include_directories(hiir INTERFACE "Thirdparty/hiir")

###
target_link_libraries(Hera
  PRIVATE
  hiir)

###
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(Hera PRIVATE "-Wall")
endif()
