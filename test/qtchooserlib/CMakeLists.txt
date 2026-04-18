set(TARGET qtchooserlib_test)

# Get info about this Qt to use when testing the chooser.
get_target_property(QTPATHS_PROG Qt::qtpaths LOCATION)
if (NOT QTPATHS_PROG)
    message(FATAL_ERROR "Could not find usable qtpaths.")
endif ()
execute_process(
        COMMAND "${QTPATHS_PROG}"
        --query --query-format json
        OUTPUT_VARIABLE QT_QUERY_JSON
        COMMAND_ERROR_IS_FATAL ANY
)
string(JSON QT_INSTALL_PREFIX GET "${QT_QUERY_JSON}" "QT_INSTALL_PREFIX")
string(JSON QT_BIN_DIR GET "${QT_QUERY_JSON}" "QT_HOST_BINS")
string(JSON QT_LIBEXEC_DIR GET "${QT_QUERY_JSON}" "QT_HOST_LIBEXECS")
configure_file(qtchooser_testinfo.h.in ${PROJECT_BINARY_DIR}/qtchooser/qtchooser_testinfo.h)

qt_add_executable(${TARGET}
        main.cpp
        QtInfoTest.cpp
#        QtFinderTest.cpp
)

find_package(Catch2 3 REQUIRED)
target_link_libraries(${TARGET} PRIVATE
        qtchooserlib
        Qt::Core
        Qt::Test
        Catch2::Catch2
)

# Because parts of the test depend on Qt, need to "deploy" it before it will run.
# Otherwise, the build will fail because catch_discover_tests tries to execute the test program POST_BUILD.
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET ${TARGET}
            OUTPUT_SCRIPT TEST_DEPLOY_SCRIPT_FILE
            NO_TRANSLATIONS
            NO_COMPILER_RUNTIME
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=$<TARGET_FILE_DIR:${TARGET}>" -DQT_DEPLOY_BIN_DIR=.
            -DQT_DEPLOY_LIB_DIR=. -P "${TEST_DEPLOY_SCRIPT_FILE}"
            VERBATIM
    )
endif ()

include(Catch)
catch_discover_tests(${TARGET})
