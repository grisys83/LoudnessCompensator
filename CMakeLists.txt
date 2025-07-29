cmake_minimum_required(VERSION 3.22)

# Project definition
project(LoudnessCompensator VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE framework
find_package(PkgConfig REQUIRED)

# Add JUCE as a subdirectory (assuming JUCE is in parent directory)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
    add_subdirectory(../JUCE JUCE)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE JUCE)
else()
    message(FATAL_ERROR "JUCE not found. Please ensure JUCE is available in parent directory or as JUCE subdirectory")
endif()

# Plugin definition
juce_add_plugin(LoudnessCompensator
    # Basic plugin settings
    COMPANY_NAME "Hyang"
    PLUGIN_MANUFACTURER_CODE "Hyang"
    PLUGIN_CODE "LdCs"
    
    # Plugin formats (conditional based on platform)
    FORMATS VST3 Standalone $<$<PLATFORM_ID:Linux>:LV2>
    
    # Plugin properties
    PRODUCT_NAME "Loudness Compensator"
    PLUGIN_NAME "LoudnessCompensator"
    DESCRIPTION "Perceptual loudness compensation based on ISO 226:2003"
    
    # Version
    VERSION ${PROJECT_VERSION}
    
    # Plugin characteristics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    
    # Copy plugin after build
    COPY_PLUGIN_AFTER_BUILD TRUE
    
    # Plugin categories
    VST3_CATEGORIES "Fx" "EQ"
    $<$<PLATFORM_ID:Linux>:LV2_URI "http://hyang.audio/plugins/LoudnessCompensator">
    $<$<PLATFORM_ID:Linux>:LV2_CATEGORIES "EQPlugin">
)

# Source files
target_sources(LoudnessCompensator
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DSP/LoudnessCompensatorDSP.cpp
        Source/DSP/LoudnessCompensatorDSP.h
        Source/DSP/ISO226Data.h
)

# Compiler definitions
target_compile_definitions(LoudnessCompensator
    PUBLIC
        # JUCE plugin defines
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
    PRIVATE
        # Plugin specific defines
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_ALSA=1
        JUCE_JACK=1
)

# Link libraries
target_link_libraries(LoudnessCompensator
    PRIVATE
        # JUCE modules
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Linux specific settings
if(UNIX AND NOT APPLE)
    # Find required Linux packages
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(X11 REQUIRED x11)
    
    target_link_libraries(LoudnessCompensator
        PRIVATE
            ${ALSA_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            ${X11_LIBRARIES}
            pthread
            dl
    )
    
    target_include_directories(LoudnessCompensator
        PRIVATE
            ${ALSA_INCLUDE_DIRS}
            ${FREETYPE_INCLUDE_DIRS}
            ${X11_INCLUDE_DIRS}
    )
    
    # Set installation directories for Linux
    set(VST3_INSTALL_DIR "~/.vst3" CACHE STRING "VST3 installation directory")
    set(LV2_INSTALL_DIR "~/.lv2" CACHE STRING "LV2 installation directory")
    
    # Install targets
    install(TARGETS LoudnessCompensator_VST3
        DESTINATION ${VST3_INSTALL_DIR}
        COMPONENT VST3
    )
    
    if(TARGET LoudnessCompensator_LV2)
        install(TARGETS LoudnessCompensator_LV2
            DESTINATION ${LV2_INSTALL_DIR}
            COMPONENT LV2
        )
    endif()
endif()

# Print configuration info
message(STATUS "LoudnessCompensator Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
if(UNIX AND NOT APPLE)
    message(STATUS "  VST3 install dir: ${VST3_INSTALL_DIR}")
    message(STATUS "  LV2 install dir: ${LV2_INSTALL_DIR}")
endif()