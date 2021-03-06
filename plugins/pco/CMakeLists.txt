cmake_minimum_required(VERSION 2.8)
project(ucapco C)

find_package(PCO)
find_package(FgLib5)
find_package(ClSerMe4)

if (PCO_FOUND AND CLSERME4_FOUND AND FGLIB5_FOUND)
    set(UCA_CAMERA_NAME "pco")
    set(PLUGIN_VERSION "1.1.0")
    set(PLUGIN_REVISION "0")
    set(PLUGIN_REQUIRES "libuca >= 1.1.0")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../package-plugin.sh.in
                   ${CMAKE_CURRENT_BINARY_DIR}/../../package-plugin-${UCA_CAMERA_NAME}.sh)
    include_directories(${PCO_INCLUDE_DIRS}
                        ${CLSERME4_INCLUDE_DIR}
                        ${FGLIB5_INCLUDE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_BINARY_DIR})

    create_enums(uca-pco-enums
                 ${CMAKE_CURRENT_SOURCE_DIR}/../../src/uca-enums
                 uca-pco-camera.h)

    add_library(ucapco SHARED
                uca-pco-camera.c
                uca-pco-enums.c)

    target_link_libraries(ucapco
                          ${UCA_DEPS}
                          ${PCO_LIBRARIES}
                          ${CLSERME4_LIBRARY}
                          ${FGLIB5_LIBRARY})

    install(TARGETS ucapco
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}/uca
            COMPONENT ${UCA_CAMERA_NAME})
endif()
