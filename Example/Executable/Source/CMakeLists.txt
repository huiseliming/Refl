
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

add_executable(ExampleExecutable ${HeaderFiles} ${SourceFiles} ${GeneratedFiles})

target_link_libraries(ExampleExecutable PUBLIC Refl)
target_link_libraries(ExampleExecutable PUBLIC ExampleLibrary)

get_target_property(TargetType ExampleExecutable TYPE)

target_include_directories(ExampleExecutable PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    $<INSTALL_INTERFACE:Public>
)

if (NOT TargetType STREQUAL "EXECUTABLE")
    include(GenerateExportHeader)
    generate_export_header(ExampleExecutable
        EXPORT_MACRO_NAME EXAMPLE_EXECUTABLE_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Public/ExampleExecutableExport.h
    )
endif ()

add_refl_generator_target(ExampleExecutable "${ReflHeaderFiles}" "${GeneratedFiles}" "${CMAKE_CURRENT_SOURCE_DIR}/Generated")

