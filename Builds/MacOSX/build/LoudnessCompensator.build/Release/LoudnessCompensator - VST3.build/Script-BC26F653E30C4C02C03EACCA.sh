#!/bin/sh
set -euo pipefail

echo Running codesign --verbose=4 --force --sign - \"${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}\"
codesign --verbose=4 --force --sign - "${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}"

echo Running \"${CONFIGURATION_BUILD_DIR}/juce_vst3_helper\" -create -version \"1.0.0\" -path \"${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}\" -output \"${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}/Contents/Resources/moduleinfo.json\"
"${CONFIGURATION_BUILD_DIR}/juce_vst3_helper" -create -version "1.0.0" -path "${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}" -output "${CONFIGURATION_BUILD_DIR}/${WRAPPER_NAME}/Contents/Resources/moduleinfo.json"

