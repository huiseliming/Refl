#include <chrono>
#include <format>
#include <thread>
#include <filesystem>

#include "ReflMatchFinder.h"

// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>


#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace llvm;
using namespace clang::tooling;
using namespace clang;
using namespace clang::ast_matchers;

const char ReflGeneratorHelpMessage[] =
"\n"
"HuiSeLiMing:\n\n"
"  -p <BuildPath>\n"
"    set path to read compile_commands.json.\n"
"    设置构建路径读取 compile_commands.json.\n\n"
"  <Source0> [ ... <SourceN> ]\n"
"    generate reflect-source files from these source files\n"
"    反射源文件从这些源文件中生成\n\n"
;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory(CommonOptionsParser::HelpMessage);

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(ReflGeneratorHelpMessage);

void ReflWork()
{
    //ClangTool Tool(OptionsParser.getCompilations(), Sources);

}

std::string OutputDirectory;

int main(int argc, const char *argv[]) 
{
    if (0 == strncmp(argv[argc - 1], "--output=", 9)) {
        OutputDirectory = &argv[argc - 1][9];
        if (OutputDirectory.back() != '\\' && OutputDirectory.back() != '/')
        {
            OutputDirectory.append("/");
        }
        argc--;
    }
    std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();
    std::string CompileCommandsJsonPath;
    auto ExpectedOptionsParser = CommonOptionsParser::create(argc, argv, MyToolCategory, cl::NumOccurrencesFlag::OneOrMore);
    if (!ExpectedOptionsParser) {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedOptionsParser.takeError();
        return -1;
    }
    if (0 == strncmp(argv[1], "-p=", 3)) {
        CompileCommandsJsonPath = &argv[1][3];
    }
    else {
        CompileCommandsJsonPath = std::filesystem::current_path().string();
    }
    if (std::string::npos ==
        CompileCommandsJsonPath.find("compile_commands.json")) {
        if (CompileCommandsJsonPath.back() != '/' &&
            CompileCommandsJsonPath.back() != '\\') {
            CompileCommandsJsonPath.append("/");
        }
        CompileCommandsJsonPath.append("compile_commands.json");
    }
    if (!std::filesystem::exists(CompileCommandsJsonPath)) {
        llvm::errs() << "compile_commands.json file not exist.";
        return -1;
    }

    auto& OptionsParser = ExpectedOptionsParser.get();
    auto& CompilationDatabase =  OptionsParser.getCompilations();
    std::vector<CompileCommand> AllCompileCommands = CompilationDatabase.getAllCompileCommands();
    const std::vector<std::string> SourcePathList = OptionsParser.getSourcePathList();
#if 0
    llvm::outs() << " >>> AllCompileCommands >>> " << "\n";
    for (size_t i = 0; i < AllCompileCommands.size(); i++)
    {
        llvm::outs() << "CompileCommands : " << i << "\n";
        llvm::outs() << "  Directory : " << AllCompileCommands[i].Directory << "\n";
        llvm::outs() << "  Filename : " << AllCompileCommands[i].Filename << "\n";
        llvm::outs() << "  Heuristic : " << AllCompileCommands[i].Heuristic << "\n";
        llvm::outs() << "  Output : " << AllCompileCommands[i].Output << "\n";
        for (size_t j = 0; j < AllCompileCommands[i].CommandLine.size(); j++)
        {
            llvm::outs() << "    CommandLine : " << j << "  "<< AllCompileCommands[i].CommandLine[j] << "\n";
        }
    }
    llvm::outs() << " <<< AllCompileCommands <<< " << "\n";
    llvm::outs() << " >>> SourcePathList >>> " << "\n";
    for (size_t i = 0; i < SourcePathList.size(); i++)
    {
        llvm::outs() << "SourcePath : " << SourcePathList[i] << "\n";
        auto CompileCommands = CompilationDatabase.getCompileCommands(SourcePathList[i]);
        for (size_t i = 0; i < CompileCommands.size(); i++)
        {
            llvm::outs() << "CompileCommands : " << i << "\n";
            llvm::outs() << "  Directory : " << CompileCommands[i].Directory << "\n";
            llvm::outs() << "  Filename : " << CompileCommands[i].Filename << "\n";
            llvm::outs() << "  Heuristic : " << CompileCommands[i].Heuristic << "\n";
            llvm::outs() << "  Output : " << CompileCommands[i].Output << "\n";
            for (size_t j = 0; j < CompileCommands[i].CommandLine.size(); j++)
            {
                llvm::outs() << "    CommandLine : " << j << "  "<< CompileCommands[i].CommandLine[j] << "\n";
            }
        }
    }
    llvm::outs() << " <<< SourcePathList <<< " << "\n";
#endif
    ClangTool Tool(OptionsParser.getCompilations(), SourcePathList);
    clang::DiagnosticConsumer DiagnosticConsumer;
    Tool.setDiagnosticConsumer(&DiagnosticConsumer);
    MatchFinder Finder;
    ReflClassMatchFinder ClassMatchFinder;

    /* Search for all records (class/struct) with an 'annotate' attribute. */
    static DeclarationMatcher const ClassMatcher = cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
    Finder.addMatcher(ClassMatcher, &ClassMatchFinder);

    /* Search for all fields with an 'annotate' attribute. */
    static DeclarationMatcher const PropertyMatcher = fieldDecl(decl().bind("id"), hasAttr(attr::Annotate));
    Finder.addMatcher(PropertyMatcher, &ClassMatchFinder);

    /* Search for all functions with an 'annotate' attribute. */
    static DeclarationMatcher const FunctionMatcher = functionDecl(decl().bind("id"), hasAttr(attr::Annotate));
    Finder.addMatcher(FunctionMatcher, &ClassMatchFinder);
    Tool.appendArgumentsAdjuster([](const CommandLineArguments& CmdArg, StringRef Filename)
        -> CommandLineArguments
        {
            auto NewCmdArg = CmdArg;
            NewCmdArg.insert(++NewCmdArg.begin(), "-D__REFL_GENERATOR__");
            return NewCmdArg;
        });
    Tool.run(newFrontendActionFactory(&Finder).get());
    std::chrono::steady_clock::time_point End = std::chrono::steady_clock::now();
    llvm::outs() << std::format("Parsing reflect object in {:f} seconds\n", std::chrono::duration<double>(End - Start).count());
}




