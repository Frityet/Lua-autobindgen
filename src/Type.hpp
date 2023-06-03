#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace LuaClang
{
    class Type {
    public:
        Type(std::string name, std::string cname) : name(name), cname(cname)
        {}

        std::string name, cname;

        virtual ~Type() = default;
    };

    static std::unordered_map<std::string, std::shared_ptr<Type>> registry;
}
