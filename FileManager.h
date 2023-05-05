#pragma once
#include "FileManager.h"
#include <string>
#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <iostream>
#include <locale>
#include <codecvt>


class FileManager
{
public:
	FileManager(std::string path);
	void CheckDirectoryChanges();
	std::string Directory;

private:
	std::string narrowDownString(std::wstring toChange);
	void findDirectory(std::wstring& filename);
};

