cmake_minimum_required(VERSION 3.16)

project(OpenSnapBooth VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appOpenSnapBooth
    main.cpp
)

qt_add_qml_module(appOpenSnapBooth
    URI OpenSnapBooth
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(appOpenSnapBooth PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appOpenSnapBooth
    PRIVATE Qt6::Quick
)

install(TARGETS appOpenSnapBooth
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
