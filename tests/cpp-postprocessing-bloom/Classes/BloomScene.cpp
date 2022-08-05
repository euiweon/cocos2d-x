#include "BloomScene.h"

USING_NS_CC;

static const float weight[] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

Scene* BloomScene::scene()
{
    return BloomScene::create();
}

bool BloomScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    
    const Size& visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2& origin = Director::getInstance()->getVisibleOrigin();

    // nodes will be rendered into fbo
    Sprite* sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(Vec2(visibleSize / 2) + origin);
    sprite->setCameraMask((unsigned short)CameraFlag::USER1);
    this->addChild(sprite);
    
    // bind a full-screen fbo with a specific camera so it render what it sees into the fbo
    auto sizeInpixels = Director::getInstance()->getWinSizeInPixels();
    auto fbo = experimental::FrameBuffer::create(1, sizeInpixels.width, sizeInpixels.height);

    auto renderTarget = experimental::RenderTarget::create(sizeInpixels.width, sizeInpixels.height);
    auto depthStencilTarget = experimental::RenderTargetDepthStencil::create(sizeInpixels.width, sizeInpixels.height);
    fbo->attachRenderTarget(renderTarget);
    fbo->attachDepthStencilTarget(depthStencilTarget);
    fbo->setClearColor(Color4F(0, 0, 0, 1));
    
    auto camera = Camera::create();
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setFrameBufferObject(fbo);
    addChild(camera);

    // bloom the fbo
	composer = Composer::createWithTexture(fbo->getRenderTarget()->getTexture());
    composer->setPosition(Vec2(visibleSize / 2));
	composer->setScaleY(-1);
    addChild(composer);

	// controls
	addSlider(0.0f, 1.0f, 0.5f, Vec2(75.0f, visibleSize.height - 35.0f), thresholdSlider, thresholdLabel);
	addSlider(0.0f, 5.0f, 1.0f, Vec2(75.0f, visibleSize.height - 85.0f), exposureSlider, exposureLabel);
	addSlider(1.0f, 5.0f, 5.0f, Vec2(75.0f, visibleSize.height - 135.0f), sampleSlider, sampleLabel);
	this->scheduleUpdate();

    return true;
}

void BloomScene::addSlider(float min, float max, float defaultVal, const Vec2& pos, extension::ControlSlider*& slider, Label*& label)
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

void BloomScene::update(float delta)
{
	Scene::update(delta);

	char str[50];

	composer->setThreshold(thresholdSlider->getValue());
    sprintf(str, "Threshold: %.1f", thresholdSlider->getValue());
	thresholdLabel->setString(str);

    composer->setExposure(exposureSlider->getValue());
    sprintf(str, "Exposure: %.1f", exposureSlider->getValue());
	exposureLabel->setString(str);

    composer->setSampleCount((unsigned int)sampleSlider->getValue());
    sprintf(str, "Sample: %u", (unsigned int)sampleSlider->getValue());
	sampleLabel->setString(str);
}

Composer::Composer()
	: sampleCount(5)
{
}

Composer::~Composer()
{
}

bool Composer::initWithTexture(Texture2D* texture)
{
    Sprite::initWithTexture(texture);

    const Size& visibleSize = Director::getInstance()->getVisibleSize();
	
    // base sprite to extract brightness
    base = Sprite::createWithTexture(texture);
    base->setPosition(Vec2(visibleSize / 2));
	base->setScaleY(-1);
    auto shaderBase = GLProgram::createWithFilenames("pos_tex_color.vert", "bloom_extract_brightness.frag");
    auto stateBase = GLProgramState::create(shaderBase);
	stateBase->setUniformFloat("threshold", 0.5);
    base->setGLProgramState(stateBase);
    
    // offscreen ping-pong RT for Gaussian blur
    renderTexX = RenderTexture::create(visibleSize.width, visibleSize.height, Texture2D::PixelFormat::RGBA8888);
    renderTexY = RenderTexture::create(visibleSize.width, visibleSize.height, Texture2D::PixelFormat::RGBA8888);

    renderTexX->setPosition(Vec2(visibleSize / 2));
    renderTexY->setPosition(Vec2(visibleSize / 2));

    auto shader = GLProgram::createWithFilenames("pos_tex_color.vert", "gaussian_blur.frag");

    auto stateX = GLProgramState::create(shader);
    stateX->setUniformInt("horizontal", 1);
    stateX->setUniformFloatv("weight", 5, &weight[0]);
	stateX->setUniformVec2("tex_size", Vec2(visibleSize.width, visibleSize.height));
    renderTexX->getSprite()->setGLProgramState(stateX);

    auto stateY = GLProgramState::create(shader);
    stateY->setUniformInt("horizontal", 0);
    stateY->setUniformFloatv("weight", 5, &weight[0]);
	stateY->setUniformVec2("tex_size", Vec2(visibleSize.width, visibleSize.height));
    renderTexY->getSprite()->setGLProgramState(stateY);
 
    // set renderTexX as CC_Texture1
    texture->setAlphaTexture(renderTexX->getSprite()->getTexture());

    // blend texture and renderTexX
    auto shaderOutput = GLProgram::createWithFilenames("pos_tex_color.vert", "bloom_combine.frag");
	auto stateOutput = GLProgramState::create(shaderOutput);
	stateOutput->setUniformFloat("exposure", 1);
    setGLProgramState(stateOutput);
    return true;
}

Composer* Composer::createWithTexture(Texture2D* texture)
{
    Composer* composer = new Composer();
    composer->initWithTexture(texture);
    return composer;
}

void Composer::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    // extract brightness and render to renderTexX
    renderTexX->begin();
    base->visit();
    renderTexX->end();

    // Gaussian blur sample
    for (int i = 0; i < sampleCount; i++)
    {
        renderTexY->begin();
        renderTexX->Node::visit();
        renderTexY->end();
        renderer->render();

        renderTexX->begin();
        renderTexY->Node::visit();
        renderTexX->end();
        renderer->render();
    }

    // blend and output
	Sprite::visit(renderer, parentTransform, parentFlags);
}

void Composer::setThreshold(float threshold)
{
	base->getGLProgramState()->setUniformFloat("threshold", threshold);
}

void Composer::setExposure(float exposure)
{
	getGLProgramState()->setUniformFloat("exposure", exposure);
}
