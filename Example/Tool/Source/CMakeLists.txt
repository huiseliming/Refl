
file(GLOB_RECURSE PublicHeaderFiles Public/*.h)
file(GLOB_RECURSE PrivateHeaderFiles Private/*.h)
file(GLOB_RECURSE PrivateSourceFiles Private/*.cpp)
set(HeaderFiles ${PublicHeaderFiles} ${PrivateHeaderFiles})
set(SourceFiles ${PrivateSourceFiles})

refl_generator_search_file(HeaderFiles "/* REFL_HEADER */")

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR} 
  PREFIX Source 
  FILES ${HeaderFiles} ${SourceFiles} ${GeneratedFiles}
)

add_library(Tool SHARED ${HeaderFiles} ${SourceFiles} ${GeneratedFiles})

target_link_libraries(Tool PUBLIC Refl)

target_include_directories(Tool PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    $<INSTALL_INTERFACE:Public>
)

target_include_directories(Tool PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Generated
)

get_target_property(TargetType Tool TYPE)
if (NOT TargetType STREQUAL "EXECUTABLE")
    include(GenerateExportHeader)
    generate_export_header(Tool
        EXPORT_MACRO_NAME TOOL_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Public/ToolExport.h
    )
endif ()

add_refl_generator_target(Tool "${ReflHeaderFiles}" "${GeneratedFiles}" "${CMAKE_CURRENT_SOURCE_DIR}/Generated" "TOOL_API")
