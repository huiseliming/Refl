






macro(refl_generator_search_file HeaderFiles ReflHeaderString)
    # if(${refl_header_string})
    #     set(ReflHeaderString ${refl_header_string})
    # else
    #     set(ReflHeaderString "/* REFL_HEADER */")
    # endif()
    unset(ReflHeaderFiles)
    unset(GeneratedHeaderFiles)
    unset(GeneratedSourceFiles)
    foreach(HeaderFile ${${HeaderFiles}})
        file(STRINGS ${HeaderFile} GeneratedFlag LENGTH_MAXIMUM 100 LIMIT_COUNT 1)
        if(GeneratedFlag AND "${GeneratedFlag}" STREQUAL ${ReflHeaderString})
            list(APPEND ReflHeaderFiles ${HeaderFile})
            string(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${HeaderFile})
            string(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${HeaderFile})
            set(GeneratedHeaderFile ${CMAKE_CURRENT_SOURCE_DIR}/Generated/${FILE_NAME}.generated.h)
            set(GeneratedSourceFile ${CMAKE_CURRENT_SOURCE_DIR}/Generated/${FILE_NAME}.generated.cpp)
            list(APPEND GeneratedHeaderFiles ${GeneratedHeaderFile})
            list(APPEND GeneratedSourceFiles ${GeneratedSourceFile})
            if(NOT EXISTS ${GeneratedHeaderFile})
                file(WRITE ${GeneratedHeaderFile} "#pragma once")
            endif()
        endif()
    endforeach()

    set(GeneratedFiles ${GeneratedHeaderFiles} ${GeneratedSourceFiles})
endmacro()

macro(add_refl_generator_target CppTarget ReflHeaderFiles GeneratedFiles)

    get_target_property(TargetType ${CppTarget} TYPE)
    if (NOT TargetType STREQUAL "EXECUTABLE")
        set(ReflGeneratorCompilerCommand "${ReflGeneratorCompilerCommand}--export_name ThisTarget")
    endif ()

    add_custom_target(${CppTarget}-ReflGenerator
        ALL
        COMMAND ReflGenerator 
        --database_dir ${CMAKE_BINARY_DIR} 
        --std c++20 
    #    --export_name ThisTarget 
        --macro_definition __REFL_GENERATED__
        ${ReflHeaderFiles}
        DEPENDS ${ReflHeaderFiles}
        BYPRODUCTS ${GeneratedFiles}
        COMMENT "working for refl generated ..."
        )

    add_dependencies(${CppTarget} ${CppTarget}-ReflGenerator)
endmacro()
