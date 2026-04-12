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
    find_path(WINREG_INCLUDE_DIRS "winreg/WinReg.hpp")
    target_include_directories(${TARGET} PRIVATE ${WINREG_INCLUDE_DIRS})
    target_sources(${TARGET} PRIVATE
            EnvironmentInjectorWin.cpp
            EnvironmentInjectorWin.h
            QtFinder_win.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(${TARGET} PRIVATE
            EnvironmentInjectorLinux.cpp
            EnvironmentInjectorLinux.h
            QtFinder_linux.cpp
    )
else ()
    target_sources(${TARGET} PRIVATE
            QtFinder_none.cpp
    )
endif ()

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        Qt::Concurrent
        spdlog::spdlog
)
