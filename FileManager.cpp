#pragma once
#include "FileManager.h"
#include <string>
#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <iostream>
#include <locale>
#include <codecvt>

using namespace std;

FileManager::FileManager(string path) {
	this->Directory = path;
}

string FileManager::narrowDownString(wstring toChange) {
    // This is better than putting this in the code constantly
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(toChange);
}

LPCWSTR widenString(string toChange) {
    int len = static_cast<int>(toChange.length()) + 1;
    int wcharLen = MultiByteToWideChar(CP_UTF8, 0, toChange.c_str(), len, NULL, 0);
    LPWSTR toReturn = new WCHAR[wcharLen];
    MultiByteToWideChar(CP_UTF8, 0, toChange.c_str(), len, toReturn, wcharLen);
    return toReturn;
}

void FileManager::findDirectory(wstring& filename) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(filename.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "File not found \n";
        return;
    }

    this->Directory = narrowDownString(findData.cFileName);
}

void FileManager::CheckDirectoryChanges() {
    // Convert narrow strings to wide strings
    string directory = this->Directory;
    wchar_t wideDirectory[256];
    MultiByteToWideChar(CP_UTF8, 0, directory.c_str(), -1, wideDirectory, 256);
    HANDLE hDir = CreateFile(wideDirectory, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        cout << "Error: Failed to open directory" << endl;
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;

    while (true) {
        if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL)) {
            FILE_NOTIFY_INFORMATION* pNotifyInfo = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                wstring filename(pNotifyInfo->FileName, pNotifyInfo->FileNameLength / 2);
                string displayFilename = narrowDownString(filename);
                // Check if the change was made to a file or directory
                if (pNotifyInfo->Action == FILE_ACTION_MODIFIED && pNotifyInfo->FileNameLength > 0) {
                    string message = "File " + displayFilename + " was modified.";
                    MessageBox(NULL, widenString(message), L"Edit detected", MB_OK);
                }
                else if (pNotifyInfo->Action == FILE_ACTION_RENAMED_NEW_NAME) {
                    cout << "File " << displayFilename << " was renamed." << endl;
                }
            } while ((pNotifyInfo = (FILE_NOTIFY_INFORMATION*)pNotifyInfo->NextEntryOffset) != NULL);
        }
        else {
            cout << "Error: Failed to read directory changes" << endl;
        }
    }

    CloseHandle(hDir);
}