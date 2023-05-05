#include "MemoryManager.h"

using namespace std;

MemoryManager::MemoryManager(string process) {
    DWORD processID = getROSProcessID();
    this->Process = processID;
    cout << this->Process << endl;
	this->MemoryAddresses = this->getMemoryAddresses();

    for (int i = 0; i < this->MemoryAddresses.size(); i++) {
        cout << this->MemoryAddresses[i] << endl;
    }

    while (true) {
        if (this->isHooked()) {
            cout << "Process has been hooked" << endl;
        }
        else {
            cout << "Process has not been hooked" << endl;
        }
    }
}

bool MemoryManager::isHooked() {
    LPVOID address = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    // Check if the first few bytes of the function are a jump instruction
    // This could indicate an inline hook
    BYTE buffer[5];
    memcpy(buffer, address, 5);
    if (buffer[0] == 0xE9 || buffer[0] == 0xE8) {
        return true;
    }
    return false;
}

vector<void*> MemoryManager::getMemoryAddresses() {
    DWORD processid = this->Process;
    vector<void*> memory_addresses;

    HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processid);

    if (process_handle == NULL)
    {
        cout << "Could not open process with ID " << processid << endl;
        return memory_addresses;
    }

    HMODULE module_handles[1024];
    DWORD module_handles_size;

    if (!EnumProcessModules(process_handle, module_handles, sizeof(module_handles), &module_handles_size))
    {
        cout << "Could not enumerate process modules" << endl;
        CloseHandle(process_handle);
        return memory_addresses;
    }

    for (unsigned int i = 0; i < module_handles_size / sizeof(HMODULE); i++)
    {
        MODULEINFO module_info;

        if (!GetModuleInformation(process_handle, module_handles[i], &module_info, sizeof(MODULEINFO)))
        {
            cout << "Could not get module information" << endl;
            continue;
        }

        void* base_address = module_info.lpBaseOfDll;
        SIZE_T region_size = module_info.SizeOfImage;

        MEMORY_BASIC_INFORMATION memory_info;

        for (void* address = base_address; address < (char*)base_address + region_size; address = (char*)memory_info.BaseAddress + memory_info.RegionSize)
        {
            if (!VirtualQueryEx(process_handle, address, &memory_info, sizeof(memory_info)))
            {
                cout << "Could not get memory information" << endl;
                break;
            }

            if (memory_info.State == MEM_COMMIT && (memory_info.Type == MEM_PRIVATE || memory_info.Type == MEM_MAPPED || memory_info.Type == MEM_IMAGE))
            {
                memory_addresses.push_back(memory_info.BaseAddress);
            }
        }
    }

    CloseHandle(process_handle);
    return memory_addresses;
}

DWORD MemoryManager::getROSProcessID() {
    const std::wstring& processName = L"pythonw.exe";
    HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot_handle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create process snapshot" << std::endl;
        return 0;
    }

    PROCESSENTRY32 process_entry = {};
    process_entry.dwSize = sizeof(process_entry);
    if (!Process32First(snapshot_handle, &process_entry))
    {
        std::cerr << "Failed to get first process entry" << std::endl;
        CloseHandle(snapshot_handle);
        return 0;
    }

    DWORD process_id = 0;
    do
    {
        std::wstring current_process_name = process_entry.szExeFile;
        if (current_process_name == processName)
        {
            process_id = process_entry.th32ProcessID;
            break;
        }
    } while (Process32Next(snapshot_handle, &process_entry));

    CloseHandle(snapshot_handle);
    return process_id;
}