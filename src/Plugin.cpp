//==============================================================================
// FILE:
//    HelloWorld.cpp
//
// DESCRIPTION:
//    Counts the number of C++ record declarations in the input translation
//    unit. The results are printed on a file-by-file basis (i.e. for each
//    included header file separately).
//
//    Internally, this implementation leverages llvm::StringMap to map file
//    names to the corresponding #count of declarations.
//
// USAGE:
//   clang -cc1 -load <BUILD_DIR>/lib/libHelloWorld.dylib '\'
//    -plugin hello-world test/HelloWorld-basic.cpp
//
// License: The Unlicense
//==============================================================================


#include <cstdio>
#include <memory>
#include <filesystem>
#include <fstream>

#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include "Function.hpp"
#include "Module.hpp"
#include "Compile.hpp"



auto &out = llvm::outs();

namespace LuaClang {
    class ASTConsumer : public clang::ASTConsumer {
    public:
        ASTConsumer(CompilerInstance &comp, Rewriter &rw, Module &module)
            : _module_handler(module),
              _function_consumer(comp, rw, module)
        {
            comp.getPreprocessor().AddPragmaHandler("lua", &_module_handler);
        }

        bool HandleTopLevelDecl(DeclGroupRef decls) override {
            //This doesn't sit right, but it works for now
            //TODO: alternate solution
            return _function_consumer.HandleTopLevelDecl(decls);
        }

    private:
        ModulePragmaHandler _module_handler;
        FunctionASTConsumer _function_consumer;
    };

    class Plugin : public PluginASTAction {
    private:
        static Rewriter _rewriter;
        static Module _module;
        std::string _file_name;
        clang::CompilerInstance *_compiler;

    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(CompilerInstance &comp, llvm::StringRef fname) override
        {
            _compiler = &comp;
            _rewriter.setSourceMgr(comp.getSourceManager(), comp.getLangOpts());
            _file_name = fname.str();
            return std::make_unique<LuaClang::ASTConsumer>(comp, _rewriter, _module);
        }

        bool ParseArgs(const CompilerInstance &, const std::vector<std::string> &) override
        { return true; }


        clang::PluginASTAction::ActionType getActionType() override
        { return clang::PluginASTAction::ReplaceAction; }

        void EndSourceFileAction() override
        {
            out << "EndSourceFileAction\n";
            auto &sm = _rewriter.getSourceMgr();
            auto fid = sm.getMainFileID();
            _rewriter.InsertTextAfter(sm.getLocForEndOfFile(fid), _module.to_code());
            clang::RewriteBuffer &buf = _rewriter.getEditBuffer(fid);
            compile(_compiler, _file_name, buf.begin(), buf.end());
        }
    };

    Rewriter Plugin::_rewriter;
    Module Plugin::_module;
}

static FrontendPluginRegistry::Add<LuaClang::Plugin> X("lua", "Plugin for automating the creation of lua modules");
