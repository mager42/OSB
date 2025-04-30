cmake_minimum_required(VERSION 3.16)

project(OpenSnapBooth VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.4)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GST_PLG_BASE REQUIRED gstreamer-gl-1.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)

find_package(Cups REQUIRED)

include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${GST_PLG_BASE_INCLUDE_DIRS})
include_directories(${GST_APP_INCLUDE_DIRS})
include_directories(${CUPS_INCLUDE_DIRS})
link_directories(${GSTREAMER_LIBRARY_DIRS})

qt_add_executable(appOpenSnapBooth
    main.cpp
    settingsmanager.h settingsmanager.cpp
    pictureutils.h pictureutils.cpp
    gstreamerpipeline.h gstreamerpipeline.cpp
    printer.hpp
    cups_printer.hpp cups_printer.cpp
    resources.qrc
)

qt_add_qml_module(appOpenSnapBooth
    URI OpenSnapBooth
    VERSION 1.0
    QML_FILES Main.qml Settings.qml Gallery.qml PhotoViewer.qml DoubleSpinBox.qml CountDown.qml ShutterButton.qml Flash.qml CustomButton.qml ConfirmationDialog.qml
)

set_target_properties(appOpenSnapBooth PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appOpenSnapBooth
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${GSTREAMER_LIBRARIES}
    ${GST_PLG_BASE_LIBRARIES}
    ${GST_APP_LIBRARIES}
)

install(TARGETS appOpenSnapBooth
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
