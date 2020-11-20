
include(FindPackageHandleStandardArgs)

find_path(
        LIBSNDFILE_INCLUDE_DIRS
            sndfile.h
        HINTS
            "C:/Program Files/Mega-Nerd/libsndfile/include")
find_library(LIBSNDFILE_LIBRARIES
             NAMES
                sndfile sndfile-1 libsndfile libsndfile-1
             HINTS
                "C:/Program Files/Mega-Nerd/libsndfile/lib")

find_package_handle_standard_args(
        Libsndfile
        DEFAULT_MSG
        LIBSNDFILE_LIBRARIES
        LIBSNDFILE_INCLUDE_DIRS
)
