#include "Maze.h"

std::vector<unsigned char>global_cipher_key{ 49,48,55,54,35,42,53,50,56,40,57,51,95,37 };
Maze_Functions initialize(){
	HINSTANCE ntdllInstance = LoadLibrary(vec_to_str(perform_xor(ntdll_str_encoded, global_cipher_key)).c_str());
	HINSTANCE dbghelpInstance = LoadLibrary(vec_to_str(perform_xor(dbghelp_str_encoded,global_cipher_key)).c_str());
	if (!dbghelpInstance ||!ntdllInstance) throw std::runtime_error{ "Couldn't adquire essential DLLS" };
	Maze_Functions ret{
		reinterpret_cast<NtQueryInformationProcess_ptr>(GetProcAddress(ntdllInstance,vec_to_str(perform_xor(ntqueryinformationprocess_str_encoded,global_cipher_key)).c_str())),
		reinterpret_cast<ImageNtHeader_ptr>(GetProcAddress(dbghelpInstance,vec_to_str(perform_xor(imagentheader_str_encoded,global_cipher_key)).c_str()))
	};
	
	if (!ret.ImageNTHeaderFunc || !ret.NTQueryInformationProcessFunc) throw std::runtime_error{ "Couldn't obtain pointer to essential functions" };
	else return ret;
}

PEB* get_peb(const Maze_Functions& dynamic_function) {
	HANDLE Process = GetCurrentProcess();
	PROCESS_BASIC_INFORMATION pbi;
	ULONG retLen;
	dynamic_function.NTQueryInformationProcessFunc(Process, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &retLen);
	return pbi.PebBaseAddress;

}

std::vector<LDR_DATA_TABLE_ENTRY>get_peb_entries(PEB* peb) {
	std::vector<LDR_DATA_TABLE_ENTRY>entries;
	const LIST_ENTRY* FirstLE = peb->Ldr->InMemoryOrderModuleList.Flink;
	LIST_ENTRY* CurrentLE = peb->Ldr->InMemoryOrderModuleList.Flink;
	while (entries.empty() || CurrentLE != FirstLE) {
		auto entry = *CONTAINING_RECORD(CurrentLE, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		if (entry.DllBase) entries.push_back(entry);
		CurrentLE = CurrentLE->Flink;
	}
	return entries;
}

void* get_procedure(char* dll_base, const char* export_name, const Maze_Functions& dynamic_function) {
	PIMAGE_NT_HEADERS peNtHeaders = dynamic_function.ImageNTHeaderFunc(dll_base);
	DWORD exportDescriptorOffset = peNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	IMAGE_EXPORT_DIRECTORY* exportTable = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(dll_base + exportDescriptorOffset);
	DWORD * name_table = reinterpret_cast<DWORD*>(dll_base + exportTable->AddressOfNames);
	WORD * ordinal_table = reinterpret_cast<WORD*>(dll_base + exportTable->AddressOfNameOrdinals);
	DWORD * func_table = reinterpret_cast<DWORD*>(dll_base + exportTable->AddressOfFunctions);

	for (int i = 0; i < exportTable->NumberOfNames; ++i) {
		char* funcName = (char*)(dll_base + name_table[i]);
		char* func_ptr = dll_base + func_table[ordinal_table[i]];
		if (!_strcmpi(funcName, export_name)) return func_ptr;
	}
	return 0;
}

void* get_module_base(std::vector<LDR_DATA_TABLE_ENTRY>::iterator beg, std::vector<LDR_DATA_TABLE_ENTRY>::iterator end, const std::wstring ModuleName)
{
	auto hit = std::find_if(beg, end, [ModuleName](LDR_DATA_TABLE_ENTRY LDR) {
		std::wstring str(LDR.FullDllName.Buffer);
		size_t hit = str.find(ModuleName);
		return hit != std::wstring::npos;
	});
	if (hit == end) return nullptr;
	return hit->DllBase;
}

void* get_module_base(const std::vector<LDR_DATA_TABLE_ENTRY>& vec, const std::wstring ModuleName)
{
	auto hit = std::find_if(vec.begin(), vec.end(), [ModuleName](LDR_DATA_TABLE_ENTRY LDR) {
		std::wstring str(LDR.FullDllName.Buffer);
		size_t hit = str.find(ModuleName);
		return hit != std::wstring::npos;
	});
	if (hit == vec.end()) return nullptr;
	return hit->DllBase;
}
