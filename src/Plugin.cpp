///
/// @author Amrit Bhogal on 03/06/2023
/// @brief 
/// @version 1.0.0
///

#include <string>
#include <ranges>
#include <unordered_map>

#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>

#include "Log.hpp"
#include "Modules/Module.hpp"
#include "Modules/Lua54Module.hpp"

namespace LuaClang
{
    class ASTConsumer : public clang::ASTConsumer {
    public:
        std::shared_ptr<clang::Rewriter> _rewriter;
        clang::CompilerInstance *_compiler;

        ASTConsumer(clang::CompilerInstance *comp, const std::shared_ptr<clang::Rewriter> &rewriter, std::shared_ptr<Module> module)
            : _rewriter(rewriter), _compiler(comp)
        {
            _compiler->getPreprocessor().AddPragmaHandler(module->pragma_prefix(), module.get());
        }
    };

    /// @brief Entry point for the plugin.
    class Plugin : public clang::PluginASTAction {
    public:

        struct Options {
            enum class Version {
                Lua54,
                Lua53,
                Lua52,
                Lua51
            } version;
        } options;

    private:
        std::string _filename;
        clang::FileID _file_id;
        clang::CompilerInstance *_compiler;

        // `Plugin` is moved/destructed somewhere whilst the plugin is running, I do not know why.
        std::shared_ptr<clang::Rewriter> _rewriter;
        std::shared_ptr<Module> _module;

    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &comp, llvm::StringRef fname) override
        {
            _compiler = const_cast<clang::CompilerInstance *>(&comp);
            _filename = fname.str();
            _file_id = comp.getSourceManager().getMainFileID();
            _rewriter = std::make_shared<clang::Rewriter>(comp.getSourceManager(), comp.getLangOpts());
            _rewriter->setSourceMgr(comp.getSourceManager(), comp.getLangOpts());

            return std::make_unique<ASTConsumer>(_compiler, _rewriter, _module);
        }

        bool ParseArgs(const clang::CompilerInstance &compiler, const std::vector<std::string> &args) override
        {
            debug("luaclang: parsing arguments");
//            std::unordered_map<std::string, std::function<bool(const std::vector<std::string> &args)>> opts = {
//                {
//                    "version", [&](const std::vector<std::string> &args) -> bool {
//                        if (args.empty()) {
//                            error("version option requires an argument");
//                            return false;
//                        }
//
//                        const std::string &ver = args[0];
//                        if (ver == "5.1")       options.version = Options::Version::Lua51;
//                        else if (ver == "5.2")  options.version = Options::Version::Lua52;
//                        else if (ver == "5.3")  options.version = Options::Version::Lua53;
//                        else if (ver == "5.4")  options.version = Options::Version::Lua54;
//                        else {
//                            error("invalid version option: ", ver);
//                            return false;
//                        }
//
//                        info("luaclang: using Lua ", ver);
//
//                        _module = std::make_shared<Lua54Module>(); //TODO: Fill the rest of the if statements
//                        return true;
//                    }
//                }
//            };

            _module = std::make_shared<Lua54Module>();
            options.version = Options::Version::Lua54;

            debug("luaclang: finished parsing arguments");
            return true;
        }

        clang::PluginASTAction::ActionType getActionType() override
        { return clang::PluginASTAction::ReplaceAction; }
    };
}

[[gnu::used]]
static clang::FrontendPluginRegistry::Add<LuaClang::Plugin> X
("luaclang", "Plugin for the automatic generation of Lua bindings for C/C++ code");
