#include "jleEngineModulesContext.h"

jleEngineModulesContext::jleEngineModulesContext(jle3DRenderer &renderer,
                                                 jle3DSettings &renderSettings_,
                                                 jle3DGraph &renderGraph_,
                                                 jleInput &input,
                                                 jleLuaEnvironment &luaEnvironment_,
                                                 jleWindow &window,
                                                 jleResources &resources,
                                                 jleFrameInfo &info)
    : rendererModule(renderer), renderSettings(renderSettings_), renderGraph(renderGraph_), inputModule(input),
      luaEnvironment(luaEnvironment_), windowModule(window), resourcesModule(resources), frameInfo(info)
{
}
