set(TARGET qtchooserlib)

qt_add_library(${TARGET} STATIC
        CurrentChosen.cpp
        CurrentChosen.h
        EnvironmentInjector.cpp
        EnvironmentInjector.h
        FindFile.cpp
        FindFile.h
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
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(${TARGET} PRIVATE
            EnvironmentInjector_linux.cpp
            QtFinder_linux.cpp
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
