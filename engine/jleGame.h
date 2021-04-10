#pragma once

namespace jle
{
	class jleGame
	{
	public:
		virtual ~jleGame(){}
		virtual void Update(float dt);
		virtual void Start();
	};
}