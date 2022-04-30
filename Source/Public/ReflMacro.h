#pragma once


#ifdef __REFL_GENERATOR__
#define RMETADATA(...) [[clang::annotate("Refl" __VA_OPT__(", ") #__VA_ARGS__)]] //__attribute__((annotate("Metadata" __VA_OPT__(",") #__VA_ARGS__)))
#define RCLASS(...)    RMETADATA(__VA_ARGS__)
#define RSTRUCT(...)   RMETADATA(__VA_ARGS__)
#define RENUM(...)     RMETADATA(__VA_ARGS__)
#define RPROPERTY(...) RMETADATA(__VA_ARGS__)
#define RFUNCTION(...) RMETADATA(__VA_ARGS__)
#else
#define RCLASS(...)
#define RENUM(...)
#define RPROPERTY(...)
#define RFUNCTION(...)
#define RMETADATA(...)
#endif

#define GENERATED_BODY()                      \
private:                                      \
template<class T> friend struct TStaticClass; \
public:                                       \
static RClass* StaticClass();                 \

