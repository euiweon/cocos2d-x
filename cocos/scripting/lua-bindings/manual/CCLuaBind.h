#ifndef __LUA_BIND_H_
#define __LUA_BIND_H_

#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

#define CCBIND_OK 1
#define CCBIND_INVALID_ARG_CNT 2
#define CCBIND_INVALID_ARG_0 10000
#define CCBIND_INVALID_ARG_1 10001
#define CCBIND_INVALID_ARG_2 10002
#define CCBIND_INVALID_ARG_3 10003
#define CCBIND_INVALID_ARG_4 10004
#define CCBIND_INVALID_ARG_5 10005
#define CCBIND_INVALID_ARG_6 10006
#define CCBIND_INVALID_ARG_7 10007
#define CCBIND_INVALID_ARG_8 10008
#define CCBIND_INVALID_ARG_9 10009

#define CCBIND_FUNC(f) cocos2d::bind::internal::MemberFunction<decltype(&f), &f>::invoke
#define CCBIND_SFUNC(f) cocos2d::bind::internal::StaticFunction<decltype(&f), &f>::invoke
#define CCBIND_OVERLOADED_FUNC(f) cocos2d::bind::internal::OverloadedMemberFunction<decltype(&f), &f>::invoke
#define CCBIND_OVERLOADED_SFUNC(f) cocos2d::bind::internal::OverloadedStaticFunction<decltype(&f), &f>::invoke
#define CCBIND_GETTER(field) cocos2d::bind::internal::MemberGetter<decltype(&field), &field>::invoke
#define CCBIND_SETTER(field) cocos2d::bind::internal::MemberSetter<decltype(&field), &field>::invoke
#define CCBIND_SGETTER(field) cocos2d::bind::internal::StaticGetter<decltype(&field), &field>::invoke
#define CCBIND_SSETTER(field) cocos2d::bind::internal::StaticSetter<decltype(&field), &field>::invoke
#define CCBIND_CONSTANT(v) (static_cast<lua_Number>(v))
#define CCBIND_CONSTRUCTOR(T) cocos2d::bind::internal::Constructor<T>::invoke

#define CCBIND_DEFINE_TABLE_TYPE(T, luatable_to_val, val_to_luatable)   \
template <>                                                             \
struct cocos2d::bind::internal::Converter<T> {                          \
    CC_ALWAYS_INLINE static bool is(lua_State* L, int lo) {             \
        return L != nullptr && lua_istable(L, lo);                      \
    }                                                                   \
    CC_ALWAYS_INLINE static T from(lua_State* L, int lo) {              \
        return (T)luatable_to_val(cocos2d::bind::Table(L, lo));         \
    }                                                                   \
    CC_ALWAYS_INLINE static void to(lua_State* L, const T& v) {         \
        lua_newtable(L);                                                \
        val_to_luatable(v, cocos2d::bind::Table(L, -1));                \
    }                                                                   \
} 

#define CCBIND_DEFINE_ARRAY_TYPE(T, luatable_to_val, val_to_luatable)   \
template <>                                                             \
struct cocos2d::bind::internal::Converter<T> {                          \
    CC_ALWAYS_INLINE static bool is(lua_State* L, int lo) {             \
        return L != nullptr && lua_istable(L, lo);                      \
    }                                                                   \
    CC_ALWAYS_INLINE static T from(lua_State* L, int lo) {              \
        return (T)luatable_to_val(cocos2d::bind::Array(L, lo));         \
    }                                                                   \
    CC_ALWAYS_INLINE static void to(lua_State* L, const T& v) {         \
        lua_newtable(L);                                                \
        val_to_luatable(v, cocos2d::bind::Array(L, -1));                \
    }                                                                   \
} 

#ifdef __cplusplus
extern "C" {
#endif
extern int lua_isusertype (lua_State* L, int lo, const char* type);
#ifdef __cplusplus
}
#endif

NS_CC_BEGIN

namespace bind
{
    template<typename T>
    CC_ALWAYS_INLINE bool begin(const char* name, const char* supername = nullptr)
    {
        return internal::beginModule(typeid(T), name, supername, !std::is_enum<T>::value);
    } 

    void end();

    int getError();

    void reportError(int error);

    void function(const char* name, lua_CFunction func, int policy = 0);

    void constant(const char* name, lua_Number value, int policy = 0);

    void variable(const char* name, lua_CFunction getter, lua_CFunction setter, int policy = 0);

    struct Argv
    {
    private:
        lua_State* L;
        
    public:
        // constructor for internal use
        Argv(lua_State* l)
        :L(l)
        {

        };

        template<typename T> 
        T get(std::size_t i) const
        {
            return internal::Converter<T>::from(L, i + 2);
        }

        template<typename T> 
        CC_ALWAYS_INLINE bool is(std::size_t i) const
        {
            return internal::Converter<T>::is(L, i + 2);
        }

        template<typename T> 
        CC_ALWAYS_INLINE void rval(T returnVal)
        {
            internal::Converter<T>::to(L, returnVal);
        }
    };

    struct Table
    {
    private:
        lua_State* L;
        int idx;

    public:
        // constructor for internal use
        Table(lua_State* l, int i)
        :L(l)
        ,idx(i)
        {
        };

        template<typename T> 
        CC_ALWAYS_INLINE T get(const char* key, const T& def) const
        {
            lua_pushstring(L, key);
            lua_gettable(L, idx - 1);
            if (internal::Converter<T>::is(L, -1))
            {
                const T& ret = internal::Converter<T>::from(L, -1);
                lua_pop(L, 1);
                return ret;
            }
            else
            {
                lua_pop(L, 1);
                return def;
            }
        }

        template<typename T> 
        CC_ALWAYS_INLINE void set(const char* key, const T& value)
        {
            lua_pushstring(L, key);
            internal::Converter<T>::to(L, value);
            lua_rawset(L, idx - 2);
        }
    };

    struct Array
    {
    private:
        lua_State* L;
        int idx;

    public:
        // constructor for internal use
        Array(lua_State* l, int i)
        :L(l)
        ,idx(i)
        {
        };

        template<typename T> 
        CC_ALWAYS_INLINE T get(size_t key, const T& def) const
        {
            lua_pushnumber(L, key);
            lua_gettable(L, idx - 1);
            if (internal::Converter<T>::is(L, -1))
            {
                const T& ret = internal::Converter<T>::from(L, -1);
                lua_pop(L, 1);
                return ret;
            }
            else
            {
                lua_pop(L, 1);
                return def;
            }
        }

        template<typename T> 
        CC_ALWAYS_INLINE void set(size_t key, const T& value)
        {
            lua_pushnumber(L, key);
            internal::Converter<T>::to(L, value);
            lua_rawset(L, idx - 2);
        }
    };

    namespace internal
    {
        extern int _error;

        extern lua_State* _luaState;

        CC_NOINLINE bool beginModule(const std::type_info& info, const char* name, const char* supername, bool isClass);

        CC_NOINLINE void endModule();

        // Converter converts objects between c++ and lua
        template <typename T, typename = void>
        struct Converter
        {
            static void is() = delete;
            static void from() = delete;
            static void to() = delete;
        };

        template <typename T>
        struct Converter<T*>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                if (L == nullptr || lua_gettop(L) < lo)
                    return false;

                if (lua_isnil(L, lo))
                    return true;

				if (const char* type = getLuaTypeName<T>())
					return lua_isusertype(L, lo, type);
				
				return false;
            }

            CC_ALWAYS_INLINE static T* from(lua_State* L, int lo)
            {
                return static_cast<T*>(tolua_tousertype(L, lo, 0));
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, T* ret)
            {
                const char* type = getLuaTypeName(ret, nullptr);
                tolua_pushusertype(L, (void*)ret, type ? type : getLuaTypeName<T>()); 
            }
        };

        template<typename T>
        struct Converter<T, typename std::enable_if<std::is_base_of<cocos2d::Ref, T>::value>::type>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                if (L == nullptr || lua_gettop(L) < lo)
                    return false;

                if (lua_isnil(L, lo))
                    return true;

				if (const char* type = getLuaTypeName<T>())
					return lua_isusertype(L, lo, type);
				
				return false;
            }

            CC_ALWAYS_INLINE static T* from(lua_State* L, int lo)
            {
                return static_cast<T*>(tolua_tousertype(L, lo, 0));
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, T* ret)
            {
                toluafix_pushusertype_ccobject(L, ret->_ID, &ret->_luaID, (void*)ret, getLuaTypeName<T>());
            }
        };

#define __INTERNAL_DEFINE_NUMBER_TYPE(T) \
        template <> \
        struct Converter<T> { \
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo) { \
                return L != nullptr && lua_isnumber(L, lo); \
            } \
            CC_ALWAYS_INLINE static T from(lua_State* L, int lo) { \
                return (T)lua_tonumber(L, lo); \
            } \
            CC_ALWAYS_INLINE static void to(lua_State* L, T ret) { \
                lua_pushnumber(L, (lua_Number)ret); \
            } \
        };

        __INTERNAL_DEFINE_NUMBER_TYPE(uint8_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(int8_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(uint16_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(int16_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(int32_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(uint32_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(int64_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(uint64_t);
        __INTERNAL_DEFINE_NUMBER_TYPE(float);
        __INTERNAL_DEFINE_NUMBER_TYPE(double);
#undef __INTERNAL_DEFINE_NUMBER_TYPE

        template <>
        struct Converter<bool>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                return L != nullptr && lua_isboolean(L, lo);
            }

            CC_ALWAYS_INLINE static bool from(lua_State* L, int lo)
            {
                return lua_toboolean(L, lo);
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, bool ret)
            {
                lua_pushboolean(L, ret);
            }
        };

        template <typename T>
        struct Converter<std::basic_string<T>>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                return L != nullptr && lua_isstring(L, lo);
            }

            CC_ALWAYS_INLINE static std::basic_string<T> from(lua_State* L, int lo)
            {
                size_t length;
                const char* data = lua_tolstring(L, lo, &length);
                return std::basic_string<T>(data, length);
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, const std::basic_string<T>& ret)
            {
                lua_pushlstring(L, ret.data(), ret.length() * sizeof(T));
            }
        };

        // enum to int
        template<typename T>
        struct Converter<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                return L != nullptr && lua_isnumber(L, lo);
            }

            CC_ALWAYS_INLINE static T from(lua_State* L, int lo)
            {
                return static_cast<T>((int)lua_tonumber(L, lo));
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, T ret)
            {
                lua_pushnumber(L, (int)ret);
            }
        };
        
        template <typename T>
        struct Converter<cocos2d::Vector<T>>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                return L != nullptr && lua_istable(L, lo);
            }

            CC_ALWAYS_INLINE static cocos2d::Vector<T> from(lua_State* L, int lo)
            {
                cocos2d::Vector<T> v;
                luaval_to_ccvector(L, lo, &v, nullptr);
                return v;
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, const cocos2d::Vector<T>& ret)
            {
                ccvector_to_luaval<T>(L, ret);
            }
        };

        template <typename T>
        struct Converter<std::vector<T>>
        {
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo)
            {
                return L != nullptr && lua_istable(L, lo);
            }

            CC_ALWAYS_INLINE static std::vector<T> from(lua_State* L, int lo)
            {
                std::vector<T> v;
                size_t len = lua_objlen(L, lo);
                for (size_t i = 0; i < len; i++)
                {
                    lua_pushnumber(L, i + 1);
                    lua_gettable(L, lo);
                    v.push_back(Converter<T>::from(L, -1));
                    lua_pop(L, 1);
                }
                return v;
            }

            CC_ALWAYS_INLINE static void to(lua_State* L, const std::vector<T>& ret)
            {
                lua_newtable(L);
                int i = 1;
                for (const T& v: ret)
                {
                    lua_pushnumber(L, (lua_Number)i);
					Converter<T>::to(L, v);
                    lua_rawset(L, -3);
                    i++;
                }
            }
        };

#define __INTERNAL_DEFINE_COCOS2D_TYPE(T, fromInternal, toInternal) \
        template <> \
        struct Converter<T> { \
            CC_ALWAYS_INLINE static bool is(lua_State* L, int lo) { \
                T v; \
                return fromInternal(L, lo, &v, nullptr); \
            } \
            CC_ALWAYS_INLINE static T from(lua_State* L, int lo) { \
                T v; \
                fromInternal(L, lo, &v, nullptr); \
                return v; \
            } \
            CC_ALWAYS_INLINE static void to(lua_State* L, const T& v) { \
                toInternal(L, v); \
            } \
        }
        
        // NOTE: built-in types already declared (12)
        // __INTERNAL_DEFINE_COCOS2D_TYPE(unsigned short, luaval_to_ushort, ushort_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(int, luaval_to_int32, int32_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(unsigned int, luaval_to_uint32, uint32_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(uint16_t, luaval_to_uint16, uint16_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(bool, luaval_to_boolean, boolean_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(double, luaval_to_number, number_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(long long, luaval_to_long_long, long_long_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::string, luaval_to_std_string, std_string_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(ssize_t, luaval_to_ssize, ssize_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(long, luaval_to_long, long_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(unsigned long, luaval_to_ulong, ulong_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(float, luaval_to_float, float_to_luaval);

        // NOTE: ignore types which cannot be convert to lua (9)
        // __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Vec2 **points, luaval_to_array_of_vec2, array_of_vec2_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<std::string>, luaval_to_std_vector_string, std_vector_string_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<int>, luaval_to_std_vector_int, std_vector_int_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<float>, luaval_to_std_vector_float, std_vector_float_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<unsigned short>, luaval_to_std_vector_ushort, std_vector_ushort_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Tex2F, luaval_to_tex2f, tex2f_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::V3F_C4B_T2F , luaval_to_v3f_c4b_t2f, v3f_c4b_t2f_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<cocos2d::Vec2>, luaval_to_std_vector_vec2, std_vector_vec2_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<cocos2d::V3F_C4B_T2F>, luaval_to_std_vector_v3f_c4b_t2f, std_vector_v3f_c4b_t2f_to_luaval);

        // NOTE: no need as std::map is defined (1)
        //__INTERNAL_DEFINE_COCOS2D_TYPE(std::map<std::string, std::string>, luaval_to_std_map_string_string, std_map_string_string_to_luaval);

        // NOTE: does not compile (2)
        // __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::__Array*, luaval_to_array, array_to_luaval);
        // __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::__Dictionary*, luaval_to_dictionary, dictionary_to_luaval);

        // (23)
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Vec2, luaval_to_vec2, vec2_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Vec3, luaval_to_vec3, vec3_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Vec4, luaval_to_vec4, vec4_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::BlendFunc, luaval_to_blendfunc, blendfunc_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::PhysicsMaterial, luaval_to_physics_material, physics_material_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Size, luaval_to_size, size_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Rect, luaval_to_rect, rect_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Color4B, luaval_to_color4b, color4b_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Color4F, luaval_to_color4f, color4f_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Color3B, luaval_to_color3b, color3b_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::AffineTransform, luaval_to_affinetransform, affinetransform_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::FontDefinition, luaval_to_fontdefinition, fontdefinition_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::TTFConfig, luaval_to_ttfconfig, ttfconfig_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Uniform, luaval_to_uniform, uniform_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::VertexAttrib, luaval_to_vertexattrib, vertexattrib_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Mat4, luaval_to_mat4, mat4_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Value, luaval_to_ccvalue, ccvalue_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::ValueMap, luaval_to_ccvaluemap, ccvaluemap_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::ValueMapIntKey, luaval_to_ccvaluemapintkey, ccvaluemapintkey_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::ValueVector, luaval_to_ccvaluevector, ccvaluevector_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::MeshVertexAttrib, luaval_to_mesh_vertex_attrib, mesh_vertex_attrib_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Quaternion, luaval_to_quaternion, quaternion_to_luaval);
        __INTERNAL_DEFINE_COCOS2D_TYPE(cocos2d::Texture2D::TexParams, luaval_to_texparams, texParams_to_luaval);
        // no need as std::vector is defined (1)
        // __INTERNAL_DEFINE_COCOS2D_TYPE(std::vector<cocos2d::Vec3>, luaval_to_std_vector_vec3, std_vector_vec3_to_luaval);
#undef __INTERNAL_DEFINE_COCOS2D_TYPE

        // void lua function as callback
        template <typename... Args>
        struct Converter<std::function<void(Args...)>>
        {
            inline static bool is(lua_State* L, int lo)
            {
                return L != nullptr && (lua_isfunction(L, lo) || lua_isnil(L, lo));
            }

            inline static std::function<void(Args...)> from(lua_State* L, int lo)
            {
                if (lua_isnil(L, lo))
                    return nullptr;

                LUA_FUNCTION handler = (toluafix_ref_function(L, lo, 0));
                return [handler](Args... args)
                {
                    auto stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
                    std::initializer_list<int>{(Converter<Args>::to(stack->getLuaState(), args), 0)...};
                    stack->executeFunctionByHandler(handler, sizeof...(Args));
                };
            }

            static void to() = delete;
        };

        // non-void lua function as callback
        template <typename ReturnType, typename... Args>
        struct Converter<std::function<ReturnType(Args...)>>
        {
            inline static bool is(lua_State* L, int lo)
            {
                return L != nullptr && (lua_isfunction(L, lo) || lua_isnil(L, lo));
            }

            inline static std::function<ReturnType(Args...)> from(lua_State* L, int lo)
            {
                if (lua_isnil(L, lo))
                    return nullptr;

                LUA_FUNCTION handler = (toluafix_ref_function(L, lo, 0));
                return [handler](Args... args) -> ReturnType
                {
                    auto stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
                    std::initializer_list<int>{(Converter<Args>::to(stack->getLuaState(), args), 0)...};
                    ReturnType ret;
                    stack->executeFunction(handler, sizeof...(Args), 1, [&ret](lua_State* L, int)
                    {
                        ret = Converter<ReturnType>::from(L, -1);
                        lua_pop(L, 1);
                    });
					return ret;
                };
            }

            static void to() = delete;
        };

        template<typename T>
        struct Converter<const T> : public Converter<T> {};

        template<typename T>
        struct Converter<T&> : public Converter<T> {};

        template<typename T>
        struct Converter<const T&> : public Converter<T> {};

        // Simple index_sequence
        template<std::size_t... I>
        struct index_sequence{};

        template<std::size_t N, std::size_t... I>
        struct make_index_sequence : make_index_sequence<N-1, N-1, I...> {};

        template<std::size_t... I>
        struct make_index_sequence<0, I...> : index_sequence<I...> {};

        // Apply class function returns non-void
        template<typename ReturnType, typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, ReturnType(*f)(Args...), index_sequence<I...>)
        {
            Converter<ReturnType>::to(L, f(Converter<Args>::from(L, I + 2)...));
        }

        // Apply class function returns void
        template<typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, void(*f)(Args...), index_sequence<I...>)
        {
            f(Converter<Args>::from(L, I + 2)...);
        }

        // Apply member function returns non-void
        template<typename ClassType, typename ReturnType, typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, ClassType* cobj, ReturnType(ClassType::*f)(Args...), index_sequence<I...>)
        {
            Converter<ReturnType>::to(L, (cobj->*f)(Converter<Args>::from(L, I + 2)...));
        }

        // Apply member function returns void
        template<typename ClassType, typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, ClassType* cobj, void(ClassType::*f)(Args...), index_sequence<I...>)
        {
            (cobj->*f)(Converter<Args>::from(L, I + 2)...);
        }

        // Apply member function with const qualifier returns non-void
        template<typename ClassType, typename ReturnType, typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, ClassType* cobj, ReturnType(ClassType::*f)(Args...) const, index_sequence<I...>)
        {
            Converter<ReturnType>::to(L, (cobj->*f)(Converter<Args>::from(L, I + 2)...));
        }

        // Apply member function with const qualifier returns void
        template<typename ClassType, typename... Args, std::size_t... I>
        CC_ALWAYS_INLINE void apply_with_index_sequence(lua_State* L, ClassType* cobj, void(ClassType::*f)(Args...) const, index_sequence<I...>)
        {
            (cobj->*f)(Converter<Args>::from(L, I + 2)...);
        }

        // function templates
        template<typename Signature, Signature f> struct StaticFunction;
        template<typename ReturnType, typename... Args, ReturnType(*f)(Args...)>
        struct StaticFunction<ReturnType(*)(Args...), f> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                if (argc == sizeof...(Args))
                {
                    int i = 2;
                    bool ok[] = {Converter<Args>::is(L, i++)..., false}; // evaluation order is guaranteed by comma-separated initializer, ending with false to avoid 0 size array
                    for (int j = 0; j < sizeof...(Args); j ++)
                    {
                        if (!ok[j])
                        {
#if COCOS2D_DEBUG >= 1
                            tolua_error(L,"Invalid arguments when calling static function", nullptr);
#endif
                            return 0;
                        }
                    }

                    apply_with_index_sequence(L, f, make_index_sequence<sizeof...(Args)>());
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling static function with wrong number of arguments: %d, was expecting %d\n ", argc, sizeof...(Args));
#endif
                return 0;
            }
        };

        template<typename Signature, Signature f> struct MemberFunction;
        template<typename ClassType, typename ReturnType, typename... Args, ReturnType(ClassType::*f)(Args...)>
        struct MemberFunction<ReturnType(ClassType::*)(Args...), f> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ClassType* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
                tolua_Error err;
                if (!tolua_isusertype(L, 1, getLuaTypeName<ClassType>(), 0, &err))
                {
                    tolua_error(L,"#fError in converting this pointer", &err);
                    return 0;
                }
#endif
                cobj = static_cast<ClassType*>(tolua_tousertype(L, 1, 0));
#if COCOS2D_DEBUG >= 1
                if (!cobj)
                {
                    luaL_error(L, "Invalid pointer to %s when calling its' member function\n ", getLuaTypeName<ClassType>());
                    return 0;
                }
#endif

                if (argc == sizeof...(Args))
                {
                    int i = 2;
                    bool ok[] = {Converter<Args>::is(L, i++)..., false}; // evaluation order is guaranteed by comma-separated initializer, ending with false to avoid 0 size array
                    for (int j = 0; j < sizeof...(Args); j ++)
                    {
                        if (!ok[j])
                        {
#if COCOS2D_DEBUG >= 1
                            tolua_error(L,"Invalid arguments when calling member function", nullptr);
#endif
                            return 0;
                        }
                    }

                    apply_with_index_sequence(L, cobj, f, make_index_sequence<sizeof...(Args)>());
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling member function of %s with wrong number of arguments: %d, was expecting %d\n ", getLuaTypeName<ClassType>(), argc, sizeof...(Args));
#endif
                return 0;
            }
        };

        // const member function template
        // TODO: Can't extend non-const template to avoid duplicated code block?
        template<typename ClassType, typename ReturnType, typename... Args, ReturnType(ClassType::*f)(Args...) const>
        struct MemberFunction<ReturnType(ClassType::*)(Args...) const, f> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ClassType* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
                tolua_Error err;
                if (!tolua_isusertype(L, 1, getLuaTypeName<ClassType>(), 0, &err))
                {
                    tolua_error(L,"#fError in converting this pointer", &err);
                    return 0;
                }
#endif
                cobj = static_cast<ClassType*>(tolua_tousertype(L, 1, 0));
#if COCOS2D_DEBUG >= 1
                if (!cobj)
                {
                    luaL_error(L, "Invalid pointer to %s when calling its' member function\n ", getLuaTypeName<ClassType>());
                    return 0;
                }
#endif

                if (argc == sizeof...(Args))
                {
                    int i = 2;
                    bool ok[] = {Converter<Args>::is(L, i++)..., false}; // evaluation order is guaranteed by comma-separated initializer, ending with false to avoid 0 size array
                    for (int j = 0; j < sizeof...(Args); j ++)
                    {
                        if (!ok[j])
                        {
#if COCOS2D_DEBUG >= 1
                            tolua_error(L,"Invalid arguments when calling member function", nullptr);
#endif
                            return 0;
                        }
                    }

                    apply_with_index_sequence(L, cobj, f, make_index_sequence<sizeof...(Args)>());
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling member function of %s with wrong number of arguments: %d, was expecting %d\n ", getLuaTypeName<ClassType>(), argc, sizeof...(Args));
#endif
                return 0;
            }
        };

        template<typename Signature, Signature field> struct MemberGetter;
        template<typename ClassType, typename MemberType, MemberType(ClassType::*field)>
        struct MemberGetter<MemberType(ClassType::*), field> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ClassType* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
                tolua_Error err;
                if (!tolua_isusertype(L, 1, getLuaTypeName<ClassType>(), 0, &err))
                {
                    tolua_error(L,"#fError in converting this pointer", &err);
                    return 0;
                }
#endif
                cobj = static_cast<ClassType*>(tolua_tousertype(L, 1, 0));
#if COCOS2D_DEBUG >= 1
                if (!cobj)
                {
                    luaL_error(L, "Invalid pointer to %s when calling its' getter method\n ", getLuaTypeName<ClassType>());
                    return 0;
                }
#endif

                if (argc == 0)
                {
                    internal::Converter<MemberType>::to(L, cobj->*field);
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling getter method of %s with wrong number of arguments: %d, was expecting %d\n ", getLuaTypeName<ClassType>(), argc, 0);
#endif
                return 0;
            }
        };

        template<typename Signature, Signature field> struct MemberSetter;
        template<typename ClassType, typename MemberType, MemberType(ClassType::*field)>
        struct MemberSetter<MemberType(ClassType::*), field> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ClassType* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
                tolua_Error err;
                if (!tolua_isusertype(L, 1, getLuaTypeName<ClassType>(), 0, &err))
                {
                    tolua_error(L,"#fError in converting this pointer", &err);
                    return 0;
                }
#endif
                cobj = static_cast<ClassType*>(tolua_tousertype(L, 1, 0));
#if COCOS2D_DEBUG >= 1
                if (!cobj)
                {
                    luaL_error(L, "Invalid pointer to %s when calling its' setter method\n ", getLuaTypeName<ClassType>());
                    return 0;
                }
#endif

                if (argc == 1)
                {
#if COCOS2D_DEBUG >= 1
                    if (!Converter<MemberType>::is(L, 2))
                    {
                        tolua_error(L,"Invalid arguments when calling setter method", nullptr);
                        return 0;
                    }
#endif
                    cobj->*field = Converter<MemberType>::from(L, 2);
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling setter method of %s with wrong number of arguments: %d, was expecting %d\n ", getLuaTypeName<ClassType>(), argc, 1);
#endif
                return 0;
            }
        };

        template<typename Signature, Signature field> struct StaticGetter;
        template<typename MemberType, MemberType(*field)>
        struct StaticGetter<MemberType(*), field> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                if (argc == 0)
                {
                    internal::Converter<MemberType>::to(L, *field);
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling static getter method with wrong number of arguments: %d, was expecting %d\n ", argc, 1);
#endif
                return 0;
            }
        };

        template<typename Signature, Signature field> struct StaticSetter;
        template<typename MemberType, MemberType(*field)>
        struct StaticSetter<MemberType(*), field> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                if (argc == 1)
                {
#if COCOS2D_DEBUG >= 1
                    if (!Converter<MemberType>::is(L, 2))
                    {
                        tolua_error(L,"Invalid arguments when calling static setter method", nullptr);
                        return 0;
                    }
#endif
                    *field = Converter<MemberType>::from(L, 2);
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling static setter method with wrong number of arguments: %d, was expecting %d\n ", argc, 1);
#endif
                return 0;
            }
        };

        template<typename Signature, Signature f> struct OverloadedMemberFunction;
        template<typename ClassType, void(*f)(ClassType&, std::size_t, ::cocos2d::bind::Argv&)>
        struct OverloadedMemberFunction<void(*)(ClassType&, std::size_t, ::cocos2d::bind::Argv&), f> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ClassType* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
                tolua_Error err;
                if (!tolua_isusertype(L, 1, getLuaTypeName<ClassType>(), 0, &err))
                {
                    tolua_error(L,"#fError in converting this pointer", &err);
                    return 0;
                }
#endif
                cobj = static_cast<ClassType*>(tolua_tousertype(L, 1, 0));
#if COCOS2D_DEBUG >= 1
                if (!cobj)
                {
                    luaL_error(L, "Invalid pointer to %s when calling its' overloaded member function\n ", getLuaTypeName<ClassType>());
                    return 0;
                }
#endif

                ::cocos2d::bind::Argv argv(L);
                f(*cobj, argc, argv);
#if COCOS2D_DEBUG >= 1
                int ok = ::cocos2d::bind::getError();
                if (ok == CCBIND_INVALID_ARG_CNT)
                {
                    luaL_error(L, "Number of arguments: %d, does not match any overloaded member function\n ", argc);
                }
                else if (ok >= CCBIND_INVALID_ARG_0)
                {
                    luaL_error(L, "Invalid argument at index: %d when calling overloaded member function\n ", ok - CCBIND_INVALID_ARG_0);
                }
                else if (ok != CCBIND_OK)
                {
                    luaL_error(L, "Error %d when calling overloaded member function\n", ok);
                }

                return ok == CCBIND_OK ? 1 : 0;
#else
                return ::cocos2d::bind::getError() == CCBIND_OK ? 1 : 0;
#endif
            }
        };

        template<typename Signature, Signature f> struct OverloadedStaticFunction;
        template<void(*f)(std::size_t, ::cocos2d::bind::Argv&)>
        struct OverloadedStaticFunction<void(*)(std::size_t, ::cocos2d::bind::Argv&), f> {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                ::cocos2d::bind::Argv argv(L);
                f(argc, argv);
#if COCOS2D_DEBUG >= 1
                int ok = ::cocos2d::bind::getError();
                if (ok == CCBIND_INVALID_ARG_CNT)
                {
                    luaL_error(L, "Number of arguments: %d, does not match any overloaded static function\n ", argc);
                }
                else if (ok >= CCBIND_INVALID_ARG_0)
                {
                    luaL_error(L, "Invalid argument at index: %d when calling overloaded static function\n ", ok - CCBIND_INVALID_ARG_0);
                }
                else if (ok != CCBIND_OK)
                {
                    luaL_error(L, "Error %d when calling overloaded static function\n", ok);
                }

                return ok == CCBIND_OK ? 1 : 0;
#else
                return ::cocos2d::bind::getError() == CCBIND_OK ? 1 : 0;
#endif
            }
        };

        template<typename T, typename = void> struct Constructor;
        template<typename T>
        struct Constructor<T, typename std::enable_if<std::is_base_of<cocos2d::Ref, T>::value>::type>
        {
            static int invoke(lua_State* L)
            {
                int argc = lua_gettop(L) - 1;
                if (argc == 0)
                {
                    T* cobj = new T();
                    cobj->autorelease();
                    Converter<T>::to(L, cobj);
                    return 1;
                }
#if COCOS2D_DEBUG >= 1
                luaL_error(L, "Calling constructor with wrong number of arguments: %d, was expecting %d\n ", argc, 1);
#endif
                return 0;
            }
        };
    }
}

NS_CC_END

#endif
