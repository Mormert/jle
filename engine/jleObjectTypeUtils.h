#pragma once

#include "jleObject.h"

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <cassert>


namespace jle
{
	class jleObjectTypeUtils
	{
	public:
		template <typename T>
		static void RegisterObject()
		{
			static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

			std::string oName{ T::GetObjectName() };
			std::function<std::shared_ptr<T>()> oCreationFunc = []()
			{
				return std::make_shared<T>();
			};

			mRegisteredObjects.insert(std::make_pair(oName, oCreationFunc));
		}

		static std::shared_ptr<jleObject> InstantiateObjectByString(const std::string& str)
		{
			auto it = mRegisteredObjects.find(str);
			if (it == mRegisteredObjects.end())
			{
				return nullptr;
			}
			return it->second();
		}

	protected:
		/*static std::map<std::string, std::function<std::shared_ptr<jleObject>()>>* GetRegisteredObjects()
		{
			if (!mRegisteredObjects)
			{
				mRegisteredObjects = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>();
			}
			return mRegisteredObjects.get();
		}*/

	private:
		static std::map<std::string, std::function<std::shared_ptr<jleObject>()>> mRegisteredObjects;
	};

	/*template <typename T>
	class jleObjectTypeUtilsRegistrator : public jleObjectTypeUtils
	{
	public:
		jleObjectTypeUtilsRegistrator(const std::string& oName)
		{
			std::function<std::shared_ptr<T>()> oCreationFunc = []()
			{
				return std::make_shared<T>();
			};
			GetRegisteredObjects->insert(std::make_pair(oName, oCreationFunc));
		}
	};*/
}
