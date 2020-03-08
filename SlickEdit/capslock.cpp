/*
When using Vim emulation, press ESC to close CapsLock and IME.

To build the dll file, add SE_INSTALL_PATH\h and sub-directory to include path,
add SE_INSTALL_PATH\lib to library path, add vsapi.lib to link library, set
configuration to x64/release depends on your platform.
To load the dll file, activate command line, type 'dload somepath\capslock.dll'.
To test, type 'capslock_information' in command line.
*/
#include <vsapi.h>

EXTERN_C_BEGIN

HINSTANCE hins = NULL;
HWND hwnd = NULL;
HHOOK hkb = NULL;
BOOL status = FALSE;
//BYTE key_state[256] = {0};

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
	if(nCode < 0)
	{
		return ::CallNextHookEx(hkb, nCode, wParam, lParam);
	}
	if(((DWORD)lParam & 0x40000000) && (nCode == HC_ACTION))
	{
		if(wParam == VK_ESCAPE)
			/*
					{
						::GetKeyboardState(key_state);
						key_state[VK_CAPITAL] = 0;
						::SetKeyboardState(key_state);
					}
			*/
		{
			//shutdown CapsLock
			SHORT s = ::GetKeyState(VK_CAPITAL);
			if(s == 1)
			{
				::keybd_event(VK_CAPITAL, 0x3A, KEYEVENTF_EXTENDEDKEY | 0, 0);
				::keybd_event(VK_CAPITAL, 0x3A, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			//close ime
			//HWND hwnd = ::GetForegroundWindow();
			if(hwnd != NULL)
			{
				HIMC himc = ::ImmGetContext(hwnd);
				::ImmSetOpenStatus(himc, FALSE);
			}
		}
	}
	return ::CallNextHookEx(hkb, nCode, wParam, lParam);
}

BOOL CALLBACK EnumWindowsProc(HWND hwindow, LPARAM lParam)
{
	DWORD dwCurProcessId = *((DWORD*)lParam);
	DWORD dwProcessId = 0;

	::GetWindowThreadProcessId(hwindow, &dwProcessId);
	if((dwProcessId == dwCurProcessId) && (::GetParent(hwindow) == NULL))
	{
		*((HWND *)lParam) = hwindow;
		return FALSE;
	}
	return TRUE;
}

HWND capslock_get_window(void)
{
	DWORD dwCurrentProcessId = ::GetCurrentProcessId();
	if(!::EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId))
	{
		return (HWND)dwCurrentProcessId;
	}
	return NULL;
}

BOOL capslock_inject_hook()
{
	BOOL r = FALSE;
	if(hins != NULL)
		hkb = ::SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hins, 0);
	if(hkb != NULL)
	{
		r = TRUE;
		//vsMessageBox("Inject Hook Success", 0, VSMB_OK);
	}
	else
	{
		//vsMessageBox("Inject Hook Failed", 0, VSMB_OK);
	}
	return r;
}

BOOL capslock_remove_hook(void)
{
	return ::UnhookWindowsHookEx(hkb);
}

/*
   Export your functions here. This function
   is called right after this DLL is loaded.
*/
#pragma comment(linker, "/EXPORT:vsDllInit")
void VSAPI vsDllInit()
{
	status = capslock_inject_hook();
	vsDllExport("_command void capslock_information()", 0, 0);
}

/*
  vsDllExit is called when your dll is unloaded. This function is not required.
*/
#pragma comment(linker, "/EXPORT:vsDllExit")
void VSAPI vsDllExit()
{
	capslock_remove_hook();
}

#pragma comment(linker, "/EXPORT:capslock_information")
void VSAPI capslock_information()
{
	if(status == TRUE)
		vsMessageBox("Shutdown CapsLock 2018-07-17\nDLL loaded, HOOK injected.", 0, VSMB_OK);
	else
		vsMessageBox("Shutdown CapsLock 2018-07-17\nDLL failed, HOOK failed.", 0, VSMB_OK);
	return;
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hInstance);
		hins = hInstance;
		hwnd = capslock_get_window();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

EXTERN_C_END
