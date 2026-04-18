set(TARGET qtchooser)

add_executable(${TARGET}
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
        tabulate::tabulate
)

install(TARGETS ${TARGET} RUNTIME)
