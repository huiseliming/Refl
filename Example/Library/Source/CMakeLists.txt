
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

add_library(ExampleLibrary SHARED ${HeaderFiles} ${SourceFiles} ${GeneratedFiles})

target_link_libraries(ExampleLibrary PUBLIC Refl)


get_target_property(TargetType ExampleLibrary TYPE)
if (NOT TargetType STREQUAL "EXECUTABLE")
    target_include_directories(ExampleLibrary PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:Public>
    )
    include(GenerateExportHeader)
    generate_export_header(ExampleLibrary
        EXPORT_MACRO_NAME EXAMPLE_LIBRARY_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Public/ExampleLibraryExport.h
    )
endif ()

add_refl_generator_target(ExampleLibrary "${ReflHeaderFiles}" "${GeneratedFiles}" "${CMAKE_CURRENT_SOURCE_DIR}/Generated")
