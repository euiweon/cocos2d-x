#ifndef __DEVTOOLS_EMSCRIPTEN_H__
#define __DEVTOOLS_EMSCRIPTEN_H__

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#include "base/CCScheduler.h"
#include "base/CCDirector.h"

using namespace cocos2d;

class DevToolsImpl
{
public:
    DevToolsImpl();

    void update(float /*dt*/);

    void step();

    void pause();

    void resume();

    void select(const Node* node);

    void nodeEntered(const Node* node);

    void nodeExited(const Node* node);

    void nodeNameChanged(const Node* node);

    static DevToolsImpl* getInstance();

private:
    unsigned int _tick;
    Scheduler* _scheduler;
    Director* _director;
};

#endif // CC_PLATFORM_EMSCRIPTEN
#endif // __DEVTOOLS_EMSCRIPTEN_H__