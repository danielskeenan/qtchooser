set(TARGET qtchooser)

qt_add_executable(${TARGET}
        main.cpp
        ChooseRunner.cpp
        ChooseRunner.h
        ListRunner.cpp
        ListRunner.h
)

find_package(argparse CONFIG REQUIRED)
find_package(tabulate CONFIG REQUIRED)

target_link_libraries(${TARGET} PRIVATE
        qtchooserlib
        argparse::argparse
        Qt::Core
        tabulate::tabulate
)

set_target_properties(${TARGET} PROPERTIES
        QT_WINDOWS_APP_PROJECT_IDENTIFIER "org.dankeenan.${TARGET}"
)

install(TARGETS ${TARGET} RUNTIME)
qt_generate_deploy_app_script(
        TARGET ${TARGET}
        OUTPUT_SCRIPT DEPLOY_SCRIPT
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT "${DEPLOY_SCRIPT}")

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
