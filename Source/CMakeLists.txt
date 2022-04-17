

file(GLOB_RECURSE PublicFile Public/*.h Public/*.cpp)
file(GLOB_RECURSE PrivateFile Private/*.h Private/*.cpp)
file(GLOB_RECURSE GeneratedFile Generated/*)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR} 
  PREFIX Source 
  FILES ${PublicFile} ${PrivateFile} ${GeneratedFile}
)

add_library(Refl SHARED ${PublicFile} ${PrivateFile} ${GeneratedFile})

target_include_directories(Refl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    $<INSTALL_INTERFACE:Public>
)

include(GenerateExportHeader)
generate_export_header(Refl
    EXPORT_MACRO_NAME REFL_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Public/ReflExport.h
)

