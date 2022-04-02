if (GGWORDS_ALL_WARNINGS)
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    else()
        # todo : windows
    endif()
endif()

#
## Main

set(TARGET analyze)

add_executable(${TARGET}
    analyze.cpp
    common.cpp
    )

target_include_directories(${TARGET} PUBLIC
    .
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}-extra/
    )

target_link_libraries(${TARGET} PRIVATE
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    )

make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}-extra/)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build_timestamp-tmpl.h   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}-extra/build_timestamp.h @ONLY)
