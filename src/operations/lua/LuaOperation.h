#pragma once

#include "../Operation.h"

#include <luajit-2.0/lua.hpp>

class LuaOperation : public Operation {

    lua_State *luaState;

    lua_State *openLua();

    void run(const AbstractBaseBuffer<HSV> &buffer);

public:
    LuaOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~LuaOperation();

    BufferType operator()(BufferType &buffer) {
        run(*buffer);
        return buffer; // FIXME: 
    }
};
