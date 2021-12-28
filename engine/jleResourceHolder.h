#pragma once

#include "FileLoadInterface.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>

template<typename T>
class jleResourceHolder {
public:

	// Gets a shared_ptr to a resource from file, or a shared_ptr to an already loaded copy of that resource
	static std::shared_ptr<T> LoadResourceFromFile(const std::string& path)
	{
		static_assert(std::is_base_of<FileLoadInterface, T>::value, "T must derive from FileLoadInterface");

		auto it = mResources.find(path);
		if (it != mResources.end())
		{
			return it->second;
		}

		std::shared_ptr<T> new_resource = std::make_shared<T>();

		new_resource->LoadFromFile(path);

		mResources.erase(path);
		mResources.insert(std::make_pair(path, new_resource));

		PeriodicResourcesCleanUp();

		return new_resource;
	}

	// Stores a resource with a certain path to be reused later
	static void StoreResource(std::shared_ptr<T> resource, const std::string& path)
	{
		mResources.erase(path);
		mResources.insert(std::make_pair(path, resource));

		PeriodicResourcesCleanUp();
	}

	// Get a resource that is already loaded
	static std::shared_ptr<T> GetResource(const std::string& path)
	{
		return mResources.at(path);
	}

	// Check to see if a resource is loaded
	static bool IsResourceLoaded(const std::string& path)
	{
		auto it = mResources.find(path);
		if (it == mResources.end())
		{
			return false;
		}
		return true;
	}

private:
	static inline std::unordered_map<std::string, std::shared_ptr<T>> mResources{};

	static inline int mPeriodicCleanCounter{ 0 };
	static void PeriodicResourcesCleanUp()
	{
		// Clean every 10th time that this method is called
		if (++mPeriodicCleanCounter % 10 == 0)
		{
			std::vector<std::string> keys_for_removal;

			for (auto&& res_kvp : mResources)
			{
				// If the use count is 1, it means that no other place is the resource used other
				// than inside the unordered map, which means that it is time to delete it from memory.
				if (res_kvp.second.use_count() == 1)
				{
					keys_for_removal.push_back(res_kvp.first);
				}
			}

			for (auto&& key : keys_for_removal)
			{
				mResources.erase(key);
			}
		}
	}
};
