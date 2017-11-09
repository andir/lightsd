//
// Created by andi on 3/14/17.
//

#include "LuaHSVBuffer.h"
#include "LuaOperation.h"
#include "../../utils/luna.h"
#include "../../VariableStore/ValueType.h"

LuaOperation::LuaOperation(const std::string& name, std::shared_ptr<VariableStore>& store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation(name, store, begin, end),
        luaState(openLua()) {

    const auto mqtt = getValueByKey<bool>("mqtt", begin, end, false);

    if (!mqtt) {
	    const auto fn = getValueByKey<std::string>("filename", begin, end, "");

	    if (fn == "") {
	        std::cerr << "No lua file configure for plugin" << this->getName() << std::endl;
	    } else {
	        luaL_dofile(luaState, fn.c_str());
	    }
    } else {
	mqtt_code = std::make_unique<BoundConcreteValue<std::string> >(name + "/code",
		Operation::STRING, store, getValueByKey<std::string>("code", begin, end, ""),
		[this](ValueType* vt){
			this->reloadCode(vt->getString());
		}
	);
        luaL_dostring(luaState, mqtt_code->getValue().c_str());
    }
}

LuaOperation::~LuaOperation() {
    std::scoped_lock lock(code_mutex);
    destroy();
}

void LuaOperation::destroy() {
   if (luaState != nullptr)
       lua_close(luaState);
}

void LuaOperation::reloadCode(const std::string code) {
   std::scoped_lock lock(code_mutex);
   destroy();
   luaState = openLua();
   luaL_dostring(luaState, code.c_str());
}

lua_State *LuaOperation::openLua() {
    auto L = lua_open();

    luaJIT_setmode(L, -1, LUAJIT_MODE_ALLFUNC | LUAJIT_MODE_ON);


    // open some standard libs/operations
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_os(L);
    luaopen_bit(L);
    luaopen_string(L);
    luaopen_math(L);

    // register our hsv buffer type
    Luna<LuaHSVBuffer>::Register(L);


    return L;
}

void LuaOperation::run(const AbstractBaseBuffer<HSV> &buffer) {
    if (!luaState) return;

    std::scoped_lock lock(code_mutex);
    lua_pushnumber(luaState, buffer.size());
    lua_setglobal(luaState, "size");

    lua_getglobal(luaState, "render");
    lua_pushlightuserdata(luaState, (void *) &buffer);
    if (lua_pcall(luaState, 1, 0, 0) != 0) {
        std::cerr << "LUA error: " << lua_tostring(luaState, -1) << std::endl;
    }
}
