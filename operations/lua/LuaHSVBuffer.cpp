#include "LuaHSVBuffer.h"
#include "../../utils/luna.h"


const char LuaHSVBuffer::className[] = "HSVBuffer";

const Luna<LuaHSVBuffer>::RegType LuaHSVBuffer::Register[] {
        { "set", &LuaHSVBuffer::set },
        { "size", &LuaHSVBuffer::size },
        { 0 },
};