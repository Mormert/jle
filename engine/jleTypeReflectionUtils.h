#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <cassert>

#define JLE_REGISTER_OBJECT_TYPE(object_name)							\
static const std::string_view GetObjectName(){ return #object_name;}	\
static inline const jle::jleObjectTypeRegistrator<object_name> object_name_Reg{ #object_name };

#define JLE_REGISTER_COMPONENT_TYPE(component_name)						\
static const std::string_view GetObjectName(){ return #component_name;}	\
static inline const jle::jleComponentTypeRegistrator<component_name> object_name_Reg{ #component_name };

namespace jle
{
	class jleObject;
	class jleComponent;

	class jleTypeReflectionUtils
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

		template <typename T>
		static void RegisterComponent()
		{
			static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

			std::string cName{ T::GetObjectName() };
			std::function<std::shared_ptr<T>()> cCreationFunc = []()
			{
				return std::make_shared<T>();
			};

			GetRegisteredObjectsRef().insert(std::make_pair(cName, cCreationFunc));
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

		static std::shared_ptr<jleComponent> InstantiateComponentByString(const std::string& str)
		{
			auto it = GetRegisteredComponentsRef().find(str);
			if (it == GetRegisteredComponentsRef().end())
			{
				return nullptr;
			}
			return it->second();
		}

		static std::map<std::string, std::function<std::shared_ptr<jleObject>()>>& GetRegisteredObjectsRef()
		{
			if (!mRegisteredObjectsPtr)
			{
				mRegisteredObjectsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>();
			}
			return *mRegisteredObjectsPtr;
		}

		static std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>& GetRegisteredComponentsRef()
		{
			if (!mRegisteredComponentsPtr)
			{
				mRegisteredComponentsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>>();
			}
			return *mRegisteredComponentsPtr;
		}

		// Should always be accessed via GetRegisteredObjectsRef()
		static inline std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>> mRegisteredObjectsPtr { nullptr };

		// Should always be accessed via GetRegisteredComponentsRef()
		static inline std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>> mRegisteredComponentsPtr { nullptr };
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

			jleTypeReflectionUtils::GetRegisteredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
		}
	};

	template <typename T>
	class jleComponentTypeRegistrator
	{
	public:
		jleComponentTypeRegistrator(const std::string& cName)
		{
			std::function<std::shared_ptr<T>()> cCreationFunc = []()
			{
				return std::make_shared<T>();
			};

			jleTypeReflectionUtils::GetRegisteredComponentsRef().insert(std::make_pair(cName, cCreationFunc));
		}
	};
}
