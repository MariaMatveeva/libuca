cmake_minimum_required(VERSION 2.8)
project(ucapylon C)


if (PYLON_FOUND)
    set(UCA_CAMERA_NAME "pylon")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../package-plugin.sh.in
                   ${CMAKE_CURRENT_BINARY_DIR}/../../package-plugin-${UCA_CAMERA_NAME}.sh)

    include_directories(${LIBPYLONCAM_INCLUDEDIR})

    add_library(ucapylon SHARED
                uca-pylon-camera.c)

    target_link_libraries(ucapylon
                          ${UCA_DEPS}
                          ${LIBPYLONCAM_LIBRARIES})

    install(TARGETS ucapylon
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}/uca
            COMPONENT ${UCA_CAMERA_NAME})
endif()
