set(LLVM_LINK_COMPONENTS support)

add_clang_executable(ReflGenerator
  Main.cpp
  ReflMatchFinder.h
  )

target_link_libraries(ReflGenerator
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )
 

target_compile_definitions(ReflGenerator PRIVATE REFL_GENERATOR)
#target_compile_definitions(ReflGenerator PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

#_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
