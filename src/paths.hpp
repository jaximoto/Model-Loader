#pragma once
#include <string>
#include <filesystem>

namespace paths
{
	/*
	* Using filesystem paths instead of strings
	* because it handles cross-platform path 
	* seperators and has built in path methods.
	* 
	* Uses inline to make methods available to multiple
	* files without a linker error.
	* 
	* This basically just wraps the cmake macro into a
	* filesystem path object that is more robust
	* and easier to manipulate.
	*/
	inline std::filesystem::path root()
	{
		return std::filesystem::path(PROJECT_ROOT_DIR);
	}

	inline std::filesystem::path shaders()
	{
		return root() / "shaders";
	}

	inline std::filesystem::path resources()
	{
		return root() / "resources";
	}
}