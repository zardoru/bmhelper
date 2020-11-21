cmake_minimum_required(VERSION 3.17)
project(bmhelper)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(wxWidgets REQUIRED COMPONENTS core base xml)
find_package(Libsndfile REQUIRED)

include_directories(src)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})

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
        src/srcview.h src/AudioSplitter.cpp src/AudioSplitter.h src/wavsplit.cpp src/wavsplit.h)

include(${wxWidgets_USE_FILE})
target_link_libraries(bmhelper ${wxWidgets_LIBRARIES} ${LIBSNDFILE_LIBRARIES})

install(TARGETS bmhelper
          DESTINATION .
        RUNTIME
          DESTINATION .)

install (DIRECTORY i18n
         DESTINATION .
         FILES_MATCHING PATTERN "*.mo")

#install(CODE [[
#    file(GET_RUNTIME_DEPENDENCIES
#        EXECUTABLES
#            bmhelper)]])