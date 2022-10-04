// Copyright (c) 2022. Johan Lind

#include "cSpritesheet.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jleQuadRendering.h"
#include "jleResourceHolder.h"
#include "json.hpp"
#include <plog/Log.h>

void cSpritesheet::Start() {
    _transform = _attachedToObject->AddDependencyComponent<cTransform>(this);
}

cSpritesheet::cSpritesheet(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

void cSpritesheet::Update(float dt) {
    if (!_spritesheet) {
        return;
    }

    auto& texture = _spritesheet->_imageTexture;
    if (texture != nullptr && _hasEntity) {
        TexturedQuad quad{texture};
        quad.x = _transform->GetWorldX() +
                 _spritesheetEntityCache._sourceSize._x - _offset.x;
        quad.y = _transform->GetWorldY() +
                 _spritesheetEntityCache._sourceSize._y - _offset.y;

        quad.height = _spritesheetEntityCache._frame._height;
        quad.width = _spritesheetEntityCache._frame._width;
        quad.textureX = _spritesheetEntityCache._frame._x;
        quad.textureY = _spritesheetEntityCache._frame._y;
        quad.depth = _transform->GetWorldDepth();

        if (quad.texture.get()) {
            jleCore::core->rendering->quads().SendTexturedQuad(
                quad, RenderingMethod::Dynamic);
        }
    }
}

void cSpritesheet::ToJson(nlohmann::json& j_out) {
    j_out["_spritesheetPath"] = _spritesheetPath;
    j_out["_spriteName"] = _spriteName;
    j_out["_offsetX"] = _offset.x;
    j_out["_offsetY"] = _offset.y;
}

void cSpritesheet::FromJson(const nlohmann::json& j_in) {
    _spritesheetPath = j_in["_spritesheetPath"];
    _spriteName = j_in["_spriteName"];
    _offset.x = j_in.value("_offsetX", 0);
    _offset.y = j_in.value("_offsetY", 0);
    _spritesheet = jleResourceHolder::LoadResourceFromFile<jleSpritesheet>(
        jleRelativePath{_spritesheetPath});

    SetEntity(_spriteName);
}

void cSpritesheet::SetEntity(const std::string& entityName) {
    if (!_spritesheet) {
        return;
    }

    _spriteName = entityName;
    const auto entity = _spritesheet->_spritesheetEntities.find(_spriteName);
    if (entity != _spritesheet->_spritesheetEntities.end()) {
        _spritesheetEntityCache = entity->second;
        _hasEntity = true;
    }
    else {
        LOGW << "Could not find sprite entity on the spritesheet: "
             << _spriteName;
    }
}
