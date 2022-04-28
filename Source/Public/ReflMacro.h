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

#define REFL_CLASS_BEGIN(CppClass, MetadataArray)                      \
template<>                                                             \
struct TStaticClass<CppClass> {                                        \
    static RClass* Initializer()                                       \
    {                                                                  \
        static TReflClass<CppClass> Class(#CppClass);                  \
        Class.AddMetadata(MetadataArray.begin(), MetadataArray.end()); \



#define REFL_CLASS_END(CppClass)                                     \
        return &Class;                                               \
    }                                                                \
};                                                                   \
RClass* CppClass##::StaticClass()                                    \
{                                                                    \
    static RClass* ClassPtr = TStaticClass<CppClass>::Initializer(); \
    return ClassPtr;                                                 \
}                                                                    \

#define REFL_PROPERTY(CppClass, CppType, PropertyName, MetadataArray)                  \
{                                                                                      \
    auto Prop = NewProperty<CppType>(#PropertyName, offsetof(CppClass, PropertyName)); \
    Prop->AddMetadata(MetadataArray.begin(), MetadataArray.end());                     \
    Class.AddProperty(Prop);                                                           \
}                                                                                      \


//void f()
//
//{
//    std::array<std::pair<std::string, std::string>, 1> aa =
//    {
//        std::pair<std::string, std::string>{ "", "" }
//    };
//    std::map<std::string, std::string> asa;
//    asa.insert(aa.begin(), aa.end());
//}
//
