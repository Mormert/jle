#pragma once

#include <string>

class FileLoadInterface {
public:
	virtual ~FileLoadInterface() {}

	// Should implement logic for loading data from file into derived class
	virtual bool LoadFromFile(const std::string& path) = 0;
};