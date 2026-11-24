# libFuzzer harnesses + standalone reproducers for fooyin (included by mayhem/project-hook.cmake).
# The whole tree is compiled with $SANITIZER_FLAGS -fsanitize=fuzzer-no-link (CMAKE_*_FLAGS from
# build.sh); here each harness is linked twice: with -fsanitize=fuzzer and with the standalone driver.
set(FY_MAYHEM_DIR "${CMAKE_SOURCE_DIR}/mayhem")
set(FY_HARNESS_DIR "${FY_MAYHEM_DIR}/harnesses")

add_library(fy_standalone_main OBJECT "$ENV{STANDALONE_FUZZ_MAIN}")
set_source_files_properties("$ENV{STANDALONE_FUZZ_MAIN}" PROPERTIES LANGUAGE C)
add_library(fy_lsan_off OBJECT "${FOOYIN_LSAN_OFF}")

function(fooyin_add_fuzzer name)
    cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDES;LIBS" ${ARGN})
    foreach(variant fuzz standalone)
        if(variant STREQUAL "fuzz")
            set(tgt ${name})
        else()
            set(tgt ${name}-standalone)
        endif()
        add_executable(${tgt} "${FY_HARNESS_DIR}/${name}.cpp" ${ARG_SOURCES} $<TARGET_OBJECTS:fy_lsan_off>)
        target_include_directories(${tgt} PRIVATE "${FY_HARNESS_DIR}" ${ARG_INCLUDES})
        target_include_directories(${tgt} SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIRS})
        target_link_libraries(${tgt} PRIVATE Fooyin::Core Fooyin::CorePrivate Fooyin::Utils Taglib::Taglib
                                             ${FFMPEG_LIBRARIES} ${ARG_LIBS})
        if(variant STREQUAL "fuzz")
            target_link_options(${tgt} PRIVATE -fsanitize=fuzzer)
        else()
            target_sources(${tgt} PRIVATE $<TARGET_OBJECTS:fy_standalone_main>)
        endif()
        set_target_properties(${tgt} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FOOYIN_FUZZ_OUT}")
        set_property(GLOBAL APPEND PROPERTY FOOYIN_FUZZ_TARGETS ${tgt})
    endforeach()
endfunction()

fooyin_add_fuzzer(fuzz_cue)
fooyin_add_fuzzer(fuzz_m3u)
fooyin_add_fuzzer(fuzz_pls)
fooyin_add_fuzzer(fuzz_taglib)
fooyin_add_fuzzer(fuzz_ffmpeg)
fooyin_add_fuzzer(fuzz_script)
fooyin_add_fuzzer(fuzz_accuraterip)
fooyin_add_fuzzer(fuzz_id3)
fooyin_add_fuzzer(fuzz_lyrics
    SOURCES "${CMAKE_SOURCE_DIR}/src/plugins/lyrics/lyricsparser.cpp"
    INCLUDES "${CMAKE_SOURCE_DIR}/src/plugins/lyrics" "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}/src/gui"
    LIBS Qt6::Widgets)
# lyrics.h pulls in gui/guistyleprovider.h for declarations only; the parser uses no fooyin_gui symbol.
target_compile_definitions(fuzz_lyrics PRIVATE FYGUI_STATIC_DEFINE)
target_compile_definitions(fuzz_lyrics-standalone PRIVATE FYGUI_STATIC_DEFINE)
find_package(SndFile REQUIRED)
fooyin_add_fuzzer(fuzz_sndfile
    SOURCES "${CMAKE_SOURCE_DIR}/src/plugins/sndfile/sndfileinput.cpp"
    INCLUDES "${CMAKE_SOURCE_DIR}/src/plugins/sndfile"
    LIBS SndFile::sndfile)

get_property(_fy_targets GLOBAL PROPERTY FOOYIN_FUZZ_TARGETS)
add_custom_target(fooyin_fuzzers DEPENDS ${_fy_targets})
