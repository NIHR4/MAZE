#pragma once
#include <Windows.h>
#include <winternl.h>
#include <dbghelp.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "xor_utils.h"

typedef __kernel_entry NTSTATUS(__stdcall* NtQueryInformationProcess_ptr)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
typedef PIMAGE_NT_HEADERS(__stdcall* ImageNtHeader_ptr)(PVOID);


constexpr auto* ntdll_str_encoded = "\x5f\x44\x53\x5a\x4f\x4\x51\x5e\x54"; //ntdll.dll
constexpr auto* dbghelp_str_encoded = "\x75\x52\x50\x5e\x46\x46\x45\x1c\x5c\x44\x55"; //Dbghelp.dll
constexpr auto* ntqueryinformationprocess_str_encoded = "\x7f\x44\x66\x43\x46\x58\x4c\x7b\x56\x4e\x56\x41\x32\x44\x45\x59\x58\x58\x73\x58\x5a\x51\x5d\x5b\x4a"; //NtQueryInformationProcess
constexpr auto* imagentheader_str_encoded = "\x78\x5d\x56\x51\x46\x64\x41\x7a\x5d\x49\x5d\x56\x2d"; //ImageNtHeader

struct Maze_Functions {
	NtQueryInformationProcess_ptr NTQueryInformationProcessFunc;
	ImageNtHeader_ptr ImageNTHeaderFunc;
};

Maze_Functions initialize();
PEB* get_peb(const Maze_Functions& dynamic_function);
std::vector<LDR_DATA_TABLE_ENTRY>get_peb_entries(PEB* peb);
void* get_module_base(std::vector<LDR_DATA_TABLE_ENTRY>::iterator beg, std::vector<LDR_DATA_TABLE_ENTRY>::iterator end, const std::wstring ModuleName);
void* get_module_base(const std::vector<LDR_DATA_TABLE_ENTRY>&vec, const std::wstring ModuleName);
void* get_procedure(char* dll_base, const char* export_name, const Maze_Functions& dynamic_function);

