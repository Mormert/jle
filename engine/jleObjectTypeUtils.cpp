#include "jleObjectTypeUtils.h"

namespace jle
{
	std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>> jleObjectTypeUtils::mRegisteredObjectsPtr { nullptr };
}
