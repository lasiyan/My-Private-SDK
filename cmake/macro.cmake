macro(copy)
    configure_file(${ARGV0} ${__include_path} COPYONLY)
endmacro()

# argv0 : file & path
# argv1 : Is root file?
macro(setup)

    # header
    if (${ARGV0} MATCHES "\\.h(pp)?|\\.inl$")
        # get tag folder
        if (${ARGV1})
            configure_file(${ARGV0} ${__include_path} COPYONLY)
        else()
            get_filename_component(dir ${ARGV0} DIRECTORY) # path/src
            get_filename_component(tag ${dir} DIRECTORY) # path/{tag}
            get_filename_component(tag ${tag} NAME) # {tag}
            configure_file(${ARGV0} ${__include_path}/${tag} COPYONLY)
        endif()

    # source
    elseif(${ARGV0} MATCHES "\\.c(pp)?$")
        list(APPEND rowen_sdk_source ${ARGV0})

    # not defined    
    else()
        message("Invalid file extension : ${ARGV0}")
    endif()

endmacro()


macro(insert)
    get_filename_component(info ${ARGV0} LAST_EXT)

    if(info STREQUAL ".c" OR info STREQUAL ".cpp")
        list(APPEND rowen_sdk_source ${ARGV0})
    else()
        list(APPEND rowen_sdk_include ${ARGV0})
    endif()
endmacro(insert)
