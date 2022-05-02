






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

macro(add_refl_generator_target CppTarget ReflHeaderFiles GeneratedFiles OutputDirectory ExportName)
    set(ReflGeneratorCommandArgs -p=${CMAKE_BINARY_DIR} ${ReflHeaderFiles} --export_name=${ExportName} --output=${OutputDirectory})
    get_target_property(TargetType ${CppTarget} TYPE)
#    if (NOT TargetType STREQUAL "EXECUTABLE")
#        set(ReflGeneratorCommandArgs "--extra-arg-before=--driver-mode=cl ${ReflGeneratorCommandArgs}")
#    endif ()

    add_custom_target(${CppTarget}-ReflGenerator
        ALL
        COMMAND ReflGenerator ${ReflGeneratorCommandArgs}
        DEPENDS ${ReflHeaderFiles}
        BYPRODUCTS ${GeneratedFiles}
        COMMENT "working for refl generated ..."
        )
    message("ReflGenerator ${ReflGeneratorCommandArgs}")
    add_dependencies(${CppTarget} ${CppTarget}-ReflGenerator)
endmacro()
