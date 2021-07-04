#pragma once

//#include "jleObject.h"

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <cassert>


#define JLE_REGISTER_OBJECT_TYPE(object_name)							\
static const std::string_view GetObjectName(){ return #object_name;}	\
static inline const jle::jleObjectTypeRegistrator<object_name> object_name_Reg{ #object_name };

namespace jle
{
	class jleObject;

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

			GetRegisteredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
		}

		static std::shared_ptr<jleObject> InstantiateObjectByString(const std::string& str)
		{
			auto it = GetRegisteredObjectsRef().find(str);
			if (it == GetRegisteredObjectsRef().end())
			{
				return nullptr;
			}
			return it->second();
		}

	private:
		template <typename T>
		friend class jleObjectTypeRegistrator;

		static std::map<std::string, std::function<std::shared_ptr<jleObject>()>>& GetRegisteredObjectsRef()
		{
			if (!mRegisteredObjectsPtr)
			{
				mRegisteredObjectsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>();
			}
			return *mRegisteredObjectsPtr;
		}

		// Should always be accessed via GetRegisteredObjectsRef()
		static std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>> mRegisteredObjectsPtr;
	};

	template <typename T>
	class jleObjectTypeRegistrator
	{
	public:
		jleObjectTypeRegistrator(const std::string& oName)
		{
			std::function<std::shared_ptr<T>()> oCreationFunc = []()
			{
				return std::make_shared<T>();
			};

			jleObjectTypeUtils::GetRegisteredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
		}
	};
}
