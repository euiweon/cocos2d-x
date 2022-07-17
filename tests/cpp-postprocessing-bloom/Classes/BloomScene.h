#ifndef __BLOOMSCENE_SCENE_H__
#define __BLOOMSCENE_SCENE_H__

#include "cocos2d.h"
#include "extensions/GUI/CCControlExtension/CCControlSlider.h"

class Composer : public cocos2d::Sprite
{
public:
    Composer();
    virtual ~Composer();
    static Composer* createWithTexture(cocos2d::Texture2D*);
    bool initWithTexture(cocos2d::Texture2D*) override;
    void setThreshold(float threshold);
    void setExposure(float exposure);
    void setSampleCount(unsigned int count) { sampleCount = count; }

protected:
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;

private:
    cocos2d::RenderTexture* renderTexX;
    cocos2d::RenderTexture* renderTexY;
    cocos2d::Sprite* base;
    unsigned int sampleCount;
};

class BloomScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* scene();
    CREATE_FUNC(BloomScene);
    virtual bool init() override;
    void update(float) override;

private:
    void addSlider(float min, float max, float defaultValue, const cocos2d::Vec2& pos, cocos2d::extension::ControlSlider*&, cocos2d::Label*&);

    cocos2d::Label* thresholdLabel;
    cocos2d::extension::ControlSlider* thresholdSlider;

    cocos2d::Label* exposureLabel;
    cocos2d::extension::ControlSlider* exposureSlider;

    cocos2d::Label* sampleLabel;
    cocos2d::extension::ControlSlider* sampleSlider;

    Composer* composer;
};

#endif // __BLOOMSCENE_SCENE_H__
