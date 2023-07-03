///
/// @author Amrit Bhogal on 04/06/2023
/// @brief 
/// @version 1.0.0
///
#pragma once

#include "Modules/Module.hpp"

namespace LuaClang
{
    class Lua54Module : public Module {
    public:
        Lua54Module() : Module() {}

        std::string generate_code() override;
    };
}
