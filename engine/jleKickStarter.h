#pragma once

#include "jleGameEngine.h"
#include "jleEditor.h"

namespace jle
{
	template <typename T>
	void KickStartGame()
	{
		static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

		auto gameSettings = std::make_shared<jleGameSettings>();
		T::OverrideGameSettings(*gameSettings);

		auto gameEngine = std::make_unique<jleGameEngine>(gameSettings);
		gameEngine->SetGame<T>();
		gameEngine->Run();
	}

	template <typename T>
	void KickStartGameInEditor()
	{
		static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

		auto gameEditorSettings = std::make_shared<jleEditorSettings>();
		T::OverrideGameSettings(*gameEditorSettings);
		T::OverrideGameEditorSettings(*gameEditorSettings);

		auto gameEngineInEditor = std::make_unique<jleEditor>(gameEditorSettings);
		gameEngineInEditor->SetGame<T>();
		gameEngineInEditor->Run();
	}

	template <typename T>
	void KickStart()
	{
		static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

#ifndef NDEBUG
		KickStartGameInEditor<T>();
#else
		KickStartGame<T>();
#endif
	}
}