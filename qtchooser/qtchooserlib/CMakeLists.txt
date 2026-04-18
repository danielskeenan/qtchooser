set(TARGET qtchooserlib)

add_library(${TARGET} STATIC
#        CurrentChosen.cpp
#        CurrentChosen.h
#        EnvironmentInjector.cpp
#        EnvironmentInjector.h
        FindFile.cpp
        FindFile.h
#        QtChooser.cpp
#        QtChooser.h
        QtFinder.cpp
        QtFinder.h
        QtInfo.cpp
        QtInfo.h
        VersionNumber.cpp
        VersionNumber.h
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_path(WINREG_INCLUDE_DIRS "winreg/WinReg.hpp")
    target_include_directories(${TARGET} PRIVATE ${WINREG_INCLUDE_DIRS})
    target_sources(${TARGET} PRIVATE
#            EnvironmentInjectorWin.cpp
#            EnvironmentInjectorWin.h
            QtFinder_win.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(${TARGET} PRIVATE
#            EnvironmentInjectorLinux.cpp
#            EnvironmentInjectorLinux.h
            QtFinder_linux.cpp
    )
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

find_package(Boost CONFIG REQUIRED COMPONENTS algorithm process)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Boost::algorithm
        Boost::process
        spdlog::spdlog
)
target_compile_definitions(${TARGET} PRIVATE BOOST_PROCESS_USE_STD_FS)
