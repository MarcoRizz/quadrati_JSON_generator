cmake_minimum_required(VERSION 3.16)
project(Quadrati_JSON_generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---- Qt ----
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent LinguistTools)

set(TS_FILES Quadrati_JSON_generator_it_IT.ts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    generate_json.h generate_json.cpp
    nlohmann_json.cpp
    dizionario.h dizionario.cpp
    dynarray.h dynarray.cpp
    lettera.h lettera.cpp
    wordlist.h wordlist.cpp
    etichette.h
    custommenubutton.h custommenubutton.cpp
    common_enums.h
    ${TS_FILES}
)

qt_add_executable(Quadrati_JSON_generator
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    widget_displaydictionary.h widget_displaydictionary.cpp
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(Quadrati_JSON_generator PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent)

set_target_properties(Quadrati_JSON_generator PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE)

# iOS/macOS bundle id (facoltativo)
#if(${QT_VERSION} VERSION_LESS 6.1.0)
#    set_target_properties(Quadrati_JSON_generator
#        PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Quadrati_JSON_generator)
#endif()

install(TARGETS Quadrati_JSON_generator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Quadrati_JSON_generator)
endif()
