#include "Repeat9Sprite.h"

NS_CC_EXT_BEGIN

Repeat9Sprite::Repeat9Sprite()
: _spritesGenerated(false)
, _spriteFrameRotated(false)
, _positionsAreDirty(false)
, _scale9Image(nullptr)
, _topLeft(nullptr)
, _top(nullptr)
, _topRight(nullptr)
, _left(nullptr)
, _centre(nullptr)
, _right(nullptr)
, _bottomLeft(nullptr)
, _bottom(nullptr)
, _bottomRight(nullptr)
, _opacityModifyRGB(false)
, _insetLeft(0)
, _insetTop(0)
, _insetRight(0)
, _insetBottom(0)
,_leftMode(RepeatEnum::RepeatNone)
,_topMode(RepeatEnum::RepeatNone)
,_rightMode(RepeatEnum::RepeatNone)
,_bottomMode(RepeatEnum::RepeatNone)
{
    
}

Repeat9Sprite::~Repeat9Sprite()
{
    CC_SAFE_RELEASE(_topLeft);
    CC_SAFE_RELEASE(_top);
    CC_SAFE_RELEASE(_topRight);
    CC_SAFE_RELEASE(_left);
    CC_SAFE_RELEASE(_centre);
    CC_SAFE_RELEASE(_right);
    CC_SAFE_RELEASE(_bottomLeft);
    CC_SAFE_RELEASE(_bottom);
    CC_SAFE_RELEASE(_bottomRight);
    CC_SAFE_RELEASE(_scale9Image);
}

bool Repeat9Sprite::isRepeatEdge(RepeatEdge edge)
{
    switch (edge) {
        case RepeatEdge::Left:
            return _leftMode != RepeatEnum::RepeatNone;
        case RepeatEdge::Right:
            return _rightMode != RepeatEnum::RepeatNone;
        case RepeatEdge::Top:
            return _topMode != RepeatEnum::RepeatNone;
        case RepeatEdge::Bottom:
            return _bottomMode != RepeatEnum::RepeatNone;
        default:
            return false;
    }
}

void Repeat9Sprite::setRepeatMode(RepeatEnum mode)
{
    bool v = false;
    bool h = false;
    switch (mode) {
        case RepeatEnum::RepeatAll:
            v = true;
            h = true;
            break;
        case RepeatEnum::RepeatVertical:
            v = true;
            h = false;
            break;
        case RepeatEnum::RepeatHorizon:
            v = false;
            h = true;
            break;
        default:
            break;
    }

    this->setRepeatModeToEdge(v, RepeatEdge::Left);
    this->setRepeatModeToEdge(v, RepeatEdge::Right);
    this->setRepeatModeToEdge(h, RepeatEdge::Top);
    this->setRepeatModeToEdge(h, RepeatEdge::Bottom);
}

void Repeat9Sprite::setRepeatModeToEdge(bool repeat,RepeatEdge edge)
{
    switch (edge) {
        case RepeatEdge::Left:
            _leftMode = repeat ? RepeatEnum::RepeatVertical : RepeatEnum::RepeatNone;
            this->_left->setRepeatMode(_leftMode);
            break;
        case RepeatEdge::Right:
            _rightMode = repeat ? RepeatEnum::RepeatVertical : RepeatEnum::RepeatNone;
            this->_right->setRepeatMode(_rightMode);
            break;
        case RepeatEdge::Top:
            _topMode = repeat ? RepeatEnum::RepeatHorizon : RepeatEnum::RepeatNone;
            this->_top->setRepeatMode(_topMode);
            break;
        case RepeatEdge::Bottom:
            _bottomMode = repeat ? RepeatEnum::RepeatHorizon : RepeatEnum::RepeatNone;
            this->_bottom->setRepeatMode(_bottomMode);
            break;
        default:
            break;
    }
}

bool Repeat9Sprite::init()
{
    return this->initWithBatchNode(nullptr, Rect::ZERO, Rect::ZERO);
}

bool Repeat9Sprite::initWithBatchNode(SpriteBatchNode* batchnode, const Rect& rect, const Rect& capInsets)
{
    return this->initWithBatchNode(batchnode, rect, false, capInsets);
}

bool Repeat9Sprite::initWithBatchNode(SpriteBatchNode* batchnode, const Rect& rect, bool rotated, const Rect& capInsets)
{
    if(batchnode)
    {
        this->updateWithBatchNode(batchnode, rect, rotated, capInsets);
    }
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->_positionsAreDirty = true;
    
    return true;
}

#define    TRANSLATE_X(x, y, xtranslate) \
x+=xtranslate;                       \

#define    TRANSLATE_Y(x, y, ytranslate) \
y+=ytranslate;                       \

bool Repeat9Sprite::updateWithBatchNode(SpriteBatchNode* batchnode, const Rect& originalRect, bool rotated, const Rect& capInsets)
{
    GLubyte opacity = getOpacity();
    Color3B color = getColor();
    Rect rect(originalRect);
    
    // Release old sprites
    this->removeAllChildrenWithCleanup(true);
    
    CC_SAFE_RELEASE(this->_centre);
    CC_SAFE_RELEASE(this->_top);
    CC_SAFE_RELEASE(this->_topLeft);
    CC_SAFE_RELEASE(this->_topRight);
    CC_SAFE_RELEASE(this->_left);
    CC_SAFE_RELEASE(this->_right);
    CC_SAFE_RELEASE(this->_bottomLeft);
    CC_SAFE_RELEASE(this->_bottom);
    CC_SAFE_RELEASE(this->_bottomRight);
    
    
    if(this->_scale9Image != batchnode)
    {
        CC_SAFE_RELEASE(this->_scale9Image);
        _scale9Image = batchnode;
        CC_SAFE_RETAIN(_scale9Image);
    }
    
    if (!_scale9Image)
    {
        return false;
    }
    
    _scale9Image->removeAllChildrenWithCleanup(true);
    
    _capInsets = capInsets;
    _spriteFrameRotated = rotated;
    
    // If there is no given rect
    if ( rect.equals(Rect::ZERO) )
    {
        // Get the texture size as original
        Size textureSize = _scale9Image->getTextureAtlas()->getTexture()->getContentSize();
        
        rect = Rect(0, 0, textureSize.width, textureSize.height);
    }
    
    // Set the given rect's size as original size
    _spriteRect = rect;
    _originalSize = rect.size;
    _preferredSize = _originalSize;
    _capInsetsInternal = capInsets;
    
    float w = rect.size.width;
    float h = rect.size.height;
    
    // If there is no specified center region
    if ( _capInsetsInternal.equals(Rect::ZERO) )
    {
        // log("... cap insets not specified : using default cap insets ...");
        _capInsetsInternal = Rect(
                                    w - floorf(w/3) * 2,
                                    h - floorf(h/3) * 2,
                                    floorf(w/3),
                                    floorf(h/3)
                                  );
    }
    
    float left_w = _capInsetsInternal.origin.x;
    float center_w = _capInsetsInternal.size.width;
    float right_w = rect.size.width - (left_w + center_w);
    
    float top_h = _capInsetsInternal.origin.y;
    float center_h = _capInsetsInternal.size.height;
    float bottom_h = rect.size.height - (top_h + center_h);
    
    // calculate rects
    
    // ... top row
    float x = 0.0;
    float y = 0.0;
    
    // top left
    Rect lefttopbounds = Rect(x, y, left_w, top_h);
    
    // top center
    TRANSLATE_X(x, y, left_w);
    Rect centertopbounds = Rect(x, y, center_w, top_h);
    
    // top right
    TRANSLATE_X(x, y, center_w);
    Rect righttopbounds = Rect(x, y, right_w, top_h);
    
    // ... center row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);
    
    // center left
    Rect leftcenterbounds = Rect(x, y, left_w, center_h);
    
    // center center
    TRANSLATE_X(x, y, left_w);
    Rect centerbounds = Rect(x, y, center_w, center_h);
    
    // center right
    TRANSLATE_X(x, y, center_w);
    Rect rightcenterbounds = Rect(x, y, right_w, center_h);
    
    // ... bottom row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);
    TRANSLATE_Y(x, y, center_h);
    
    // bottom left
    Rect leftbottombounds = Rect(x, y, left_w, bottom_h);
    
    // bottom center
    TRANSLATE_X(x, y, left_w);
    Rect centerbottombounds = Rect(x, y, center_w, bottom_h);
    
    // bottom right
    TRANSLATE_X(x, y, center_w);
    Rect rightbottombounds = Rect(x, y, right_w, bottom_h);
    
    if (!rotated) {
        // log("!rotated");
        
        AffineTransform t = AffineTransform::IDENTITY;
        t = AffineTransformTranslate(t, rect.origin.x, rect.origin.y);
        
        centerbounds = RectApplyAffineTransform(centerbounds, t);
        rightbottombounds = RectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = RectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = RectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = RectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = RectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = RectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = RectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = RectApplyAffineTransform(centertopbounds, t);
        
        // Centre
        _centre = Sprite::createWithTexture(_scale9Image->getTexture(), centerbounds);
        _centre->retain();
        this->addChild(_centre, 0);
        
        
        // Top
        _top = RepeatNode::create(_leftMode, _scale9Image->getTexture(), centertopbounds);
        _top->retain();
        this->addChild(_top, 1);
        
        // Bottom
        _bottom = RepeatNode::create(_bottomMode, _scale9Image->getTexture(), centerbottombounds);
        _bottom->retain();
        this->addChild(_bottom, 1);
        
        // Left
        _left = RepeatNode::create(_leftMode, _scale9Image->getTexture(), leftcenterbounds);
        _left->retain();
        this->addChild(_left, 1);
        
        // Right
        _right = RepeatNode::create(_rightMode, _scale9Image->getTexture(), rightcenterbounds);
        _right->retain();
        this->addChild(_right, 1);
        
        // Top left
        _topLeft = Sprite::createWithTexture(_scale9Image->getTexture(), lefttopbounds);
        _topLeft->retain();
        this->addChild(_topLeft, 2);
        
        // Top right
        _topRight = Sprite::createWithTexture(_scale9Image->getTexture(), righttopbounds);
        _topRight->retain();
        this->addChild(_topRight, 2);
        
        // Bottom left
        _bottomLeft = Sprite::createWithTexture(_scale9Image->getTexture(), leftbottombounds);
        _bottomLeft->retain();
        this->addChild(_bottomLeft, 2);
        
        // Bottom right
        _bottomRight = Sprite::createWithTexture(_scale9Image->getTexture(), rightbottombounds);
        _bottomRight->retain();
        this->addChild(_bottomRight, 2);
    } else {
        // set up transformation of coordinates
        // to handle the case where the sprite is stored rotated
        // in the spritesheet
        // log("rotated");
        
        AffineTransform t = AffineTransform::IDENTITY;
        
        Rect rotatedcenterbounds = centerbounds;
        Rect rotatedrightbottombounds = rightbottombounds;
        Rect rotatedleftbottombounds = leftbottombounds;
        Rect rotatedrighttopbounds = righttopbounds;
        Rect rotatedlefttopbounds = lefttopbounds;
        Rect rotatedrightcenterbounds = rightcenterbounds;
        Rect rotatedleftcenterbounds = leftcenterbounds;
        Rect rotatedcenterbottombounds = centerbottombounds;
        Rect rotatedcentertopbounds = centertopbounds;
        
        t = AffineTransformTranslate(t, rect.size.height+rect.origin.x, rect.origin.y);
        t = AffineTransformRotate(t, 1.57079633f);
        
        centerbounds = RectApplyAffineTransform(centerbounds, t);
        rightbottombounds = RectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = RectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = RectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = RectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = RectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = RectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = RectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = RectApplyAffineTransform(centertopbounds, t);
        
        rotatedcenterbounds.origin = centerbounds.origin;
        rotatedrightbottombounds.origin = rightbottombounds.origin;
        rotatedleftbottombounds.origin = leftbottombounds.origin;
        rotatedrighttopbounds.origin = righttopbounds.origin;
        rotatedlefttopbounds.origin = lefttopbounds.origin;
        rotatedrightcenterbounds.origin = rightcenterbounds.origin;
        rotatedleftcenterbounds.origin = leftcenterbounds.origin;
        rotatedcenterbottombounds.origin = centerbottombounds.origin;
        rotatedcentertopbounds.origin = centertopbounds.origin;
        
        // Centre
        _centre = Sprite::createWithTexture(_scale9Image->getTexture(), rotatedcenterbounds, true);
        _centre->retain();
        this->addChild(_centre, 0);
        
        // Top
        _top = RepeatNode::create(_topMode, _scale9Image->getTexture(), rotatedcentertopbounds,true);
        _top->retain();
        this->addChild(_top, 1);
        
        // Bottom
        _bottom = RepeatNode::create(_bottomMode, _scale9Image->getTexture(), rotatedcenterbottombounds,true);
        _bottom->retain();
        this->addChild(_bottom, 1);
        
        // Left
        _left = RepeatNode::create(_leftMode, _scale9Image->getTexture(), rotatedleftcenterbounds,true);
        _left->retain();
        this->addChild(_left, 1);
        
        // Right
        _right = RepeatNode::create(_rightMode, _scale9Image->getTexture(), rotatedrightcenterbounds,true);
        _right->retain();
        this->addChild(_right, 1);
        
        // Top left
        _topLeft = Sprite::createWithTexture(_scale9Image->getTexture(), rotatedlefttopbounds, true);
        _topLeft->retain();
        this->addChild(_topLeft, 2);
        
        // Top right
        _topRight = Sprite::createWithTexture(_scale9Image->getTexture(), rotatedrighttopbounds, true);
        _topRight->retain();
        this->addChild(_topRight, 2);
        
        // Bottom left
        _bottomLeft = Sprite::createWithTexture(_scale9Image->getTexture(), rotatedleftbottombounds, true);
        _bottomLeft->retain();
        this->addChild(_bottomLeft, 2);
        
        // Bottom right
        _bottomRight = Sprite::createWithTexture(_scale9Image->getTexture(), rotatedrightbottombounds, true);
        _bottomRight->retain();
        this->addChild(_bottomRight, 2);
    }
    
    this->setContentSize(rect.size);
    this->addChild(_scale9Image);
    
    if (_spritesGenerated)
    {
        // Restore color and opacity
        this->setOpacity(opacity);
        this->setColor(color);
    }
    _spritesGenerated = true;
    return true;
}

void Repeat9Sprite::setContentSize(const Size &size)
{
    Node::setContentSize(size);
    this->_positionsAreDirty = true;
}

void Repeat9Sprite::updatePositions()
{
    // Check that instances are non-nullptr
    if(!((_topLeft) &&
         (_topRight) &&
         (_bottomRight) &&
         (_bottomLeft) &&
         (_centre))) {
        // if any of the above sprites are nullptr, return
        return;
    }
    
    Size size = this->_contentSize;
    
    float sizableWidth = size.width - _topLeft->getContentSize().width - _topRight->getContentSize().width;
    float sizableHeight = size.height - _topLeft->getContentSize().height - _bottomRight->getContentSize().height;
    float horizontalScale = sizableWidth/_centre->getContentSize().width;
    float verticalScale = sizableHeight/_centre->getContentSize().height;
    
    _centre->setScaleX(horizontalScale);
    _centre->setScaleY(verticalScale);
    
    float rescaledWidth = _centre->getContentSize().width * horizontalScale;
    float rescaledHeight = _centre->getContentSize().height * verticalScale;
    
    float leftWidth = _bottomLeft->getContentSize().width;
    float bottomHeight = _bottomLeft->getContentSize().height;
    
    _bottomLeft->setAnchorPoint(Vec2(0,0));
    _bottomRight->setAnchorPoint(Vec2(0,0));
    _topLeft->setAnchorPoint(Vec2(0,0));
    _topRight->setAnchorPoint(Vec2(0,0));
    _left->setAnchorPoint(Vec2(0,0));
    _right->setAnchorPoint(Vec2(0,0));
    _top->setAnchorPoint(Vec2(0,0));
    _bottom->setAnchorPoint(Vec2(0,0));
    _centre->setAnchorPoint(Vec2(0,0));
    
    // Position corners
    _bottomLeft->setPosition(Vec2(0,0));
    _bottomRight->setPosition(Vec2(leftWidth+rescaledWidth,0));
    _topLeft->setPosition(Vec2(0, bottomHeight+rescaledHeight));
    _topRight->setPosition(Vec2(leftWidth+rescaledWidth, bottomHeight+rescaledHeight));
    
    // Scale and position borders
    _left->setPosition(Vec2(0, bottomHeight));
    _left->setContentSize(Size(_bottomLeft->getContentSize().width, sizableHeight));
    
    _right->setPosition(Vec2(leftWidth+rescaledWidth,bottomHeight));
    _right->setContentSize(Size(_bottomRight->getContentSize().width, sizableHeight));
    
    _bottom->setPosition(Vec2(leftWidth,0));
    _bottom->setContentSize(Size(sizableWidth, _bottomLeft->getContentSize().height));
    
    _top->setPosition(Vec2(leftWidth,bottomHeight+rescaledHeight));
    _top->setContentSize(Size(sizableWidth, _topLeft->getContentSize().height));
    
    // Position centre
    _centre->setPosition(Vec2(leftWidth, bottomHeight));
}

bool Repeat9Sprite::initWithFile(const std::string& file, const Rect& rect,  const Rect& capInsets)
{
    SpriteBatchNode *batchnode = SpriteBatchNode::create(file, 9);
    bool pReturn = this->initWithBatchNode(batchnode, rect, capInsets);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::create(const std::string& file, const Rect& rect,  const Rect& capInsets)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

bool Repeat9Sprite::initWithFile(const std::string& file, const Rect& rect)
{
    bool pReturn = this->initWithFile(file, rect, Rect::ZERO);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::create(const std::string& file, const Rect& rect)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}


bool Repeat9Sprite::initWithFile(const Rect& capInsets, const std::string& file)
{
    bool pReturn = this->initWithFile(file, Rect::ZERO, capInsets);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::create(const Rect& capInsets, const std::string& file)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithFile(capInsets, file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

bool Repeat9Sprite::initWithFile(const std::string& file)
{
    bool pReturn = this->initWithFile(file, Rect::ZERO);
    return pReturn;
    
}

Repeat9Sprite* Repeat9Sprite::create(const std::string& file)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithFile(file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

bool Repeat9Sprite::initWithSpriteFrame(SpriteFrame* spriteFrame, const Rect& capInsets)
{
    Texture2D* texture = spriteFrame->getTexture();
    CCASSERT(texture != nullptr, "CCTexture must be not nil");
    
    SpriteBatchNode *batchnode = SpriteBatchNode::createWithTexture(texture, 9);
    CCASSERT(batchnode != nullptr, "CCSpriteBatchNode must be not nil");
    
    bool pReturn = this->initWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), capInsets);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::createWithSpriteFrame(SpriteFrame* spriteFrame, const Rect& capInsets)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}
bool Repeat9Sprite::initWithSpriteFrame(SpriteFrame* spriteFrame)
{
    CCASSERT(spriteFrame != nullptr, "Invalid spriteFrame for sprite");
    bool pReturn = this->initWithSpriteFrame(spriteFrame, Rect::ZERO);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::createWithSpriteFrame(SpriteFrame* spriteFrame)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

bool Repeat9Sprite::initWithSpriteFrameName(const std::string& spriteFrameName, const Rect& capInsets)
{
    CCASSERT((SpriteFrameCache::getInstance()) != nullptr, "SpriteFrameCache::getInstance() must be non-nullptr");
    
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    CCASSERT(frame != nullptr, "CCSpriteFrame must be non-nullptr");
    
    if (nullptr == frame) return false;
    
    bool pReturn = this->initWithSpriteFrame(frame, capInsets);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::createWithSpriteFrameName(const std::string& spriteFrameName, const Rect& capInsets)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

bool Repeat9Sprite::initWithSpriteFrameName(const std::string& spriteFrameName)
{
    bool pReturn = this->initWithSpriteFrameName(spriteFrameName, Rect::ZERO);
    return pReturn;
}

Repeat9Sprite* Repeat9Sprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    
    log("Could not allocate Repeat9Sprite()");
    return nullptr;
    
}

Repeat9Sprite* Repeat9Sprite::resizableSpriteWithCapInsets(const Rect& capInsets)
{
    Repeat9Sprite* pReturn = new Repeat9Sprite();
    if ( pReturn && pReturn->initWithBatchNode(_scale9Image, _spriteRect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

Repeat9Sprite* Repeat9Sprite::create()
{
    Repeat9Sprite *pReturn = new Repeat9Sprite();
    if (pReturn && pReturn->init())
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return nullptr;
}

/** sets the opacity.
 @warning If the the texture has premultiplied alpha then, the R, G and B channels will be modifed.
 Values goes from 0 to 255, where 255 means fully opaque.
 */

void Repeat9Sprite::setPreferredSize(Size preferedSize)
{
    this->setContentSize(preferedSize);
    this->_preferredSize = preferedSize;
}

Size Repeat9Sprite::getPreferredSize()
{
    return this->_preferredSize;
}

void Repeat9Sprite::setCapInsets(Rect capInsets)
{
    Size contentSize = this->_contentSize;
    this->updateWithBatchNode(this->_scale9Image, this->_spriteRect, _spriteFrameRotated, capInsets);
    this->setContentSize(contentSize);
}

Rect Repeat9Sprite::getCapInsets()
{
    return _capInsets;
}

void Repeat9Sprite::updateCapInset()
{
    Rect insets;
    if (this->_insetLeft == 0 && this->_insetTop == 0 && this->_insetRight == 0 && this->_insetBottom == 0)
    {
        insets = Rect::ZERO;
    }
    else
    {
        insets = Rect(_insetLeft,
                      _insetTop,
                      _spriteRect.size.width-_insetLeft-_insetRight,
                      _spriteRect.size.height-_insetTop-_insetBottom);
    }
    this->setCapInsets(insets);
}

void Repeat9Sprite::setOpacityModifyRGB(bool var)
{
    if (!_scale9Image)
    {
        return;
    }
    _opacityModifyRGB = var;
    
    for(auto child : _scale9Image->getChildren()){
        child->setOpacityModifyRGB(_opacityModifyRGB);
    }
}

bool Repeat9Sprite::isOpacityModifyRGB() const
{
    return _opacityModifyRGB;
}

void Repeat9Sprite::setSpriteFrame(SpriteFrame * spriteFrame)
{
    SpriteBatchNode * batchnode = SpriteBatchNode::createWithTexture(spriteFrame->getTexture(), 9);
    this->updateWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), Rect::ZERO);
    
    // Reset insets
    this->_insetLeft = 0;
    this->_insetTop = 0;
    this->_insetRight = 0;
    this->_insetBottom = 0;
}

float Repeat9Sprite::getInsetLeft()
{
    return this->_insetLeft;
}

float Repeat9Sprite::getInsetTop()
{
    return this->_insetTop;
}

float Repeat9Sprite::getInsetRight()
{
    return this->_insetRight;
}

float Repeat9Sprite::getInsetBottom()
{
    return this->_insetBottom;
}

void Repeat9Sprite::setInsetLeft(float insetLeft)
{
    this->_insetLeft = insetLeft;
    this->updateCapInset();
}

void Repeat9Sprite::setInsetTop(float insetTop)
{
    this->_insetTop = insetTop;
    this->updateCapInset();
}

void Repeat9Sprite::setInsetRight(float insetRight)
{
    this->_insetRight = insetRight;
    this->updateCapInset();
}

void Repeat9Sprite::setInsetBottom(float insetBottom)
{
    this->_insetBottom = insetBottom;
    this->updateCapInset();
}

void Repeat9Sprite::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if(this->_positionsAreDirty)
    {
        this->updatePositions();
        this->_positionsAreDirty = false;
    }
    Node::visit(renderer, parentTransform, parentFlags);
}

void Repeat9Sprite::setColor(const Color3B& color)
{
    if (!_scale9Image)
    {
        return;
    }
    
    Node::setColor(color);
    
    for(auto child : _scale9Image->getChildren()){
        child->setColor(color);
    }
}

void Repeat9Sprite::setOpacity(GLubyte opacity)
{
    if (!_scale9Image)
    {
        return;
    }
    Node::setOpacity(opacity);
    
    for(auto child : _scale9Image->getChildren()){
        child->setOpacity(opacity);
    }
}

void Repeat9Sprite::updateDisplayedColor(const cocos2d::Color3B &parentColor)
{
    if (!_scale9Image)
    {
        return;
    }
    Node::updateDisplayedColor(parentColor);
    
    for(auto child : _scale9Image->getChildren()){
        child->updateDisplayedColor(parentColor);
    }
}

void Repeat9Sprite::updateDisplayedOpacity(GLubyte parentOpacity)
{
    if (!_scale9Image)
    {
        return;
    }
    Node::updateDisplayedOpacity(parentOpacity);
    
    for(auto child : _scale9Image->getChildren()){
        child->updateDisplayedOpacity(parentOpacity);
    }
}

NS_CC_EXT_END