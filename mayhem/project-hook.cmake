# Passed to upstream's configure as -DCMAKE_PROJECT_fooyin_INCLUDE=... by mayhem/build.sh.
# It runs right after upstream's project(fooyin) call and defers mayhem/fuzzers.cmake to the end of the
# top-level directory, when every upstream target (Fooyin::Core, ...) exists. No upstream file is edited.
if(FOOYIN_FUZZERS)
    enable_language(C) # for the standalone driver (a C file)
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL include "${CMAKE_SOURCE_DIR}/mayhem/fuzzers.cmake")
endif()
