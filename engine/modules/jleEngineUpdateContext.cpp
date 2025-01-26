#include "jleEngineUpdateContext.h"

jleEngineUpdateContext::jleEngineUpdateContext(jleGameRuntime &gameRuntime,
                                                 jleEngineSettings &engineSettings,
                                                 jleResourceHolder &resources,
                                                 jleFrameInfo &info,
                                                 const jleSerializationContext & serializationContext)
    : serializationContext(serializationContext), gameRuntime(gameRuntime), resourcesHolder(resources), frameInfo(info), settings(engineSettings)
{
}
