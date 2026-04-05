set(TARGET qtchooserlib)

qt_add_library(${TARGET} STATIC
        QtInfo.cpp
        QtInfo.h
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        Qt::Concurrent
        spdlog::spdlog
)
