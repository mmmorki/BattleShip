cmake_minimum_required(VERSION 3.31)
project(BattleShip)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Users/ivane/Devtools/C++/Qt/6.10.2/msvc2022_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Multimedia
        REQUIRED)

add_executable(BattleShip source/main.cpp
        source/MainWindow.cpp
        source/Server.cpp
        source/Field.cpp
        source/OpponentField.cpp
        source/PlayerField.cpp
        source/Cell.cpp
        source/Client.cpp
        source/ThemeMusicManager.cpp
        headers/Cell.h
        headers/Field.h
        headers/OpponentField.h
        headers/PlayerField.h
        headers/MainWindow.h
        headers/Server.h
        headers/Client.h
        headers/ThemeMusicManager.h
        resources.qrc)

target_include_directories(BattleShip PRIVATE
        headers)

target_link_libraries(BattleShip
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Multimedia
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Network Multimedia)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

if (EXISTS "${QT_INSTALL_PATH}/plugins/multimedia")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/multimedia/")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QT_INSTALL_PATH}/plugins/multimedia"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/multimedia/")
endif()

foreach(FFMPEG_LIB avcodec-61 avformat-61 avutil-59 swresample-5 swscale-8)
    if(EXISTS "${QT_INSTALL_PATH}/bin/${FFMPEG_LIB}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/${FFMPEG_LIB}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endif()
endforeach()