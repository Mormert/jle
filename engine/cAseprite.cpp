// Copyright (c) 2023. Johan Lind

#include "cAseprite.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePath.h"
#include "jlePathDefines.h"
#include "jleQuadRendering.h"
#include "jleResource.h"

void cAseprite::start() {
    _aseprites.clear();
    for (auto &&path : _asepritePaths) {
        _aseprites.push_back(
            gCore->resources().loadResourceFromFile<jleAseprite>(jlePath{path}));
    }
}

void cAseprite::update(float dt) {
    if (_aseprites.empty() ||
        _currentlyActiveAseprite + 1 > _aseprites.size()) {
        return;
    }

    auto &&aseprite = _aseprites[_currentlyActiveAseprite];

    if (aseprite->frames.empty()) {
        return;
    }

    if (_animating) {
        _currentFrameTimeSpent += dt * 1000.f;
        if (_currentFrameTimeSpent >= _currentFrameDurationMs) {
            _currentFrame++;
            if (_currentFrame >= aseprite->frames.size()) {
                _currentFrame = 0;
            }
            _currentFrameTimeSpent = 0;
            _currentFrameDurationMs =
                aseprite->frames.at(_currentFrame).duration;
        }
    }

    if (_currentFrame >= aseprite->frames.size()) {
        _currentFrame = aseprite->frames.size() - 1;
    }
    const auto &frame = aseprite->frames.at(_currentFrame);

    auto &texture = aseprite->imageTexture;
    if (texture != nullptr) {
        texturedQuad quad{texture};
        quad.x = getTransform().getWorldPosition().x + _offsetX;
        quad.y = getTransform().getWorldPosition().y + _offsetY;
        quad.height = _height;
        quad.width = _width;
        quad.textureX = frame.frame.x + _textureX;
        quad.textureY = frame.frame.y + _textureY;
        quad.depth = getTransform().getWorldPosition().z;

        if (quad.texture.get()) {
            gCore->quadRendering().sendTexturedQuad(quad);
        }
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
    _aseprites.push_back(gCore->resources().loadResourceFromFile<jleAseprite>(jlePath{path}));
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

    const auto frames = _aseprites[_currentlyActiveAseprite]->frames.size();
    if (index < frames) {
        _currentFrameDurationMs = 0;
        _currentFrame = index;
        return;
    }
    LOGE << "Trying to set current frame outside bounds!";
}
