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

namespace LuaClang
{
    class ASTConsumer : public clang::ASTConsumer {
    private:
        clang::Rewriter _rewriter;

    public:
        explicit ASTConsumer(clang::CompilerInstance *comp) : _rewriter(comp->getSourceManager(), comp->getLangOpts())
        {}
    };

    /// @brief Entry point for the plugin.
    class Plugin : public clang::PluginASTAction {
    public:
        struct Options {
            enum class Version {
                Lua51,
                Lua52,
                Lua53,
                Lua54
            } version = Version::Lua54;
        } options;

    private:
        std::string _filename;
        clang::CompilerInstance *_compiler;

    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &comp, llvm::StringRef fname) override
        {
            _compiler = const_cast<clang::CompilerInstance *>(&comp);
            _filename = fname.str();

            return std::make_unique<ASTConsumer>(_compiler);
        }

        bool ParseArgs(const clang::CompilerInstance &compiler, const std::vector<std::string> &args) override
        {
            std::unordered_map<std::string, std::function<bool(const std::vector<std::string> &args, Options &options)>> opts = {
                {
                    "version", [](const std::vector<std::string> &args, Options &options) -> bool {
                        if (args.empty()) {
                            error("version option requires an argument");
                            return false;
                        }

                        if (args[0] == "5.1") {
                            options.version = Options::Version::Lua51;
                        } else if (args[0] == "5.2") {
                            options.version = Options::Version::Lua52;
                        } else if (args[0] == "5.3") {
                            options.version = Options::Version::Lua53;
                        } else if (args[0] == "5.4") {
                            options.version = Options::Version::Lua54;
                        } else {
                            error("invalid version option: ", args[0]);
                            return false;
                        }

                        llvm::outs() << "luaclang: using Lua " << args[0] << "\n";

                        return true;
                    }
                }
            };

            return true;
        }

        clang::PluginASTAction::ActionType getActionType() override
        { return clang::PluginASTAction::ReplaceAction; }
    };
}

[[gnu::used]]
static clang::FrontendPluginRegistry::Add<LuaClang::Plugin> X("luaclang", "Plugin for the automatic generation of Lua bindings for C/C++ code");
