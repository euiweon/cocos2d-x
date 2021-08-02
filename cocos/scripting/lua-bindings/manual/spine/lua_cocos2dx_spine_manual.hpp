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
#ifndef COCOS_SCRIPTING_LUA_BINDINGS_LUA_COCOS2DX_SPINE_MANUAL_H
#define COCOS_SCRIPTING_LUA_BINDINGS_LUA_COCOS2DX_SPINE_MANUAL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

#include "editor-support/spine/spine-cocos2dx.h"

/// @cond
TOLUA_API int register_all_cocos2dx_spine_manual(lua_State* L);
/// @endcond

/**
 * @addtogroup lua
 * @{
 */

/**
 * Call this function can import the lua bindings for the spine module.
 * After registering, we could call the related spine code conveniently in the lua.
 * If you don't want to use the spine module in the lua, you only don't call this registering function.
 * If you don't register the spine module, the package size would become smaller .
 * The current mechanism,this registering function is called in the lua_module_register.h
 */
TOLUA_API int register_spine_module(lua_State* L);

#ifdef __cplusplus
extern "C" {
#endif

void speventdata_to_luaval(lua_State* L, const spEventData& v);
void spevent_to_luaval(lua_State* L, const spEvent& v);
void spbonedata_to_luaval(lua_State* L, const const spBoneData* v);
void spbone_to_luaval(lua_State* L, const spBone& v);
void spskeleton_to_luaval(lua_State* L, const spSkeleton& v);
void spattachment_to_luaval(lua_State* L, const spAttachment& v);
void spslotdata_to_luaval(lua_State* L, const spSlotData& v);
void spslot_to_luaval(lua_State* L, const spSlot& v);
void sptimeline_to_luaval(lua_State* L, const spTimeline& v);
void spanimationstate_to_luaval(lua_State* L, const spAnimationState& v);
void spanimation_to_luaval(lua_State* L, const spAnimation& v);
void sptrackentry_to_luaval(lua_State* L, const spTrackEntry& v);

#ifdef __cplusplus
}
#endif

// end group
/// @}
#endif // #ifndef COCOS_SCRIPTING_LUA_BINDINGS_LUA_COCOS2DX_SPINE_MANUAL_H
