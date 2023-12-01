#pragma once
#include "../../Framework.h"

class CMemory
{
public:
	template <typename T>
	inline T FindInterface( const char* const dll, const char* const version )
	{
		HMODULE hModule = GetModuleHandleA( dll );
		if ( hModule )
		{
			auto CreateInterface = reinterpret_cast< __declspec( dllexport ) void* ( __cdecl* )( const char*, int* ) >( GetProcAddress( hModule, "CreateInterface" ) );
			int nReturnCode = 0;
			void* pInterface = CreateInterface( version, &nReturnCode );
			if ( pInterface )
			{
				return reinterpret_cast< T >( pInterface );
			}
		}
		return nullptr;
	}

	template <typename T>
	inline T GetVFunc( void* ptr, const unsigned short index )
	{
		return ( *static_cast< T** >( ptr ) )[ index ];
	}

	template < typename T, const std::uint16_t offset = 0u >
	inline T FindSignature( const char* module_name, const char* signature ) {
		if ( const auto module_handle = GetModuleHandleA( module_name ) ) {
			static auto signature_to_byte = [ ] ( const char* signature ) {
				auto bytes = std::vector< int >{ };
				auto start = const_cast< char* >( signature );
				auto end = const_cast< char* >( signature ) + strlen( signature );

				for ( auto current = start; current < end; ++current ) {
					if ( *current == '?' ) {
						++current;

						if ( *current == '?' )
							++current;

						bytes.push_back( -1 );
					}

					else bytes.push_back( strtoul( current, &current, 16 ) );
				}

				return bytes;
			};

			auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( module_handle );
			auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< std::uint8_t* >( module_handle ) + dos_header->e_lfanew );

			auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
			auto pattern_bytes = signature_to_byte( signature );
			auto scan_bytes = reinterpret_cast< std::uint8_t* >( module_handle );

			auto s = pattern_bytes.size( );
			auto d = pattern_bytes.data( );

			for ( auto i = 0ul; i < size_of_image - s; ++i ) {
				bool found = true;

				for ( auto j = 0ul; j < s; ++j ) {
					if ( scan_bytes[ i + j ] != d[ j ] && d[ j ] != -1 ) {
						found = false;
						break;
					}
				}

				if ( found )
					return reinterpret_cast< T >( &scan_bytes[ i ] + offset );
			}
		}

		return 0;
	}
};

inline CMemory* Memory = new CMemory;