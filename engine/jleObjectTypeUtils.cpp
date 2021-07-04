#include "jleObjectTypeUtils.h"

namespace jle
{
	std::map<std::string, std::function<std::shared_ptr<jleObject>()>> jleObjectTypeUtils::mRegisteredObjects {};
}
