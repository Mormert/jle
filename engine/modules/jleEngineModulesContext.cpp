#include "jleEngineModulesContext.h"

jleEngineModulesContext::jleEngineModulesContext(jle3DRenderer &renderer,
                                                 jleInput &input,
                                                 jleWindow &window,
                                                 jleResources &resources,
                                                 jleFrameInfo& info)
    : rendererModule(renderer), inputModule(input), windowModule(window), resourcesModule(resources), frameInfo(info)
{
}
