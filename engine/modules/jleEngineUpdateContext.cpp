#include "jleEngineUpdateContext.h"

jleEngineUpdateContext::jleEngineUpdateContext(jleGameRuntime &gameRuntime,
                                                 jleGraphics &renderer,
                                                 jleRenderThread &renderThread,
                                                 jleFramePacket &renderGraph_,
                                                 jleEngineSettings &engineSettings,
                                                 jleInput &input,
                                                 jleWindow &window,
                                                 jleResourceHolder &resources,
                                                 jleFrameInfo &info,
                                                 const jleSerializationContext & serializationContext)
    : serializationContext(serializationContext), gameRuntime(gameRuntime), graphics(renderer),
      window(window),
      resourcesHolder(resources), input(input), renderThread(renderThread),
      currentFramePacket(renderGraph_), frameInfo(info), settings(engineSettings)
{
}
