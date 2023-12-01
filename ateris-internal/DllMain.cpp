// dllmain.cpp : Defines the entry point for the DLL application.
#include "Framework/Framework.h"
#include "Globals.h"

long APIENTRY ExceptionFilter( PEXCEPTION_POINTERS Info )
{
	if ( Info->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION )
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}

	std::stringstream error;
	error << "Copy this error message and send it to cleric by using CTRL+C and CTRL+V.\n";
	error << "Exception code: 0x" << std::hex << std::uppercase << Info->ExceptionRecord->ExceptionCode << " at 0x" << std::hex << std::uppercase << Info->ExceptionRecord->ExceptionAddress << "\n\n";
	error << "EAX: 0x" << std::hex << std::uppercase << Info->ContextRecord->Eax << "\n";
	error << "EBX: 0x" << std::hex << std::uppercase << Info->ContextRecord->Ebx << "\n";
	error << "ECX: 0x" << std::hex << std::uppercase << Info->ContextRecord->Ecx << "\n";
	error << "EDX: 0x" << std::hex << std::uppercase << Info->ContextRecord->Edx << "\n";
	error << "EBP: 0x" << std::hex << std::uppercase << Info->ContextRecord->Ebp << "\n";
	error << "ESP: 0x" << std::hex << std::uppercase << Info->ContextRecord->Esp << "\n";
	error << "ESI: 0x" << std::hex << std::uppercase << Info->ContextRecord->Esi << "\n";
	error << "EDI: 0x" << std::hex << std::uppercase << Info->ContextRecord->Edi << "\n";
	//error << "EIP: 0x" << std::hex << std::uppercase << Info->ContextRecord->Eip << "\n";
	error << "EFLAGS: 0x" << std::hex << std::uppercase << Info->ContextRecord->EFlags << "\n\n";

	for ( auto& frame : Framework->GetStackTrace( Info->ContextRecord ) )
	{
		error << std::hex << std::uppercase << frame.address << std::resetiosflags << " in " << frame.module << " - " << frame.name << "() - " << frame.file << ": " << std::dec << frame.line << "\n";
	}

	MessageBoxA( nullptr, error.str( ).c_str( ), "Exception found!", MB_OK );

	return EXCEPTION_EXECUTE_HANDLER;
}

DWORD APIENTRY MainThread( LPVOID lpReserved )
{
	AddVectoredExceptionHandler( 1, ExceptionFilter );
	Framework->Attach( );
	
	if ( G->Authentication->IsDebugAllowed )
	{
		while ( !( GetAsyncKeyState( VK_DELETE ) && Input->InFocus( ) ) )
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	else
	{
		while ( true )
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}

	Framework->Detach( );

	FreeLibraryAndExitThread( static_cast< HMODULE >( lpReserved ), EXIT_SUCCESS );
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	DisableThreadLibraryCalls( hModule );
	if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		if ( HANDLE hHandle = CreateThread( nullptr, 0, MainThread, hModule, 0, nullptr ) )
		{
			CloseHandle( hHandle );
		}
	}

	return TRUE;
}