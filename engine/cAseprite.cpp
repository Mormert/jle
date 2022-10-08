// Copyright (c) 2022. Johan Lind

#include "cAseprite.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePath.h"
#include "jlePathDefines.h"
#include "jleQuadRendering.h"
#include "jleResource.h"

void cAseprite::start() {
    _transform = _attachedToObject->addDependencyComponent<cTransform>(this);

    _aseprites.clear();
    for (auto &&path : _asepritePaths) {
        _aseprites.push_back(
            gCore->resources().loadResourceFromFile<jleAseprite>(
                jleRelativePath{path._string}));
    }
}

void cAseprite::update(float dt) {
    if (_aseprites.empty() ||
        _currentlyActiveAseprite + 1 > _aseprites.size()) {
        return;
    }

    auto &&aseprite = _aseprites[_currentlyActiveAseprite];

    if (aseprite->_frames.empty()) {
        return;
    }

    if (_animating) {
        _currentFrameTimeSpent += dt * 1000.f;
        if (_currentFrameTimeSpent >= _currentFrameDurationMs) {
            _currentFrame++;
            if (_currentFrame >= aseprite->_frames.size()) {
                _currentFrame = 0;
            }
            _currentFrameTimeSpent = 0;
            _currentFrameDurationMs =
                aseprite->_frames.at(_currentFrame)._duration;
        }
    }

    if (_currentFrame >= aseprite->_frames.size()) {
        _currentFrame = aseprite->_frames.size() - 1;
    }
    const auto &frame = aseprite->_frames.at(_currentFrame);

    auto &texture = aseprite->_imageTexture;
    if (texture != nullptr) {
        texturedQuad quad{texture};
        quad.x = _transform->worldX() + _offsetX;
        quad.y = _transform->worldY() + _offsetY;
        quad.height = _height;
        quad.width = _width;
        quad.textureX = frame._frame._x + _textureX;
        quad.textureY = frame._frame._y + _textureY;
        quad.depth = _transform->worldDepth();

        if (quad.texture.get()) {
            gCore->quadRendering().sendTexturedQuad(quad);
        }
    }
}

void cAseprite::toJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{{"_asepritePaths_STRVEC", _asepritePaths},
                           {"height", _height},
                           {"width", _width},
                           {"textureX", _textureX},
                           {"textureY", _textureY},
                           {"offsetX", _offsetX},
                           {"offsetY", _offsetY},
                           {"animating", _animating}};
}

void cAseprite::fromJson(const nlohmann::json &j_in) {

    const auto asepritePaths = j_in.find("_asepritePaths_STRVEC");
    if (asepritePaths != j_in.end()) {
        _asepritePaths =
            j_in.at("_asepritePaths_STRVEC").get<std::vector<jleJsonString>>();
    }

    _height = j_in.at("height");
    _width = j_in.at("width");
    _textureX = j_in.at("textureX");
    _textureY = j_in.at("textureY");
    _offsetX = j_in.at("offsetX");
    _offsetY = j_in.at("offsetY");
    _animating = j_in.at("animating");

    // Make sure to reset current frame to not cause out of bounds crash
    _currentFrame = 0;
    _currentFrameTimeSpent = 0.f;
    _currentlyActiveAseprite = 0;

    _aseprites.clear();
    for (auto &&path : _asepritePaths) {
        _aseprites.push_back(
            gCore->resources().loadResourceFromFile<jleAseprite>(
                jleRelativePath{path._string}));
    }
}

cAseprite::cAseprite(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

void cAseprite::currentAseprite(unsigned int index) {
    if (index < _aseprites.size()) {
        _currentlyActiveAseprite = index;
        return;
    }
    LOGE << "Trying to set active aseprite animation outside bounds!";
}

unsigned int cAseprite::currentAsepriteIndex() const {
    return _currentlyActiveAseprite;
}

int cAseprite::addAsepritePath(const std::string &path) {
    _asepritePaths.push_back({path});
    _aseprites.push_back(gCore->resources().loadResourceFromFile<jleAseprite>(
        jleRelativePath{path}));
    return (int)_aseprites.size() - 1;
}

jleAseprite &cAseprite::activeAsepriteRef() {
    return *_aseprites[_currentlyActiveAseprite];
}

void cAseprite::currentAsepriteFrame(unsigned int index) {
    if (_aseprites.empty()) {
        LOGE << "No found aseprites on this object";
        return;
    }

    const auto frames = _aseprites[_currentlyActiveAseprite]->_frames.size();
    if (index < frames) {
        _currentFrameDurationMs = 0;
        _currentFrame = index;
        return;
    }
    LOGE << "Trying to set current frame outside bounds!";
}
