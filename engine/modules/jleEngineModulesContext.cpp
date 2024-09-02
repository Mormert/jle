#include "jleEngineModulesContext.h"

jleEngineModulesContext::jleEngineModulesContext(jleGameRuntime &gameRuntime,
                                                 jle3DRenderer &renderer,
                                                 jleRenderThread &renderThread,
                                                 jle3DSettings &renderSettings_,
                                                 jle3DGraph &renderGraph_,
                                                 jleEngineSettings &engineSettings,
                                                 jleInput &input,
                                                 jleLuaEnvironment &luaEnvironment_,
                                                 jleWindow &window,
                                                 jleResources &resources,
                                                 jleFrameInfo &info)
    : gameRuntime(gameRuntime), rendererModule(renderer), renderThread(renderThread), renderSettings(renderSettings_), renderGraph(renderGraph_),
      settings(engineSettings), inputModule(input), luaEnvironment(luaEnvironment_), windowModule(window),
      resourcesModule(resources), frameInfo(info)
{
}
