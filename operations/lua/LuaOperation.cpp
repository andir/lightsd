//
// Created by andi on 3/14/17.
//

#include "LuaOperation.h"
#include "../../utils/luna.h"
#include "LuaHSVBuffer.h"


LuaOperation::LuaOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("lua_" + getValueByKey<std::string>("name", begin, end, "unknown_script"), store, begin, end),
        luaState(openLua())
{

    const auto fn = getValueByKey<std::string>("filename", begin, end, "");

    if (fn == "") {
        std::cerr << "No lua file configure for plugin" << this->getName() << std::endl;
    } else {
        luaL_dofile(luaState, fn.c_str());
    }

}

LuaOperation::~LuaOperation() {
    if (this->luaState != nullptr) {
        lua_close(this->luaState);
        this->luaState = nullptr;
    }
}

lua_State* LuaOperation::openLua() {
    auto L = lua_open();

    luaJIT_setmode(L, -1, LUAJIT_MODE_ALLFUNC|LUAJIT_MODE_ON);


    // open some standard libs/operations
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_os(L);
    luaopen_string(L);
    luaopen_math(L);

    // register our hsv buffer type
    Luna<LuaHSVBuffer>::Register(L);


    return L;
}

void LuaOperation::run(const AbstractBaseBuffer<HSV>& buffer) {
    if (!isEnabled())
        return;
    lua_pushnumber(luaState, buffer.size());
    lua_setglobal(luaState, "size");

    lua_getglobal(luaState, "render");
    lua_pushlightuserdata(luaState, (void*)&buffer);
    if (lua_pcall(luaState, 1, 0, 0) != 0) {
        std::cerr << "LUA error: " << lua_tostring(luaState, -1) << std::endl;
    }
}