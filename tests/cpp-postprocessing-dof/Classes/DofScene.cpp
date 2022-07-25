#include "DofScene.h"

USING_NS_CC;

Scene* DofScene::scene()
{
    return DofScene::create();
}

bool DofScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    for (int x = -5; x <= 5; x++)
    {
        for (int y = -5; y <= 5; y ++)
        {
            for (int z = -5; z <= 5; z++)
            {
                auto cylinder = Sprite3D::create("cylinder.c3b","cylinder.png");
                cylinder->setCameraMask((unsigned short)CameraFlag::USER1);
                cylinder->setPosition3D(Vec3(x * 50, y * 50, z * 50));
                addChild(cylinder);
            }
        }
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto sizeInPixels = Director::getInstance()->getWinSizeInPixels();

    // bind a full-screen fbo with a specific camera so it render what it sees into the fbo
    auto fbo = experimental::FrameBuffer::create(1, sizeInPixels.width, sizeInPixels.height);
    auto renderTarget = experimental::RenderTarget::create(sizeInPixels.width, sizeInPixels.height);
    auto depthStencilTarget = experimental::RenderTarget::create(sizeInPixels.width, sizeInPixels.height, Texture2D::PixelFormat::D24S8);
    fbo->attachRenderTarget(renderTarget);
    fbo->attachDepthStencilTarget(depthStencilTarget);
    
    auto camera = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 10.0f, 1000);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setFrameBufferObject(fbo);
    addChild(camera);

    // depth of field shader
    renderTarget->getTexture()->setAlphaTexture(depthStencilTarget->getTexture());
    composer = Sprite::createWithTexture(renderTarget->getTexture());
    composer->setPosition(Vec2(visibleSize / 2));
    composer->setScaleY(-1);

    auto shaderOutput = GLProgram::createWithFilenames("pos_tex_color.vert", "depth_of_field.frag");
    auto stateOutput = GLProgramState::create(shaderOutput);
    stateOutput->setUniformFloat("maxblur",  0.01f);
    stateOutput->setUniformFloat("focus", 25.0f);
    stateOutput->setUniformFloat("aspect", visibleSize.width / visibleSize.height);
    stateOutput->setUniformFloat("aperture", 0.001f);
    stateOutput->setUniformFloat("nearClip", 10.0f);
    stateOutput->setUniformFloat("farClip", 1000.0f);
    composer->setGLProgramState(stateOutput);

    addChild(composer);

    // controls
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), 150, 160);
    colorLayer->setPosition(Vec2(2.0f, visibleSize.height - 170.0f));
    addChild(colorLayer);
    addSlider(10.0f, 300.0f, 25.0f, Vec2(75.0f, visibleSize.height - 35.0f), focusSlider, focusLabel);
    addSlider(0.0f, 0.001, 0.001f, Vec2(75.0f, visibleSize.height - 85.0f), apertureSlider, apertureLabel);
    addSlider(0.0f, 0.01f, 0.01f, Vec2(75.0f, visibleSize.height - 135.0f), maxblurSlider, maxblurLabel);

    scheduleUpdate();
    return true;
}

void DofScene::addSlider(float min, float max, float defaultVal, const Vec2& pos, extension::ControlSlider*& slider, Label*& label)
{
    label = Label::createWithSystemFont("", "Arial", 16);
    label->setPosition(Vec2(pos.x - 61, pos.y + 5));
    label->setAnchorPoint(Vec2(0, 0.5));
    addChild(label);

    slider = extension::ControlSlider::create("sliderTrack.png", "sliderProgress.png", "sliderThumb.png");
    slider->setMinimumValue(min);
    slider->setMaximumValue(max);
    slider->setValue(defaultVal);
    slider->setPosition(Vec2(pos.x, pos.y - 12.5));
    addChild(slider);
}

void DofScene::update(float delta)
{
    Scene::update(delta);

    char str[50];
    auto state = composer->getGLProgramState();

    state->setUniformFloat("focus", focusSlider->getValue());
    sprintf(str, "focus: %.1f", focusSlider->getValue());
    focusLabel->setString(str);

    state->setUniformFloat("aperture", apertureSlider->getValue());
    sprintf(str, "aperture: %.4f", apertureSlider->getValue());
    apertureLabel->setString(str);

    state->setUniformFloat("maxblur", maxblurSlider->getValue());
    sprintf(str, "maxblur: %.3f", maxblurSlider->getValue());
    maxblurLabel->setString(str);
}
