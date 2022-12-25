#include "scripting/lua-bindings/manual/CCLuaBind.h"

namespace cocos2d
{
    namespace bind
    {
        namespace internal
        {
            int _error = CCBIND_OK;

            int _moduleDepth = 0;

            lua_State* _luaState = nullptr;

            bool beginModule(const std::type_info& info, const char* name, const char* supername, bool isClass)
            {
                CCASSERT(_moduleDepth == 0, "Module depth not 0, did you forget to call bind::end()");
                _moduleDepth = 1;

                lua_State* L = _luaState;
                std::string fullname = name;
                size_t start = 0;
                size_t end = fullname.find('.');
                while (end != std::string::npos)
                {
                    const std::string& module = fullname.substr(start, end);
                    CCASSERT(!module.empty(), "Empty module name is not allowed!");
                    tolua_module(L, module.c_str(), 0);
                    tolua_beginmodule(L, module.c_str());
                    start = end + 1;
                    end = fullname.find('.', start);
                    _moduleDepth++;
                }

                const std::string& classname = fullname.substr(start, end);
                CCASSERT(!classname.empty(), "Empty class name is not allowed!");
                if (isClass)
                {
                    tolua_usertype(L, fullname.c_str());
                    tolua_cclass(L, classname.c_str(), fullname.c_str(), supername ? supername : "", nullptr);
                    tolua_beginmodule(L, classname.c_str());

                    g_luaType[info.name()] = fullname;
                    g_typeCast[classname] = fullname;
                }
                else
                {
                    tolua_module(L, classname.c_str(), 0);
                    tolua_beginmodule(L, classname.c_str());
                }
                
                return true;
            }

            void endModule()
            {
                CCASSERT(_moduleDepth > 0, "Module depth is 0, did you call bind::end() without bind::begin()?");
                while (_moduleDepth > 0)
                {
                    tolua_endmodule(_luaState);
                    _moduleDepth--;
                }
            }
        }

        void end()
        {
            internal::endModule();
        }

        void function(const char* name, lua_CFunction func, int)
        {
            tolua_function(internal::_luaState, name, func);
        }

        void constant(const char* name, lua_Number value, int)
        {
            tolua_constant(internal::_luaState, name, value);
        }

        void variable(const char* name, lua_CFunction getter, lua_CFunction setter, int)
        {
            tolua_variable(internal::_luaState, name, getter, setter);
        }

        int getError()
        {
            int ret = internal::_error;
            internal::_error = CCBIND_OK;
            return ret;
        }

        void reportError(int _error)
        {
            if (internal::_error == CCBIND_OK)
                internal::_error = _error;
        }
    }
}