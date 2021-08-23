#include "ProfilerWindow.h"

#include <ctime>

jle::ProfilerWindow::ProfilerWindow(const std::string& window_name) 
	: iEditorImGuiWindow{ window_name }
{
}

void jle::ProfilerWindow::Update(jleGameEngine& ge)
{
	mProfilersWindow.Render();
}
