set(CPACK_PACKAGE_NAME "${PROJECT_DISPLAY_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR} <dk@dankeenan.org>")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_MONOLITHIC_INSTALL On)
set(CPACK_STRIP_FILES On)

# Build the license files as HTML, RTF, and plain text
find_program(PANDOC_PROG NAMES pandoc)
if (PANDOC_PROG)
    set(LICENSE_SOURCE_FILE "${PROJECT_SOURCE_DIR}/LICENSE.md")
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.html"
            COMMAND ${PANDOC_PROG}
            "--embed-resources"
            "--standalone"
            "--from" "commonmark"
            "--to" "html"
            "--eol" "lf"
            "--output" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.html"
            "${LICENSE_SOURCE_FILE}"
            DEPENDS "${LICENSE_SOURCE_FILE}"
    )
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf"
            COMMAND ${PANDOC_PROG}
            "--standalone"
            "--embed-resources"
            "--standalone"
            "--from" "commonmark+gfm_auto_identifiers"
            "--to" "rtf"
            "--eol" "lf"
            "--output" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf"
            "${LICENSE_SOURCE_FILE}"
            DEPENDS "${LICENSE_SOURCE_FILE}"
    )
    add_custom_target(license ALL DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.html"
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf"
    )
    set(README_SOURCE_FILE "${PROJECT_SOURCE_DIR}/README.md")
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/README.html"
            COMMAND ${PANDOC_PROG}
            "--embed-resources"
            "--standalone"
            "--from" "commonmark+definition_lists+gfm_auto_identifiers"
            "--to" "html"
            "--eol" "lf"
            "--output" "${CMAKE_CURRENT_SOURCE_DIR}/README.html"
            "${README_SOURCE_FILE}"
            DEPENDS "${README_SOURCE_FILE}"
    )
    add_custom_target(readme ALL DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/README.html")
endif ()

install(FILES
        LICENSE.html
        README.html
        TYPE DOC
)


set(PLATFORM_INSTALLER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_SYSTEM_NAME}")
if (EXISTS "${PLATFORM_INSTALLER_DIR}/CMakeLists.txt")
    include("${PLATFORM_INSTALLER_DIR}/CMakeLists.txt")
endif ()
include(CPack)
