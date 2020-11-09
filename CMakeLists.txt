cmake_minimum_required(VERSION 3.17)
project(bmhelper)

set(CMAKE_CXX_STANDARD 17)

find_package(wxWidgets REQUIRED COMPONENTS core base xml)

include_directories(src)

add_executable(bmhelper WIN32
        src/app.cpp
        src/app.h
        src/common.h
        src/Configuration.cpp
        src/defview.cpp
        src/defview.h
        src/divedit.cpp
        src/division.cpp
        src/division.h
        src/divview.cpp
        src/divview.h
        src/frame.cpp
        src/frame.h
        src/gui.h
        src/mdview.cpp
        src/mdview.h
        src/midi_base.h
        src/midi_data.h
        src/midi_events.h
        src/midi_smfin.cpp
        src/midi_smfout.cpp
        src/midi_sr.cpp
        src/project.cpp
        src/project.h
        src/serialize.h
        src/smf_accs.cpp
        src/smf_data.cpp
        src/smf_event.cpp
        src/smf_io.h
        src/srcview.cpp
        src/srcview.h)

if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    # and for each of your dependent executable/library targets:
    target_link_libraries(bmhelper ${wxWidgets_LIBRARIES})
endif()