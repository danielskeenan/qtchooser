set(TARGET qtchooserlib)

qt_add_library(${TARGET} STATIC
        EnvironmentInjector.cpp
        EnvironmentInjector.h
        QtChooser.cpp
        QtChooser.h
        QtFinder.cpp
        QtFinder.h
        QtInfo.cpp
        QtInfo.h
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(${TARGET} PRIVATE
            EnvironmentInjector_win.cpp
            QtFinder_win.cpp
    )
else ()
    target_sources(${TARGET} PRIVATE
            EnvironmentInjector_none.cpp
            QtFinder_none.cpp
    )
endif ()

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        Qt::Concurrent
        spdlog::spdlog
)
