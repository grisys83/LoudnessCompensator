#!/bin/sh
set -euo pipefail

if [[ "${CONFIGURATION}" == "Debug" ]]; then
  destinationPlugin="${HOME}/Library/Audio/Plug-Ins/Components//$(basename "${TARGET_BUILD_DIR}/${WRAPPER_NAME}")"
  echo Running rm -rf \"${destinationPlugin}\"
  rm -rf "${destinationPlugin}"
  echo Running ditto \"${TARGET_BUILD_DIR}/${WRAPPER_NAME}\" \"${destinationPlugin}\"
  ditto "${TARGET_BUILD_DIR}/${WRAPPER_NAME}" "${destinationPlugin}"

  if [[ -n "${EXPANDED_CODE_SIGN_IDENTITY-}" ]]; then
    if [[ -n "${CODE_SIGN_ENTITLEMENTS-}" ]]; then
      entitlementsArg=(--entitlements "${CODE_SIGN_ENTITLEMENTS}")
    fi

    echo Signing Identity: \"${EXPANDED_CODE_SIGN_IDENTITY_NAME}\"
    echo Running codesign --verbose=4 --force --sign \"${EXPANDED_CODE_SIGN_IDENTITY}\" ${entitlementsArg[*]-} ${OTHER_CODE_SIGN_FLAGS-} \"${HOME}/Library/Audio/Plug-Ins/Components//${WRAPPER_NAME}\"
    codesign --verbose=4 --force --sign "${EXPANDED_CODE_SIGN_IDENTITY}" ${entitlementsArg[*]-} ${OTHER_CODE_SIGN_FLAGS-} "${HOME}/Library/Audio/Plug-Ins/Components//${WRAPPER_NAME}"
  fi
fi

if [[ "${CONFIGURATION}" == "Release" ]]; then
  destinationPlugin="${HOME}/Library/Audio/Plug-Ins/Components//$(basename "${TARGET_BUILD_DIR}/${WRAPPER_NAME}")"
  echo Running rm -rf \"${destinationPlugin}\"
  rm -rf "${destinationPlugin}"
  echo Running ditto \"${TARGET_BUILD_DIR}/${WRAPPER_NAME}\" \"${destinationPlugin}\"
  ditto "${TARGET_BUILD_DIR}/${WRAPPER_NAME}" "${destinationPlugin}"

  if [[ -n "${EXPANDED_CODE_SIGN_IDENTITY-}" ]]; then
    if [[ -n "${CODE_SIGN_ENTITLEMENTS-}" ]]; then
      entitlementsArg=(--entitlements "${CODE_SIGN_ENTITLEMENTS}")
    fi

    echo Signing Identity: \"${EXPANDED_CODE_SIGN_IDENTITY_NAME}\"
    echo Running codesign --verbose=4 --force --sign \"${EXPANDED_CODE_SIGN_IDENTITY}\" ${entitlementsArg[*]-} ${OTHER_CODE_SIGN_FLAGS-} \"${HOME}/Library/Audio/Plug-Ins/Components//${WRAPPER_NAME}\"
    codesign --verbose=4 --force --sign "${EXPANDED_CODE_SIGN_IDENTITY}" ${entitlementsArg[*]-} ${OTHER_CODE_SIGN_FLAGS-} "${HOME}/Library/Audio/Plug-Ins/Components//${WRAPPER_NAME}"
  fi
fi

