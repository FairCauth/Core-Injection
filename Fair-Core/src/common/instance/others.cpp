#include "..\utils.h"
#include <psapi.h>
#include <locale>
#include <codecvt>
#include <tlhelp32.h>
#include <comdef.h>
namespace utils::others {
    DWORD get_process_id(const char* processName) {
        PROCESSENTRY32 pe32;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe32)) {
            CloseHandle(hSnapshot);
            return 0;
        }

        do {
            _bstr_t b(pe32.szExeFile);
            const char* c = b;
            if (strcmp(c, processName) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return 0;
    }
    bool inject_dll(DWORD processID, const char* dllPath) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
        if (hProcess == NULL) {
            std::cerr << "Could not open process: " << GetLastError() << std::endl;
            return false;
        }

        LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
        if (pDllPath == NULL) {
            std::cerr << "Could not allocate memory in target process: " << GetLastError() << std::endl;
            CloseHandle(hProcess);
            return false;
        }

        if (!WriteProcessMemory(hProcess, pDllPath, dllPath, strlen(dllPath) + 1, NULL)) {
            std::cerr << "Could not write to process memory: " << GetLastError() << std::endl;
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0, NULL);
        if (hThread == NULL) {
            std::cerr << "Could not create remote thread: " << GetLastError() << std::endl;
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        WaitForSingleObject(hThread, INFINITE);
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hThread);
        CloseHandle(hProcess);

        return true;
    }

    std::string get_self_path() {
        char path[MAX_PATH];
        HMODULE hModule = NULL;
        GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)&get_self_path,
            &hModule
        );
        GetModuleFileNameA(hModule, path, MAX_PATH);
        std::string s(path);
        return s.substr(0, s.find_last_of("\\/"));
    }
    std::string get_self_name() {

        char path[MAX_PATH];

        HMODULE hModule = NULL;

        GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)&get_self_name,
            &hModule
        );

        GetModuleFileNameA(hModule, path, MAX_PATH);

        std::string s(path);
        return s.substr(s.find_last_of("\\/") + 1);
    }
}