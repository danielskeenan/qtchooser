cmake_minimum_required(VERSION 3.28)
project(qtchooser
        VERSION 1.0.0
        DESCRIPTION "Manage multiple Qt versions"
        HOMEPAGE_URL "https://github.com/danielskeenan/qtchooser"
)
set(PROJECT_DISPLAY_NAME "qtchooser")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")
string(TIMESTAMP PROJECT_BUILD_TIMESTAMP "%s" UTC)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" log -n1 --format=%H
        OUTPUT_VARIABLE PROJECT_COMMIT_SHA
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_SHA}" PROJECT_COMMIT_SHA)
execute_process(COMMAND "${GIT_EXECUTABLE}" tag --points-at=HEAD
        OUTPUT_VARIABLE PROJECT_COMMIT_TAG
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_TAG}" PROJECT_COMMIT_TAG)

option(BUILD_EXEC "Build the executable program.  You probably want to do this." ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (BUILD_EXEC)
    add_compile_definitions(QT_NO_KEYWORDS)

    include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

    # Decide which Qt version to use.
    set(QT_COMPONENTS
            Core
            Concurrent
            Test
    )
    find_package(Qt6 COMPONENTS ${QT_COMPONENTS} REQUIRED)

    qt_standard_project_setup()

    # Log levels
    add_compile_definitions("SPDLOG_ACTIVE_LEVEL=$<IF:$<CONFIG:Debug>,0,2>")

    add_subdirectory(qtchooser)

    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif ()

    option(BUILD_PACKAGE Off)
    if (BUILD_PACKAGE)
        add_subdirectory(install)
    endif ()
endif ()
