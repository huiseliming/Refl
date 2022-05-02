
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

add_executable(Calculator ${HeaderFiles} ${SourceFiles} ${GeneratedFiles})

target_link_libraries(Calculator PUBLIC Refl)
target_link_libraries(Calculator PUBLIC Tool)

get_target_property(TargetType Calculator TYPE)

target_include_directories(Calculator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    $<INSTALL_INTERFACE:Public>
)

target_include_directories(Calculator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Generated
)

if (NOT TargetType STREQUAL "EXECUTABLE")
    include(GenerateExportHeader)
    generate_export_header(Calculator
        EXPORT_MACRO_NAME CALCULATOR_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Public/CalculatorExport.h
    )
endif ()

add_refl_generator_target(Calculator "${ReflHeaderFiles}" "${GeneratedFiles}" "${CMAKE_CURRENT_SOURCE_DIR}/Generated" "")

