# MAZE
Memory AntiviruZ Evasion 
This library is designed to help you  avoid AV false positives by dynamically resolving the address of your desired function.
As of everything there is potential for misuse

## Usage
### Step 1
First things first you call your `initialize()` function, this step is crucial to load DLLs and get addresses to the required functions provided by these libraries. You shall save the return value of this function in a **Maze_Functions** struct.
If this function fails std::runtime_error is thrown.
### Step 2
Then you must  get a pointer to the Process Enviroment Block of the current process, this is task is performed by the `get_peb` function
which takes as an argument a constant reference to a the Maze_Functions struct created in the previous step.
If this function fails it will return `nullptr`
### Step 3
Next you call `get_peb_entries` passing the PEB pointer you adquired in the previous step to the function, this will return an std::vector<LDR_DATA_TABLE_ENTRY> object, you can implement your own search algorithm, or use any of the existing STL algorithms however it is recommended you use `get_module_base` which takes as arguments a constant reference to an object of type std::vector<LDR_DATA_TABLE_ENTRY> and a constant std::wstring , or it's overloaded version which takes 1 pair of iterators and a constant std::wstring, this string contains the name of the **module** to be found. this function will return a value of type `void*` which represents the base address of the desired module, in case the module can't be found the return value will be `nullptr`
### Step 4
The final step is calling `get_procedure` of type `void*`, which will return the address of the desired function if successful, this address is then to be stored inside a function pointer defined by the programmer and appropriately casted.
It's arguments are a void pointer to the base of the module where the desired function comes from(Step 3), a `const char*` or C-STRING with the name of the exported function and finally a constant reference to the **Maze_Functions** struct from step number 1.

## Example
```cpp
#include <iostream>
#include "Maze.h"

typedef bool(__stdcall *rpm_ptr)(HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T*);
typedef bool(__stdcall *wpm_ptr)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);

int main() {	
	Maze_Functions functions = initialize();
	PEB* peb = get_peb(functions);	
	void* baseaddress = get_module_base(get_peb_entries(peb), L"KERNEL32.DLL");
	if (baseaddress == nullptr) return 0;
	wpm_ptr dWriteProcessMemory_ptr = reinterpret_cast<wpm_ptr>(get_procedure(baseaddress, "WriteProcessMemory", functions));
	rpm_ptr dReadProcesMemory_ptr = reinterpret_cast<rpm_ptr>(get_procedure(baseaddress, "ReadProcessMemory", functions));
  //You can proceed to call the function pointers as normal functions...
  return 0;
  
  }

```
