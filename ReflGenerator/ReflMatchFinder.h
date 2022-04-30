#pragma once

#include <fstream>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>


extern std::string OutputDirectory;

struct ClassMember
{
    std::vector<clang::FieldDecl const*> FieldDecls;
    std::vector<clang::FunctionDecl const*> FunctionDecls;
};

class ReflClassMatchFinder : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    void run(const clang::ast_matchers::MatchFinder::MatchResult& MatchResult) {
        ASTContext = MatchResult.Context;
        SourceManager = MatchResult.SourceManager;

        clang::Decl const* Decl = MatchResult.Nodes.getNodeAs<clang::Decl>("id");
        if (SourceManager->isInMainFile(Decl->getLocation()))
        {
            if (clang::CXXRecordDecl const* CXXRecordDecl = MatchResult.Nodes.getNodeAs<clang::CXXRecordDecl>("id"))
            {
                ClassMemberMap.insert(std::make_pair(CXXRecordDecl, ClassMember{}));
            }

            if (clang::FieldDecl const* FieldDecl = MatchResult.Nodes.getNodeAs<clang::FieldDecl>("id"))
            {
                clang::CXXRecordDecl const* Parent = cast<clang::CXXRecordDecl const>(FieldDecl->getParent());
                if (Parent && ClassMemberMap.contains(Parent))
                {
                    ClassMemberMap[Parent].FieldDecls.push_back(FieldDecl);
                }
            }

            if (clang::FunctionDecl const* FunctionDecl = MatchResult.Nodes.getNodeAs<clang::FunctionDecl>("id"))
            {
                clang::CXXRecordDecl const* Parent = cast<clang::CXXRecordDecl const>(FunctionDecl->getParent());
                if (Parent && ClassMemberMap.contains(Parent))
                {
                    ClassMemberMap[Parent].FunctionDecls.push_back(FunctionDecl);
                }
            }
        }
    }

    bool FindReflectAnnotation(const clang::Decl* CheckedDecl, std::map<std::string, std::string>& OutMetadata) {
        OutMetadata.clear();
        if (CheckedDecl->hasAttrs()) {
            for (auto Attr : CheckedDecl->getAttrs()) {

                if (Attr->getKind() == clang::attr::Annotate)
                {
                    std::string RowStringBuffer;
                    llvm::raw_string_ostream RowStringOutputStream(RowStringBuffer);
                    Attr->printPretty(RowStringOutputStream, clang::PrintingPolicy(clang::LangOptions()));
                    std::string AttrFullString(RowStringOutputStream.str());
                    constexpr static size_t ReflAnnotateStartSize = sizeof("[[clang::annotate(\"Refl") - 1;
                    constexpr static size_t ReflAnnotateEndSize = sizeof("\")]]") - 1;
                    size_t ReflAnnotateStartPos = AttrFullString.find("[[clang::annotate(\"Refl");
                    size_t ReflAnnotateEndPos = AttrFullString.rfind("\")]]");
                    if (ReflAnnotateStartPos != std::string::npos && 
                        ReflAnnotateEndPos != std::string::npos &&
                        ReflAnnotateStartPos <= 1 &&
                        (ReflAnnotateEndPos == (AttrFullString.size() - 4)))
                    {
                        std::string MetaString;
                        MetaString.reserve(AttrFullString.size() - ReflAnnotateStartSize - ReflAnnotateEndSize - ReflAnnotateStartPos);
                        std::string MetadataKey, MetadataValue;
                        size_t OffsetIndex = ReflAnnotateStartSize + ReflAnnotateStartPos;
                        size_t LastIndex = AttrFullString.size() - ReflAnnotateEndSize;
                        if (AttrFullString[OffsetIndex] == ',') OffsetIndex++;
                        while (OffsetIndex < LastIndex)
                        {
                            while (OffsetIndex < LastIndex && std::isspace(AttrFullString[OffsetIndex])) OffsetIndex++;
                            while (OffsetIndex < LastIndex && (
                                    std::isalpha(AttrFullString[OffsetIndex]) || 
                                    std::isdigit(AttrFullString[OffsetIndex]) ||
                                    AttrFullString[OffsetIndex] == '_'
                                ))
                            {
                                MetadataKey.push_back(AttrFullString[OffsetIndex]);
                                OffsetIndex++;
                            }
                            while (OffsetIndex < LastIndex && std::isspace(AttrFullString[OffsetIndex])) OffsetIndex++;
                            if (AttrFullString[OffsetIndex] == '=')
                            {
                                OffsetIndex++;
                                while (OffsetIndex < LastIndex && std::isspace(AttrFullString[OffsetIndex])) OffsetIndex++;
                                if (AttrFullString[OffsetIndex] == '\"')
                                {
                                    OffsetIndex++;
                                    while (OffsetIndex < LastIndex && AttrFullString[OffsetIndex] != '\"')
                                    {
                                        if (AttrFullString[OffsetIndex] == '\\')
                                        {
                                            if (AttrFullString[OffsetIndex] == '\\' || AttrFullString[OffsetIndex] == '\"')
                                            {
                                                MetadataValue.push_back(AttrFullString[OffsetIndex]);
                                                OffsetIndex++;
                                            }
                                            else
                                            {
                                                throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                                            }
                                        }
                                        MetadataValue.push_back(AttrFullString[OffsetIndex]);
                                        OffsetIndex++;
                                    }
                                    OffsetIndex++;
                                }
                                else 
                                {
                                    if (std::isdigit(AttrFullString[OffsetIndex]))
                                    {
                                        while (OffsetIndex < LastIndex && (
                                            std::isdigit(AttrFullString[OffsetIndex]) ||
                                            AttrFullString[OffsetIndex] != '.'
                                            ))
                                        {
                                            MetadataValue.push_back(AttrFullString[OffsetIndex]);
                                            OffsetIndex++;
                                        }
                                    }
                                    else
                                    {
                                        while (OffsetIndex < LastIndex && (
                                            std::isalpha(AttrFullString[OffsetIndex]) ||
                                            std::isdigit(AttrFullString[OffsetIndex]) ||
                                            AttrFullString[OffsetIndex] == '_'
                                            ))
                                        {
                                            MetadataValue.push_back(AttrFullString[OffsetIndex]);
                                            OffsetIndex++;
                                        }
                                    }
                                }
                                while (OffsetIndex < LastIndex && std::isspace(AttrFullString[OffsetIndex])) OffsetIndex++;
                                if (OutMetadata.contains(MetadataKey))
                                {
                                    throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                                }
                                OutMetadata.insert(std::make_pair(MetadataKey, MetadataValue));
                                if (AttrFullString[OffsetIndex] == ',')
                                {
                                    OffsetIndex++;
                                }
                                else if (OffsetIndex == LastIndex)
                                {

                                }
                                else
                                {
                                    throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                                }
                            }
                            else if (AttrFullString[OffsetIndex] == ',')
                            {
                                if (OutMetadata.contains(MetadataKey))
                                {
                                    throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                                }
                                OutMetadata.insert(std::make_pair(MetadataKey, MetadataValue));
                                OffsetIndex++;
                            }
                            else if (OffsetIndex == LastIndex)
                            {
                                if (OutMetadata.contains(MetadataKey))
                                {
                                    throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                                }
                                OutMetadata.insert(std::make_pair(MetadataKey, MetadataValue));
                            }
                            else
                            {
                                throw std::runtime_error(std::format("{:s} illegal metadata macro, correct format should be RMETADATA(A=B, C = \"D\")", Attr->getLocation().printToString(*SourceManager)));
                            }
                            MetadataKey.clear();
                            MetadataValue.clear();
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    virtual void onStartOfTranslationUnit() override
    {
    }

    virtual void onEndOfTranslationUnit() override
    {
        for (auto Pair : ClassMemberMap)
        {
            clang::CXXRecordDecl const* CXXRecordDecl = Pair.first;
            std::map<std::string, std::string> OutCXXRecordDeclMetadata;
            if (FindReflectAnnotation(CXXRecordDecl, OutCXXRecordDeclMetadata))
            {
                auto& Attrs = CXXRecordDecl->getAttrs();
                for (size_t i = 0; i < Attrs.size(); i++)
                {
                    Attrs[i]->getKind();
                }
                ClassMember& ClassMemberRef = Pair.second;
                std::string FullPathString = SourceManager->getFilename(CXXRecordDecl->getLocation()).str();
                std::filesystem::path FullPath(FullPathString);
                std::string Filename = FullPath.filename().string();

                std::string GeneratedHeader, GeneratedSource;
                size_t Pos = Filename.find_last_of('.');
                if (Pos != std::string::npos)
                {
                    GeneratedHeader = OutputDirectory + Filename.substr(0, Pos) + ".generated.h";
                    GeneratedSource = OutputDirectory + Filename.substr(0, Pos) + ".generated.cpp";
                }
                if (!GeneratedFileMap.contains(GeneratedHeader))
                {
                    GeneratedFileMap[GeneratedHeader].append("#pragma once\n");
                }
                if (!GeneratedFileMap.contains(GeneratedSource))
                {
                    GeneratedFileMap[GeneratedSource].append("#include\"" + FullPathString + "\"\n");
                }
                std::string MetadataArrayDefine;
                std::string ClassMetadataName = std::format("GEN_{}_Metadata", CXXRecordDecl->getNameAsString());
                MetadataArrayDefine += std::format("static std::array<std::pair<std::string, std::string>, {:d}> {:s} = {{\n", OutCXXRecordDeclMetadata.size(), ClassMetadataName);
                for (auto Metadata : OutCXXRecordDeclMetadata)
                {
                    MetadataArrayDefine += std::format("    std::pair{{\"{:s}\", \"{:s}\"}},\n", Metadata.first, Metadata.second);
                }
                MetadataArrayDefine.resize(MetadataArrayDefine.size() - 1);
                if (MetadataArrayDefine.back() == ',')
                {
                    MetadataArrayDefine.back() = '\n';
                }
                MetadataArrayDefine += std::format("}};\n");
                std::string ClassDefine;
                ClassDefine += std::format("template<>") + "\n";
                ClassDefine += std::format("struct TStaticClass<{:s}>", CXXRecordDecl->getNameAsString()) + "\n";
                ClassDefine += std::format("{{") + "\n";
                ClassDefine += std::format("    static RClass* Initializer()") + "\n";
                ClassDefine += std::format("    {{") + "\n";
                ClassDefine += std::format("        static TReflClass<{0:s}> Class(\"{0:s}\");", CXXRecordDecl->getNameAsString()) + "\n";
                ClassDefine += std::format("        Class.AddMetadata({0:s}.begin(), {0:s}.end());", ClassMetadataName) + "\n";
                for (size_t i = 0; i < ClassMemberRef.FieldDecls.size(); i++)
                {
                    clang::FieldDecl const* FieldDecl = ClassMemberRef.FieldDecls[i];
                    std::map<std::string, std::string> OutFieldDeclMetadata;
                    if (FindReflectAnnotation(FieldDecl, OutFieldDeclMetadata))
                    {
                        std::string FieldMetadataName = std::format("GEN_{}_{}_Metadata", CXXRecordDecl->getNameAsString(), FieldDecl->getNameAsString());
                        MetadataArrayDefine += std::format("static std::array<std::pair<std::string, std::string>, {:d}> {:s} = {{\n", OutFieldDeclMetadata.size(), FieldMetadataName);
                        for (auto Metadata : OutFieldDeclMetadata)
                        {
                            MetadataArrayDefine += std::format("    std::pair{{\"{:s}\", \"{:s}\"}},\n", Metadata.first, Metadata.second);
                        }
                        MetadataArrayDefine.resize(MetadataArrayDefine.size() - 1);
                        if (MetadataArrayDefine.back() == ',')
                        {
                            MetadataArrayDefine.back() = '\n';
                        }
                        MetadataArrayDefine += std::format("}};\n");
                        ClassDefine += std::format("        {{") + "\n";
                        ClassDefine += std::format("            auto Prop = NewProperty<{0:s}>(\"{1:s}\", offsetof({2:s}, {1:s}));", FieldDecl->getType().getAsString(), FieldDecl->getNameAsString(), CXXRecordDecl->getNameAsString()) + "\n";
                        ClassDefine += std::format("            Prop->AddMetadata({0:s}.begin(), {0:s}.end());", FieldMetadataName) + "\n";
                        ClassDefine += std::format("            Class.AddProperty(Prop);") + "\n";
                        ClassDefine += std::format("        }}") + "\n";
                    }
                }
                ClassDefine += std::format("        return &Class;") + "\n";
                ClassDefine += std::format("    }}") + "\n";
                ClassDefine += std::format("}};") + "\n";
                ClassDefine += std::format("RClass* {:s}::StaticClass()", CXXRecordDecl->getNameAsString()) + "\n";
                ClassDefine += std::format("{{") + "\n";
                ClassDefine += std::format("    static RClass* ClassPtr = TStaticClass<{:s}>::Initializer();", CXXRecordDecl->getNameAsString()) + "\n";
                ClassDefine += std::format("    return ClassPtr;") + "\n";
                ClassDefine += std::format("}}") + "\n";
                GeneratedFileMap[GeneratedSource].append(MetadataArrayDefine);
                GeneratedFileMap[GeneratedSource].append(ClassDefine);
            }
        }

        for (auto GeneratedFile : GeneratedFileMap)
        {
            std::fstream OutputGeneratedFileStream;
            OutputGeneratedFileStream.open(GeneratedFile.first, std::ios::out | std::ios::trunc);
            if (OutputGeneratedFileStream.is_open())
            {
                OutputGeneratedFileStream.write(GeneratedFile.second.data(), GeneratedFile.second.size());
                OutputGeneratedFileStream.close();
            }
        }
    }

    std::map<clang::CXXRecordDecl const*, ClassMember> ClassMemberMap;
    std::map<std::string, std::string> GeneratedFileMap;

    clang::CXXRecordDecl const* LastCXXRecordDecl;
    clang::ASTContext* ASTContext;
    clang::SourceManager* SourceManager;
};

