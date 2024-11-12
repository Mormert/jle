#include "jleEngineUpdateContext.h"

jleEngineUpdateContext::jleEngineUpdateContext(jleGameRuntime &gameRuntime,
                                                 jleGraphics &renderer,
                                                 jleRenderThread &renderThread,
                                                 jleFramePacket &renderGraph_,
                                                 jleEngineSettings &engineSettings,
                                                 jleInput &input,
                                                 jleLuaEnvironment &luaEnvironment_,
                                                 jleWindow &window,
                                                 jleResourceHolder &resources,
                                                 jleFrameInfo &info,
                                                 jleSerializationContext& serializationContext)
    : gameRuntime(gameRuntime), rendererModule(renderer), renderThread(renderThread),
      currentFramePacket(renderGraph_),
      settings(engineSettings), inputModule(input), luaEnvironment(luaEnvironment_), windowModule(window),
      resourcesModule(resources), frameInfo(info), serializationContext(serializationContext)
{
}
