#pragma once

#include <luajit-2.1/lua.hpp>
#include <memory>
#include <mutex>
#include "../../VariableStore/BoundConcreteValueType.h"
#include "../Operation.h"

class LuaOperation : public Operation {
  std::mutex code_mutex;

  std::unique_ptr<BoundConcreteValue<std::string>> mqtt_code;

  lua_State* luaState;

  lua_State* openLua();

  void destroy();
  void reloadCode(const std::string code);

  void run(const AbstractBaseBuffer<HSV>& buffer);

 public:
  LuaOperation(const std::string& name,
               std::shared_ptr<VariableStore>& store,
               YAML::const_iterator begin,
               YAML::const_iterator end);

  virtual ~LuaOperation();

  BufferType operator()(BufferType& buffer) {
    run(*buffer);
    return buffer;  // FIXME:
  }
};
