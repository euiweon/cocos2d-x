/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "scripting/lua-bindings/manual/spine/lua_cocos2dx_spine_manual.hpp"
#include "scripting/lua-bindings/auto/lua_cocos2dx_spine_auto.hpp"

#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/cocos2d/LuaScriptHandlerMgr.h"
#include "scripting/lua-bindings/manual/CCLuaValue.h"
#include "editor-support/spine/spine.h"
#include "editor-support/spine/spine-cocos2dx.h"
#include "scripting/lua-bindings/manual/spine/LuaSkeletonAnimation.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

using namespace spine;

// setBlendFunc
template<class T>
static int tolua_cocos2dx_setBlendFunc(lua_State* tolua_S,const char* className)
{
    if (NULL == tolua_S || NULL == className || strlen(className) == 0)
        return 0;

    int argc = 0;
    T* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,className,0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<T*>(tolua_tousertype(tolua_S,1,0));

    argc = lua_gettop(tolua_S) - 1;
    if (2 == argc)
    {
        CCLOG("setBlendFunc of %s will deprecate two int parameter form,please pass a table like {src = xx, dst = xx} as a parameter", className);
        
        GLenum src, dst;
        if (!luaval_to_int32(tolua_S, 2, (int32_t*)&src, StringUtils::format("%s%s",className,":setBlendFunc").c_str()))
            return 0;

        if (!luaval_to_int32(tolua_S, 3, (int32_t*)&dst,StringUtils::format("%s%s",className,":setBlendFunc").c_str()))
            return 0;

        BlendFunc blendFunc = {src, dst};
        self->setBlendFunc(blendFunc);
        return 0;
    }


    luaL_error(tolua_S, "'setBlendFunc' has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'setBlendFunc'.",&tolua_err);
    return 0;
#endif
}


static int lua_cocos2dx_CCSkeletonAnimation_createWithFile(lua_State* L)
{
    if (nullptr == L)
        return 0 ;
    
    int argc = 0;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
	if (!tolua_isusertable(L,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (2 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err)  ||
            !tolua_isstring(L, 3 ,0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* skeletonDataFile = tolua_tostring(L, 2, "");
        const char* atlasFile = tolua_tostring(L, 3, "");
        
        auto tolua_ret = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile);
        
        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"sp.SkeletonAnimation");
        return 1;
    } else if (3 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err)  ||
            !tolua_isstring(L, 3 ,0, &tolua_err)  ||
            !tolua_isnumber(L, 4 ,0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* skeletonDataFile = tolua_tostring(L, 2, "");
        const char* atlasFile = tolua_tostring(L, 3, "");
        LUA_NUMBER scale = tolua_tonumber(L, 4, 1);

        auto tolua_ret = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile, scale);

        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"sp.SkeletonAnimation");
        return 1;
    }
    
    luaL_error(L, "'createWithFile' function of SkeletonAnimation has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'createWithFile'.",&tolua_err);
#endif
    return 0;
}

int executeSpineEvent(LuaSkeletonAnimation* skeletonAnimation, int handler, spEventType eventType, spTrackEntry* entry, spEvent* event = nullptr )
{
    if (nullptr == skeletonAnimation || 0 == handler)
        return 0;
    
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    if (nullptr == stack)
        return 0;
    
    lua_State* L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    if (nullptr == L)
        return 0;
    
    int ret = 0;
    
    std::string animationName = (entry && entry->animation) ? entry->animation->name : "";
    std::string eventTypeName = "";
    
    switch (eventType) {
        case spEventType::SP_ANIMATION_START:
            {
                eventTypeName = "start";
            }
            break;
        case spEventType::SP_ANIMATION_INTERRUPT:
            {
                eventTypeName = "interrupt";
            }
                break;
        case spEventType::SP_ANIMATION_END:
            {
                eventTypeName = "end";
            }
            break;
        case spEventType::SP_ANIMATION_DISPOSE:
            {
                eventTypeName = "dispose";
            }
            break;
        case spEventType::SP_ANIMATION_COMPLETE:
            {
                eventTypeName = "complete";
            }
            break;
        case spEventType::SP_ANIMATION_EVENT:
            {
                eventTypeName = "event";
            }
            break;
            
        default:
            break;
    }
    
    LuaValueDict spineEvent;
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("type", LuaValue::stringValue(eventTypeName)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("trackIndex", LuaValue::intValue(entry->trackIndex)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("animation", LuaValue::stringValue(animationName)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("loopCount", LuaValue::intValue(std::floor(entry->trackTime / entry->animationEnd))));
    
    if (nullptr != event)
    {
        LuaValueDict eventData;
        eventData.insert(eventData.end(), LuaValueDict::value_type("name", LuaValue::stringValue(event->data->name)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("intValue", LuaValue::intValue(event->data->intValue)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("floatValue", LuaValue::floatValue(event->data->floatValue)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("stringValue", LuaValue::stringValue(event->data->stringValue)));
        spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("eventData", LuaValue::dictValue(eventData)));
    }
    
    stack->pushLuaValueDict(spineEvent);
    ret = stack->executeFunctionByHandler(handler, 1);
    return ret;
}

int tolua_Cocos2d_CCSkeletonAnimation_registerSpineEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err)                  ||
        !tolua_isnoobj(tolua_S,4,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
    	LuaSkeletonAnimation* self    = (LuaSkeletonAnimation*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            int handler = (  toluafix_ref_function(tolua_S,2,0));
            spEventType eventType = static_cast<spEventType>((int)tolua_tonumber(tolua_S, 3, 0));
            
            switch (eventType) {
                case spEventType::SP_ANIMATION_START:
                    {
                        self->setStartListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START);
                    }
                    break;
                case spEventType::SP_ANIMATION_INTERRUPT:
                    {
                        self->setInterruptListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT);
                    }
                    break;
                case spEventType::SP_ANIMATION_END:
                    {
                        self->setEndListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_END);
                    }
                    break;
                case spEventType::SP_ANIMATION_DISPOSE:
                {
                    self->setDisposeListener([=](spTrackEntry* entry){
                        executeSpineEvent(self, handler, eventType, entry);
                    });
                    ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE);
                }
                    break;
                case spEventType::SP_ANIMATION_COMPLETE:
                    {
                        self->setCompleteListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE);
                    }
                    break;
                case spEventType::SP_ANIMATION_EVENT:
                    {
                        self->setEventListener([=](spTrackEntry* entry, spEvent* event){
                            executeSpineEvent(self, handler, eventType, entry, event);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_EVENT);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'registerSpineEventHandler'.",&tolua_err);
    return 0;
#endif
}

int tolua_Cocos2d_CCSkeletonAnimation_unregisterSpineEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
    	LuaSkeletonAnimation* self    = (LuaSkeletonAnimation*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            spEventType eventType = static_cast<spEventType>((int)tolua_tonumber(tolua_S, 2, 0));
            ScriptHandlerMgr::HandlerType handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START;
            switch (eventType) {
                case spEventType::SP_ANIMATION_START:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START;
                    self->setStartListener(nullptr);
                    break;
                case spEventType::SP_ANIMATION_INTERRUPT:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT;
                    break;
                case spEventType::SP_ANIMATION_END:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_END;
                    self->setEndListener(nullptr);
                    break;
                case spEventType::SP_ANIMATION_DISPOSE:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE;
                    break;
                case spEventType::SP_ANIMATION_COMPLETE:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE;
                    self->setCompleteListener(nullptr);
                    break;
                case spEventType::SP_ANIMATION_EVENT:
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_EVENT;
                    self->setEventListener(nullptr);
                    break;
                    
                default:
                    break;
            }
            ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, handlerType);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'unregisterScriptHandler'.",&tolua_err);
    return 0;
#endif
}

extern int lua_cocos2dx_spine_SkeletonRenderer_setBlendFunc(lua_State* tolua_S);

CC_DEPRECATED_ATTRIBUTE static int tolua_spine_SkeletoneAnimation_setBlendFunc(lua_State* tolua_S)
{
    int argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
        return tolua_cocos2dx_setBlendFunc<spine::SkeletonAnimation>(tolua_S,"sp.SkeletonAnimation");
    }
    
    return lua_cocos2dx_spine_SkeletonRenderer_setBlendFunc(tolua_S);
}

static int lua_cocos2dx_spine_SkeletonAnimation_addAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_addAnimation'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:addAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:addAnimation");
        if(!ok)
            return 0;
        cobj->addAnimation(arg0, arg1, arg2);
        
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        double arg3;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:addAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:addAnimation");
        
        ok &= luaval_to_number(tolua_S, 5,&arg3, "sp.SkeletonAnimation:addAnimation");
        if(!ok)
            return 0;
        
        cobj->addAnimation(arg0, arg1, arg2, arg3);

        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "addAnimation",argc, 3);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_addAnimation'.",&tolua_err);
#endif
    
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_setAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_setAnimation'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:setAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:setAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:setAnimation");
        if(!ok)
            return 0;
        
        cobj->setAnimation(arg0, arg1, arg2);
        
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "setAnimation",argc, 3);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_setAnimation'.",&tolua_err);
#endif
    
    return 0;
}

static void extendCCSkeletonAnimation(lua_State* L)
{
    lua_pushstring(L, "sp.SkeletonAnimation");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "create", lua_cocos2dx_CCSkeletonAnimation_createWithFile);
        tolua_function(L, "registerSpineEventHandler", tolua_Cocos2d_CCSkeletonAnimation_registerSpineEventHandler00);
        tolua_function(L, "unregisterSpineEventHandler", tolua_Cocos2d_CCSkeletonAnimation_unregisterSpineEventHandler00);
        tolua_function(L, "setBlendFunc", tolua_spine_SkeletoneAnimation_setBlendFunc);
        tolua_function(L, "addAnimation", lua_cocos2dx_spine_SkeletonAnimation_addAnimation);
        tolua_function(L, "setAnimation", lua_cocos2dx_spine_SkeletonAnimation_setAnimation);
    }
    lua_pop(L, 1);
    
    /*Because sp.SkeletonAnimation:create creat a LuaSkeletonAnimation object,so we need use LuaSkeletonAnimation typename for g_luaType*/
    std::string typeName = typeid(LuaSkeletonAnimation).name();
    g_luaType[typeName] = "sp.SkeletonAnimation";
    g_typeCast["SkeletonAnimation"] = "sp.SkeletonAnimation";
}

int register_all_cocos2dx_spine_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendCCSkeletonAnimation(L);
    
    return 0;
}

int register_spine_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L,-1))//stack:...,_G,
    {
        register_all_cocos2dx_spine(L);
        register_all_cocos2dx_spine_manual(L);
    }
    lua_pop(L, 1);

    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif

void speventdata_to_luaval(lua_State* L, const spEventData& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "name");
    lua_pushstring(L, v.name);
    lua_rawset(L, -3);

    lua_pushstring(L, "intValue");
    lua_pushnumber(L, (lua_Number)v.intValue);
    lua_rawset(L, -3);

    lua_pushstring(L, "floatValue");
    lua_pushnumber(L, (lua_Number)v.floatValue);
    lua_rawset(L, -3);

    lua_pushstring(L, "stringValue");
    lua_pushstring(L, v.stringValue);
    lua_rawset(L, -3);
}

void spevent_to_luaval(lua_State* L, const spEvent& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "data");
    speventdata_to_luaval(L, *v.data);
    lua_rawset(L, -3);

    lua_pushstring(L, "intValue");
    lua_pushnumber(L, (lua_Number)v.intValue);
    lua_rawset(L, -3);

    lua_pushstring(L, "floatValue");
    lua_pushnumber(L, (lua_Number)v.floatValue);
    lua_rawset(L, -3);

    lua_pushstring(L, "stringValue");
    lua_pushstring(L, v.stringValue);
    lua_rawset(L, -3);
}

void spbonedata_to_luaval(lua_State* L, const spBoneData* v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "index");
    lua_pushnumber(L, (lua_Number)v->index);
    lua_rawset(L, -3);

    lua_pushstring(L, "name");
    lua_pushstring(L, v->name);
    lua_rawset(L, -3);

    if (strcmp(v->name, "root") && v->parent)
    {
        lua_pushstring(L, "parent");
        spbonedata_to_luaval(L, v->parent);
        lua_rawset(L, -3);
    }

    lua_pushstring(L, "length");
    lua_pushnumber(L, (lua_Number)v->length);
    lua_rawset(L, -3);

    lua_pushstring(L, "x");
    lua_pushnumber(L, (lua_Number)v->x);
    lua_rawset(L, -3);

    lua_pushstring(L, "y");
    lua_pushnumber(L, (lua_Number)v->y);
    lua_rawset(L, -3);

    lua_pushstring(L, "rotation");
    lua_pushnumber(L, (lua_Number)v->rotation);
    lua_rawset(L, -3);

    lua_pushstring(L, "scaleX");
    lua_pushnumber(L, (lua_Number)v->scaleX);
    lua_rawset(L, -3);

    lua_pushstring(L, "scaleY");
    lua_pushnumber(L, (lua_Number)v->scaleY);
    lua_rawset(L, -3);

    lua_pushstring(L, "shearX");
    lua_pushnumber(L, (lua_Number)v->shearX);
    lua_rawset(L, -3);

    lua_pushstring(L, "shearX");
    lua_pushnumber(L, (lua_Number)v->shearX);
    lua_rawset(L, -3);

    lua_pushstring(L, "transformMode");
    lua_pushnumber(L, (lua_Number)v->transformMode);
    lua_rawset(L, -3);
}

void spbone_to_luaval(lua_State* L, const spBone& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "data");
    spbonedata_to_luaval(L, v.data);
    lua_rawset(L, -3);

    if (strcmp(v.data->name, "root") && v.parent)
    {
        lua_pushstring(L, "parent");
        spbone_to_luaval(L, *v.parent);
        lua_rawset(L, -3);
    }
    
    lua_pushstring(L, "x");
    lua_pushnumber(L, (lua_Number)v.x);
    lua_rawset(L, -3);

    lua_pushstring(L, "y");
    lua_pushnumber(L, (lua_Number)v.y);
    lua_rawset(L, -3);

    lua_pushstring(L, "rotation");
    lua_pushnumber(L, (lua_Number)v.rotation);
    lua_rawset(L, -3);

    lua_pushstring(L, "scaleX");
    lua_pushnumber(L, (lua_Number)v.scaleX);
    lua_rawset(L, -3);

    lua_pushstring(L, "scaleY");
    lua_pushnumber(L, (lua_Number)v.scaleY);
    lua_rawset(L, -3);

    lua_pushstring(L, "shearX");
    lua_pushnumber(L, (lua_Number)v.shearX);
    lua_rawset(L, -3);

    lua_pushstring(L, "shearY");
    lua_pushnumber(L, (lua_Number)v.shearY);
    lua_rawset(L, -3);

    lua_pushstring(L, "m00");
    lua_pushnumber(L, (lua_Number)v.a);
    lua_rawset(L, -3);

    lua_pushstring(L, "m01");
    lua_pushnumber(L, (lua_Number)v.b);
    lua_rawset(L, -3);

    lua_pushstring(L, "worldX");
    lua_pushnumber(L, (lua_Number)v.worldX);
    lua_rawset(L, -3);

    lua_pushstring(L, "m10");
    lua_pushnumber(L, (lua_Number)v.c);
    lua_rawset(L, -3);

    lua_pushstring(L, "m11");
    lua_pushnumber(L, (lua_Number)v.d);
    lua_rawset(L, -3);

    lua_pushstring(L, "worldX");
    lua_pushnumber(L, (lua_Number)v.worldX);
    lua_rawset(L, -3);
}

void spskeleton_to_luaval(lua_State* L, const spSkeleton& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "x");
    lua_pushnumber(L, (lua_Number)v.x);
    lua_rawset(L, -3);

    lua_pushstring(L, "y");
    lua_pushnumber(L, (lua_Number)v.y);
    lua_rawset(L, -3);

    lua_pushstring(L, "flipX");
    lua_pushnumber(L, (lua_Number)v.flipX);
    lua_rawset(L, -3);

    lua_pushstring(L, "flipY");
    lua_pushnumber(L, (lua_Number)v.flipY);
    lua_rawset(L, -3);

    lua_pushstring(L, "time");
    lua_pushnumber(L, (lua_Number)v.time);
    lua_rawset(L, -3);

    lua_pushstring(L, "boneCount");
    lua_pushnumber(L, (lua_Number)v.bonesCount);
    lua_rawset(L, -3);

    lua_pushstring(L, "slotCount");
    lua_pushnumber(L, (lua_Number)v.slotsCount);
    lua_rawset(L, -3);
}

void spattachment_to_luaval(lua_State* L, const spAttachment& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "name");
    lua_pushstring(L, v.name);
    lua_rawset(L, -3);

    lua_pushstring(L, "type");
    lua_pushnumber(L, (lua_Number)v.type);
    lua_rawset(L, -3);
}

void spColor_to_luaval(lua_State* L, const spColor& v)
{
    if (NULL  == L) {
        return;
    }

    lua_newtable(L);

    lua_pushstring(L, "r");
    lua_pushnumber(L, (lua_Number)v.r);
    lua_rawset(L, -3);

    lua_pushstring(L, "g");
    lua_pushnumber(L, (lua_Number)v.g);
    lua_rawset(L, -3);

    lua_pushstring(L, "b");
    lua_pushnumber(L, (lua_Number)v.b);
    lua_rawset(L, -3);
}

void spslotdata_to_luaval(lua_State* L, const spSlotData& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "name");
    lua_pushstring(L, v.name);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachmentName");
    lua_pushstring(L, v.attachmentName);
    lua_rawset(L, -3);

    lua_pushstring(L, "color");
    spColor_to_luaval(L, v.color);
    lua_rawset(L, -3);

    lua_pushstring(L, "darkColor");
    spColor_to_luaval(L, *v.darkColor);
    lua_rawset(L, -3);

    lua_pushstring(L, "blendMode");
    lua_pushnumber(L, (lua_Number)v.blendMode);
    lua_rawset(L, -3);

    lua_pushstring(L, "boneData");
    spbonedata_to_luaval(L, v.boneData);
    lua_rawset(L, -3);

    lua_pushstring(L, "index");
    lua_pushnumber(L, (lua_Number)v.index);
    lua_rawset(L, -3);
}

void spslot_to_luaval(lua_State* L, const spSlot& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "color");
    spColor_to_luaval(L, v.color);
    lua_rawset(L, -3);

    lua_pushstring(L, "darkColor");
    spColor_to_luaval(L, *v.darkColor);
    lua_rawset(L, -3);

    lua_pushstring(L, "bone");
    spbone_to_luaval(L, *v.bone);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachment");
    spattachment_to_luaval(L, *v.attachment);
    lua_rawset(L, -3);

    lua_pushstring(L, "data");
    spslotdata_to_luaval(L, *v.data);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachmentVerticesCapacity");
    lua_pushnumber(L, (lua_Number)v.attachmentVerticesCapacity);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachmentVerticesCount");
    lua_pushnumber(L, (lua_Number)v.attachmentVerticesCount);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachmentVertices");
    lua_pushnumber(L, (lua_Number)*v.attachmentVertices);
    lua_rawset(L, -3);
}

void sptimeline_to_luaval(lua_State* L, const spTimeline& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "type");
    lua_pushnumber(L, (lua_Number)v.type);
    lua_rawset(L, -3);
}

void spanimationstate_to_luaval(lua_State* L, const spAnimationState& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "timeScale");
    lua_pushnumber(L, (lua_Number)v.timeScale);
    lua_rawset(L, -3);

    lua_pushstring(L, "trackCount");
    lua_pushnumber(L, (lua_Number)v.tracksCount);
    lua_rawset(L, -3);
}

void spanimation_to_luaval(lua_State* L, const spAnimation& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "duration");
    lua_pushnumber(L, (lua_Number)v.duration);
    lua_rawset(L, -3);

    lua_pushstring(L, "timelineCount");
    lua_pushnumber(L, (lua_Number)v.timelinesCount);
    lua_rawset(L, -3);

    lua_pushstring(L, "name");
    lua_pushstring(L, v.name);
    lua_rawset(L, -3);

    lua_pushstring(L, "timelines");
    sptimeline_to_luaval(L, **v.timelines);
    lua_rawset(L, -3);
}

void sptrackentry_to_luaval(lua_State* L, const spTrackEntry& v)
{
    if (NULL  == L) {
        return;
    }

	lua_newtable(L);

    lua_pushstring(L, "animation");
    spanimation_to_luaval(L, *v.animation);
    lua_rawset(L, -3);

    lua_pushstring(L, "trackIndex");
    lua_pushnumber(L, (lua_Number)v.trackIndex);
    lua_rawset(L, -3);

    lua_pushstring(L, "loop");
    lua_pushnumber(L, (lua_Number)v.loop);
    lua_rawset(L, -3);

    lua_pushstring(L, "eventThreshold");
    lua_pushnumber(L, (lua_Number)v.eventThreshold);
    lua_rawset(L, -3);

    lua_pushstring(L, "attachmentThreshold");
    lua_pushnumber(L, (lua_Number)v.attachmentThreshold);
    lua_rawset(L, -3);

    lua_pushstring(L, "drawOrderThreshold");
    lua_pushnumber(L, (lua_Number)v.drawOrderThreshold);
    lua_rawset(L, -3);

    lua_pushstring(L, "animationStart");
    lua_pushnumber(L, (lua_Number)v.animationStart);
    lua_rawset(L, -3);

    lua_pushstring(L, "animationEnd");
    lua_pushnumber(L, (lua_Number)v.animationEnd);
    lua_rawset(L, -3);

    lua_pushstring(L, "animationLast");
    lua_pushnumber(L, (lua_Number)v.animationLast);
    lua_rawset(L, -3);

    lua_pushstring(L, "nextAnimationLast");
    lua_pushnumber(L, (lua_Number)v.nextAnimationLast);
    lua_rawset(L, -3);

    lua_pushstring(L, "delay");
    lua_pushnumber(L, (lua_Number)v.delay);
    lua_rawset(L, -3);

    lua_pushstring(L, "trackTime");
    lua_pushnumber(L, (lua_Number)v.trackTime);
    lua_rawset(L, -3);

    lua_pushstring(L, "trackLast");
    lua_pushnumber(L, (lua_Number)v.trackLast);
    lua_rawset(L, -3);

    lua_pushstring(L, "nextTrackLast");
    lua_pushnumber(L, (lua_Number)v.nextTrackLast);
    lua_rawset(L, -3);

    lua_pushstring(L, "trackEnd");
    lua_pushnumber(L, (lua_Number)v.trackEnd);
    lua_rawset(L, -3);

    lua_pushstring(L, "timeScale");
    lua_pushnumber(L, (lua_Number)v.timeScale);
    lua_rawset(L, -3);

    lua_pushstring(L, "alpha");
    lua_pushnumber(L, (lua_Number)v.alpha);
    lua_rawset(L, -3);

    lua_pushstring(L, "mixTime");
    lua_pushnumber(L, (lua_Number)v.mixTime);
    lua_rawset(L, -3);

    lua_pushstring(L, "mixDuration");
    lua_pushnumber(L, (lua_Number)v.mixDuration);
    lua_rawset(L, -3);

    lua_pushstring(L, "interruptAlpha");
    lua_pushnumber(L, (lua_Number)v.interruptAlpha);
    lua_rawset(L, -3);

    lua_pushstring(L, "totalAlpha");
    lua_pushnumber(L, (lua_Number)v.totalAlpha);
    lua_rawset(L, -3);

    if (v.timelinesRotation)
    {
        lua_pushstring(L, "timelinesRotation");
        lua_pushnumber(L, (lua_Number)(*v.timelinesRotation));
        lua_rawset(L, -3);
    }

    lua_pushstring(L, "timelinesRotationCount");
    lua_pushnumber(L, (lua_Number)v.timelinesRotationCount);
    lua_rawset(L, -3);
}

#ifdef __cplusplus
}
#endif
