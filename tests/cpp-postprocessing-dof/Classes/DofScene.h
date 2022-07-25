#ifndef __DOFSCENE_SCENE_H__
#define __DOFSCENE_SCENE_H__

#include "cocos2d.h"
#include "extensions/GUI/CCControlExtension/CCControlSlider.h"

class DofScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* scene();
    CREATE_FUNC(DofScene);
    virtual bool init() override;
    void update(float) override;

private:
    void addSlider(float min, float max, float defaultValue, const cocos2d::Vec2& pos, cocos2d::extension::ControlSlider*&, cocos2d::Label*&);

    cocos2d::Label* focusLabel;
    cocos2d::extension::ControlSlider* focusSlider;

    cocos2d::Label* apertureLabel;
    cocos2d::extension::ControlSlider* apertureSlider;

    cocos2d::Label* maxblurLabel;
    cocos2d::extension::ControlSlider* maxblurSlider;

    cocos2d::Sprite* composer;
};

#endif // __DOFSCENE_SCENE_H__
