#pragma once


#include "../../utils/luna.h"
#include "../../hsv.h"
#include "../../Buffer.h"

class LuaHSVBuffer {

    const AbstractBaseBuffer<HSV>* buffer;

public:

    LuaHSVBuffer(lua_State *L) :  buffer(((AbstractBaseBuffer<HSV>*)(lua_touserdata(L, 1)))) {
    }

    int set(lua_State *L) {
        const auto c = lua_gettop(L);
        if (c != 5) {
            return luaL_error(L, "expecting exactly 4 arguments");
        }

        const size_t index = lua_tointeger(L, 2);
        const float hue = lua_tonumber(L, 3);
        const float saturation = lua_tonumber(L, 4);
        const float value = lua_tonumber(L, 5);

        if (index < buffer->size()) {
            auto& p = buffer->at(index);
            p.hue = hue;
            p.saturation = saturation;
            p.value = value;
        } else {
            // FIXME: add some logging here
        }
        return 0;
    }

    int size(lua_State *L) {
        lua_pushinteger(L, buffer->size());
        return 1;
    }

    static const char className[];
    static const Luna<LuaHSVBuffer>::RegType Register[];
};
