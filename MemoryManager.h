#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <TlHelp32.h>

class MemoryManager
{
public:
	MemoryManager(std::string process);
	DWORD Process;

private:
	DWORD getROSProcessID();
	std::vector<void*> getMemoryAddresses();
	std::vector<void*> MemoryAddresses; 
	bool isHooked();
};

