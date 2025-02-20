#include <windows.h>
#include <thread>

#include "utils/utils.h"
#include "math/math.h"
#include "sdk/sdk.h"
#include "utils/hook/hooks.h"
#include "helpers/crt/crt.h"
#include "utils/render/render.h"
#include "utils/render/ui/menu.h"
#include "utils/configs/config.h"

static BOOL CALLBACK EnumWindowsCallback( HWND handle, LPARAM lParam )
{
	const auto is_main_window = [ handle ] ( )
	{
		return GetWindow( handle, GW_OWNER ) == nullptr && IsWindowVisible( handle ) && handle != GetConsoleWindow( );
	};

	DWORD window_pid = 0;
	GetWindowThreadProcessId( handle, &window_pid );

	if ( GetCurrentProcessId( ) != window_pid || !is_main_window( ) )
		return TRUE;

	*reinterpret_cast< HWND* >( lParam ) = handle;
	return FALSE;
}

DWORD WINAPI LoadClient( PVOID module_ptr )
{
	try
	{
		while (g::m_hwnd == nullptr)
		{
			EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&g::m_hwnd));
			std::this_thread::sleep_for(200ms);
		}

		if (!math::Init()) { CS2_LOG(xorstr_("Failed to initialize math.\n"));throw std::runtime_error(xorstr_("1A")); }

		if (!utils::Init()) { CS2_LOG(xorstr_("Failed to initialize.\n")); throw std::runtime_error(xorstr_("1A")); }

		if (!sdk::Init()) { CS2_LOG(xorstr_("Failed to initialize.\n")); throw std::runtime_error(xorstr_("1A")); }
		
		if (!SCHEMA::Setup(xorstr_(L"schema_client.txt"), xorstr_("client.dll"))) { CS2_LOG(xorstr_("Failed to initialize.\n")); throw std::runtime_error(xorstr_("1A")); }

		if (!SCHEMA::Setup(xorstr_(L"schema_sv.txt"), xorstr_("server.dll"))) { CS2_LOG(xorstr_("Failed to initialize.\n")); throw std::runtime_error(xorstr_("1A")); }

		if (!hooks::Init()) { CS2_LOG(xorstr_("Failed to initialize.\n")); throw std::runtime_error(xorstr_("1A")); }

		if (!C::Setup(xorstr_(CS_CONFIGURATION_DEFAULT_FILE_NAME))) { CS2_LOG(xorstr_("found no configuration to load\n")); }
	
		CS2_LOG(xorstr_("done\n"));
	}

	// catch exceptions
	catch ( const std::exception& exception )
	{
		// show exception text in a popup
		MessageBox( nullptr, exception.what( ), xorstr_( "cs2" ), MB_OK | MB_ICONERROR );

		// stop running
		return 0;
	}
			

	// allow for unload in debug
#ifdef CS2_UNLOAD
	// wait for VK_END
	while ( !GetAsyncKeyState( VK_END ) )
		std::this_thread::sleep_for( 200ms );

	// tell hooks to not run
	g::m_unloading = true;

	// wait for active hooks to finish
	std::this_thread::sleep_for( 200ms );

	// destroy render
	render::Destroy( );

	// close GUI
	menu::m_opened = false;

	L::DetachConsole();
	L::CloseFile();

	// wait for menu to close
	std::this_thread::sleep_for( 200ms );

	// check if old wndproc is valid
	if ( hooks::m_old_wndproc != nullptr )
	{
		// restore wnd proc
		SetWindowLongPtrW( g::m_hwnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( hooks::m_old_wndproc ) );

		// invalidate old wnd proc
		hooks::m_old_wndproc = nullptr;
	}

	// destroy minhook
	MH_DisableHook( MH_ALL_HOOKS );
	MH_RemoveHook( MH_ALL_HOOKS );
	MH_Uninitialize( );

	// close thread
	FreeLibraryAndExitThread( reinterpret_cast< HMODULE >( module_ptr ), EXIT_SUCCESS );
#endif

	return 1;
}

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);

BOOL WINAPI DllMain( HMODULE module, DWORD reason, LPVOID reserved )
{
	// process destroy of the cheat before crt calls atexit table
	// on injection
	if ( reason == DLL_PROCESS_ATTACH )
	{		
		if (!_CRT_INIT(module, reason, reserved))
			return FALSE;

		g::m_module = module;
		// create main thread
		const HANDLE main_handle = CreateThread( nullptr, NULL, LoadClient, module, NULL, nullptr );

		// close handle
		if ( main_handle != nullptr )
			CloseHandle( main_handle );
	}

	return 1;
}
