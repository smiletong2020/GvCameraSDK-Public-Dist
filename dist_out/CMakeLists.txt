cmake_minimum_required(VERSION 3.20)

project(GvCameraSDKPublicDist LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "This distribution currently supports Windows builds only.")
endif()

option(BUILD_SAMPLES "Build GvCameraSDK sample executables" ON)
option(GVSDK_INCLUDE_SAMPLE_EXES_IN_INSTALL "Install built sample executables to bin/<Config>" ON)
option(GVSDK_ENABLE_DEBUG_VARIANT "Enable Debug SDK variant (GvCameraSDKd)" ON)

set(GVSDK_DIST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(GVSDK_INCLUDE_DIR "${GVSDK_DIST_ROOT}/include/GvCameraSDK")
set(GVSDK_RELEASE_LIBRARY_FILE "${GVSDK_DIST_ROOT}/lib/GvCameraSDK.lib")
set(GVSDK_DEBUG_LIBRARY_FILE "${GVSDK_DIST_ROOT}/lib/GvCameraSDKd.lib")
set(GVSDK_RELEASE_RUNTIME_DLL "${GVSDK_DIST_ROOT}/bin/GvCameraSDK.dll")
set(GVSDK_DEBUG_RUNTIME_DLL "${GVSDK_DIST_ROOT}/bin/GvCameraSDKd.dll")

if(NOT EXISTS "${GVSDK_INCLUDE_DIR}/GvCameraAPI.h")
    message(FATAL_ERROR "Missing SDK header: ${GVSDK_INCLUDE_DIR}/GvCameraAPI.h")
endif()
if(NOT EXISTS "${GVSDK_RELEASE_LIBRARY_FILE}")
    message(FATAL_ERROR "Missing Release import library: ${GVSDK_RELEASE_LIBRARY_FILE}")
endif()
if(NOT EXISTS "${GVSDK_RELEASE_RUNTIME_DLL}")
    message(FATAL_ERROR "Missing Release runtime DLL: ${GVSDK_RELEASE_RUNTIME_DLL}")
endif()
if(GVSDK_ENABLE_DEBUG_VARIANT)
    if(NOT EXISTS "${GVSDK_DEBUG_LIBRARY_FILE}")
        message(FATAL_ERROR "Missing Debug import library: ${GVSDK_DEBUG_LIBRARY_FILE}")
    endif()
    if(NOT EXISTS "${GVSDK_DEBUG_RUNTIME_DLL}")
        message(FATAL_ERROR "Missing Debug runtime DLL: ${GVSDK_DEBUG_RUNTIME_DLL}")
    endif()
endif()

add_library(GvCameraSDK::GvCameraSDK SHARED IMPORTED GLOBAL)
set_target_properties(
    GvCameraSDK::GvCameraSDK
    PROPERTIES
        IMPORTED_CONFIGURATIONS RELEASE
        IMPORTED_IMPLIB_RELEASE "${GVSDK_RELEASE_LIBRARY_FILE}"
        IMPORTED_LOCATION_RELEASE "${GVSDK_RELEASE_RUNTIME_DLL}"
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
)
if(GVSDK_ENABLE_DEBUG_VARIANT)
    set_property(TARGET GvCameraSDK::GvCameraSDK APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(
        GvCameraSDK::GvCameraSDK
        PROPERTIES
            IMPORTED_IMPLIB_DEBUG "${GVSDK_DEBUG_LIBRARY_FILE}"
            IMPORTED_LOCATION_DEBUG "${GVSDK_DEBUG_RUNTIME_DLL}"
    )
else()
    set_target_properties(
        GvCameraSDK::GvCameraSDK
        PROPERTIES
            MAP_IMPORTED_CONFIG_DEBUG Release
    )
endif()

set(GVSDK_COMMON_RUNTIME_DLLS)
file(GLOB GVSDK_ALL_RUNTIME_DLLS "${GVSDK_DIST_ROOT}/bin/*.dll")
foreach(runtime_dll IN LISTS GVSDK_ALL_RUNTIME_DLLS)
    get_filename_component(runtime_name "${runtime_dll}" NAME)
    if(runtime_name STREQUAL "GvCameraSDK.dll" OR runtime_name STREQUAL "GvCameraSDKd.dll")
        continue()
    endif()
    list(APPEND GVSDK_COMMON_RUNTIME_DLLS "${runtime_dll}")
endforeach()
set(GVSDK_RELEASE_RUNTIME_DLLS "${GVSDK_RELEASE_RUNTIME_DLL};${GVSDK_COMMON_RUNTIME_DLLS}")
if(GVSDK_ENABLE_DEBUG_VARIANT)
    set(GVSDK_DEBUG_RUNTIME_DLLS "${GVSDK_DEBUG_RUNTIME_DLL};${GVSDK_COMMON_RUNTIME_DLLS}")
else()
    set(GVSDK_DEBUG_RUNTIME_DLLS "")
endif()

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

install(FILES
    "${GVSDK_DIST_ROOT}/README.md"
    "${GVSDK_DIST_ROOT}/manifest.txt"
    "${GVSDK_DIST_ROOT}/CMakeLists.txt"
    DESTINATION "."
)

install(DIRECTORY "${GVSDK_DIST_ROOT}/include/" DESTINATION "include")
install(
    FILES "${GVSDK_RELEASE_LIBRARY_FILE}"
    DESTINATION "lib/Release"
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
)
install(
    FILES ${GVSDK_RELEASE_RUNTIME_DLLS}
    DESTINATION "bin/Release"
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
)
if(GVSDK_ENABLE_DEBUG_VARIANT)
    install(
        FILES "${GVSDK_DEBUG_LIBRARY_FILE}"
        DESTINATION "lib/Debug"
        CONFIGURATIONS Debug
    )
    install(
        FILES ${GVSDK_DEBUG_RUNTIME_DLLS}
        DESTINATION "bin/Debug"
        CONFIGURATIONS Debug
    )
endif()
install(DIRECTORY "${GVSDK_DIST_ROOT}/docs/" DESTINATION "docs")
install(DIRECTORY "${GVSDK_DIST_ROOT}/licenses/" DESTINATION "licenses")

install(FILES
    "${GVSDK_DIST_ROOT}/samples/gvsdk_capture2d_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/gvsdk_capture3d_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/gvsdk_fix_ip_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/gvsdk_list_devices_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/gvsdk_open_device_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/gvsdk_version_sample.cpp"
    "${GVSDK_DIST_ROOT}/samples/CMakeLists.txt"
    DESTINATION "samples"
)
