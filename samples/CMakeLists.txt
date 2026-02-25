set(GVSDK_SAMPLE_SOURCES
    gvsdk_version_sample.cpp
    gvsdk_list_devices_sample.cpp
    gvsdk_open_device_sample.cpp
    gvsdk_fix_ip_sample.cpp
    gvsdk_capture2d_sample.cpp
    gvsdk_capture3d_sample.cpp
)

if(GVSDK_RELEASE_RUNTIME_DLLS STREQUAL "")
    message(FATAL_ERROR "No runtime DLLs were found under ${GVSDK_DIST_ROOT}/bin")
endif()

set(GVSDK_SAMPLE_TARGETS)
foreach(sample_source IN LISTS GVSDK_SAMPLE_SOURCES)
    get_filename_component(sample_name "${sample_source}" NAME_WE)
    add_executable("${sample_name}" "${sample_source}")
    target_compile_features("${sample_name}" PRIVATE cxx_std_17)
    target_include_directories("${sample_name}" PRIVATE "${GVSDK_INCLUDE_DIR}")
    target_link_libraries("${sample_name}" PRIVATE GvCameraSDK::GvCameraSDK)
    if(MSVC)
        target_compile_options("${sample_name}" PRIVATE /utf-8)
    endif()
    set_target_properties(
        "${sample_name}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/samples/bin/$<CONFIG>"
    )

    foreach(runtime_dll IN LISTS GVSDK_COMMON_RUNTIME_DLLS)
        add_custom_command(
            TARGET "${sample_name}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${runtime_dll}"
                "$<TARGET_FILE_DIR:${sample_name}>"
        )
    endforeach()

    add_custom_command(
        TARGET "${sample_name}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${GVSDK_RELEASE_RUNTIME_DLL}"
            "$<TARGET_FILE_DIR:${sample_name}>"
    )

    list(APPEND GVSDK_SAMPLE_TARGETS "${sample_name}")
endforeach()

if(GVSDK_INCLUDE_SAMPLE_EXES_IN_INSTALL)
    install(TARGETS ${GVSDK_SAMPLE_TARGETS} RUNTIME DESTINATION "bin/$<CONFIG>")
endif()
