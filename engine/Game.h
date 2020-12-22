#pragma once

#include <memory>

#include "Renderer2D.h"

class Game
{
public:

	// Called from engine
	virtual void Start() = 0;
	virtual void Update(float dt) = 0;

	Renderer2D& GetRenderer() const { return *renderer2D.get(); }

private:
	// Set from engine
	std::shared_ptr<Renderer2D> renderer2D;
	
	friend class Engine;

};