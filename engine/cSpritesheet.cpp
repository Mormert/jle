// Copyright (c) 2022. Johan Lind

#include "cSpritesheet.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jlePathDefines.h"
#include "jleQuadRendering_OpenGL.h"
#include "jleResourceHolder.h"

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
        quad.x = _transform->GetWorldX();
        quad.y = _transform->GetWorldY();

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
}

void cSpritesheet::FromJson(const nlohmann::json& j_in) {
    _spritesheetPath = j_in["_spritesheetPath"];
    _spriteName = j_in["_spriteName"];
    _spritesheet = jleResourceHolder::LoadResourceFromFile<jleSpritesheet>(
        jleRelativePath{_spritesheetPath});

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
