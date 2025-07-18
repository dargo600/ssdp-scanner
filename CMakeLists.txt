# Updated Cmake was based on below Dave Turgeon
# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(ssdpscanner LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(ssdpscanner
    main.cpp
    mainwindow.cpp mainwindow.h
)

set_target_properties(ssdpscanner PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(ssdpscanner PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
)



install(TARGETS ssdpscanner
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ssdpscanner
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
