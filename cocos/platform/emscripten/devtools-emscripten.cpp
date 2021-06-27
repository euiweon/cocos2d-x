#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#include "devtools-emscripten.h"
#include <emscripten.h>
#include "base/ccUTF8.h"

using namespace std;
using namespace cocos2d;

DevToolsImpl::DevToolsImpl()
{
    _director = Director::getInstance();
    _scheduler = _director->getScheduler();
    _tick = 0;
    _isEnabled = EM_ASM_INT({
        return Module.initializeDevTools() ? 1 : 0;
    });
}

void DevToolsImpl::update(float /*dt*/)
{
    // tick for 2 frames becuase delta time of the 1st frame after resume is forced to 0
    _tick ++;
    if (_tick >= 2)
    {
        _director->pause();
        _scheduler->unscheduleUpdate(this);
    }
}

void DevToolsImpl::select(const Node* node)
{
    string desc = node->getDescription();

    string info = StringUtils::format("Pos = %.2f,%.2f, AP = %.2f,%.2f, Size = %.2f,%.2f, Scale = %.2f,%.2f, Rot = %.2f, Skew = %.2f,%.2f, Color = %d,%d,%d,%d, ", 
        node->getPositionX(), node->getPositionY(),
        node->getAnchorPoint().x, node->getAnchorPoint().y,
        node->getContentSize().width, node->getContentSize().height,
        node->getScaleX(), node->getScaleY(),
        node->getRotation(),
        node->getSkewX(), node->getSkewY(),
        node->getColor().r,node->getColor().g,node->getColor().b,node->getOpacity()
        );
    size_t separator = desc.find('|');
    if (separator != string::npos)
    {
        desc.insert(separator + 1, info);
    }
    else
    {
        desc = StringUtils::format("<%s | %s, Tag = %d>", desc.c_str(), info.c_str(), node->getTag());
    }

    EM_ASM({
        Module.cocosNodeSelected(UTF8ToString($0));
    }, desc.c_str());
}

void DevToolsImpl::step()
{
    _director->resume();
    _tick = 0;
    _scheduler->scheduleUpdate(this, 0, false);
}

void DevToolsImpl::pause()
{
    _director->pause();
}

void DevToolsImpl::resume()
{
    _director->resume();
}

void DevToolsImpl::nodeEntered(const Node* node)
{
    if (!_isEnabled) 
    {
        return;
    }

    EM_ASM({
        Module.cocosNodeEntered($0, $1, UTF8ToString($2), UTF8ToString($3));
    }, node, node->getParent(), node->getName().c_str(), node->getDescription().c_str());
}

void DevToolsImpl::nodeExited(const Node* node)
{
    if (!_isEnabled) 
    {
        return;
    }
    
    EM_ASM({
        Module.cocosNodeExited($0);
    }, node);
}

void DevToolsImpl::nodeNameChanged(const Node* node)
{
    if (!_isEnabled) 
    {
        return;
    }
    
    EM_ASM({
        Module.cocosNodeNameChanged($0, UTF8ToString($1), UTF8ToString($2));
    }, node, node->getName().c_str(), node->getDescription().c_str());
}

DevToolsImpl* DevToolsImpl::getInstance()
{
    static DevToolsImpl instance;
    return &instance;
}

extern "C"
{
    void cocos_ccdirector_pause()
    {
        DevToolsImpl::getInstance()->pause();
    }

    void cocos_ccdirector_resume()
    {
        DevToolsImpl::getInstance()->resume();
    }

    void cocos_ccdirector_step()
    {
        DevToolsImpl::getInstance()->step();
    }

    void cocos_ccnode_select(int intPtr)
    {
        DevToolsImpl::getInstance()->select(reinterpret_cast<Node*>(intPtr));
    }
}

#endif // CC_PLATFORM_EMSCRIPTEN
