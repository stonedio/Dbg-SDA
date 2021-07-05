# for making visual studio filters that repeat the physical structure of the project
function(create_vs_filters SRC)
    if(NOT DEFINED REMOVED_FILTER_PREFIX_PATH)
        set(REMOVED_FILTER_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    foreach(_source IN ITEMS ${SRC})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "${REMOVED_FILTER_PREFIX_PATH}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endfunction()