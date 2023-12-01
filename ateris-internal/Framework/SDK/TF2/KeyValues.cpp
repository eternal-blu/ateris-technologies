#include "KeyValues.h"
#include "../../Utils/Memory/Memory.h"

#include <stringapiset.h>
#include <string>

#pragma warning (disable : 6031)
#pragma warning (disable : 4244)

#define Q_ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))

int UnicodeToUTF8( const wchar_t* unicode, char* ansi, int ansiBufferSize )
{
	int result = WideCharToMultiByte( CP_UTF8, 0, unicode, -1, ansi, ansiBufferSize, NULL, NULL );
	ansi[ ansiBufferSize - 1 ] = 0;
	return result;
}

int UTF8ToUnicode( const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes )
{
	int chars = MultiByteToWideChar( CP_UTF8, 0, ansi, -1, unicode, unicodeBufferSizeInBytes / sizeof( wchar_t ) );
	unicode[ ( unicodeBufferSizeInBytes / sizeof( wchar_t ) ) - 1 ] = 0;
	return chars;
}

bool KeyValues::LoadFromBuffer( char const* resource_name, const char* buffer, void* file_system, const char* path_id )
{
	static auto FN = Memory->FindSignature< int( __thiscall* )( KeyValues*, char const*, const char*, void*, const char* ) >( "client.dll", "55 8B EC 83 EC 38 53 8B 5D 0C" );
	return FN( this, resource_name, buffer, file_system, path_id );
}

void KeyValues::Initialize( char* name )
{
	static DWORD address = Memory->FindSignature< DWORD >( "engine.dll", "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6" ) - 0x42;
	static auto FN = reinterpret_cast< KeyValues* ( __thiscall* )( KeyValues*, char* ) >( address );
	FN( this, name );
}

KeyValues::KeyValues( const char* name )
{
	char _name[ 128 ];
	sprintf_s( _name, sizeof( _name ), name );
	this->Initialize( ( char* )_name );
}

KeyValues* KeyValues::FindKey( const char* keyName, bool bCreate )
{
	static auto FN = Memory->FindSignature< KeyValues * ( __thiscall* )( KeyValues*, const char*, bool ) >( "client.dll", "55 8B EC 81 EC ? ? ? ? 56 8B 75 08 57 8B F9 85 F6 0F 84 ? ? ? ? 80 3E 00 0F 84" );
	return FN( this, keyName, bCreate );
}

//55 8B EC 8B 51 ? 85 D2 75 ? 8B 45 ? 89 41 ? 5D C2
KeyValues* KeyValues::AddSubkey( KeyValues* subKey )
{
	static auto FN = Memory->FindSignature < KeyValues * ( __thiscall* )( KeyValues*, KeyValues* ) >( "client.dll", "55 8B EC 8B 51 ? 85 D2 75 ? 8B 45 ? 89 41 ? 5D C2" );
	return FN( this, subKey );
}

int KeyValues::GetInt( const char* keyName, int defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
			case TYPE_STRING:
				return atoi( dat->m_sValue );
			case TYPE_WSTRING:
				return _wtoi( dat->m_wsValue );
			case TYPE_FLOAT:
				return ( int )dat->m_flValue;
			case TYPE_UINT64:
				return 0;
			case TYPE_INT:
			case TYPE_PTR:
			default:
				return dat->m_iValue;
		};
	}
	return defaultValue;
}

unsigned long long KeyValues::GetUint64( const char* keyName, unsigned long long defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
			case TYPE_STRING:
				return ( unsigned long long )_atoi64( dat->m_sValue );
			case TYPE_WSTRING:
				return _wtoi64( dat->m_wsValue );
			case TYPE_FLOAT:
				return ( int )dat->m_flValue;
			case TYPE_UINT64:
				return *( ( unsigned long long* )dat->m_sValue );
			case TYPE_INT:
			case TYPE_PTR:
			default:
				return dat->m_iValue;
		};
	}
	return defaultValue;
}

float KeyValues::GetFloat( const char* keyName, float defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
			case TYPE_STRING:
				return ( float )atof( dat->m_sValue );
			case TYPE_WSTRING:
				return ( float )_wtof( dat->m_wsValue );
			case TYPE_FLOAT:
				return dat->m_flValue;
			case TYPE_INT:
				return ( float )dat->m_iValue;
			case TYPE_UINT64:
				return ( float )( *( ( unsigned long long* )dat->m_sValue ) );
			case TYPE_PTR:
			default:
				return 0.0f;
		};
	}
	return defaultValue;
}

const char* KeyValues::GetString( const char* keyName, const char* defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		char buf[ 64 ];
		switch ( dat->m_iDataType )
		{
			case TYPE_FLOAT:
				snprintf( buf, sizeof( buf ), "%f", dat->m_flValue );
				SetString( keyName, buf );
				break;
			case TYPE_PTR:
				snprintf( buf, sizeof( buf ), "%lld", ( int64_t )( size_t )dat->m_pValue );
				SetString( keyName, buf );
				break;
			case TYPE_INT:
				snprintf( buf, sizeof( buf ), "%d", dat->m_iValue );
				SetString( keyName, buf );
				break;
			case TYPE_UINT64:
				snprintf( buf, sizeof( buf ), "%lld", *( ( unsigned long long* )( dat->m_sValue ) ) );
				SetString( keyName, buf );
				break;
			case TYPE_WSTRING:
			{
				char wideBuf[ 512 ];
				int result = UnicodeToUTF8( dat->m_wsValue, wideBuf, 512 );
				if ( result )
				{
					SetString( keyName, wideBuf );
				}
				else
				{
					return defaultValue;
				}
				break;
			}
			case TYPE_STRING:
				break;
			default:
				return defaultValue;
		};

		return dat->m_sValue;
	}
	return defaultValue;
}

const wchar_t* KeyValues::GetWString( const char* keyName, const wchar_t* defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		wchar_t wbuf[ 64 ];
		switch ( dat->m_iDataType )
		{
			case TYPE_FLOAT:
				swprintf( wbuf, Q_ARRAYSIZE( wbuf ), L"%f", dat->m_flValue );
				SetWString( keyName, wbuf );
				break;
			case TYPE_PTR:
				swprintf( wbuf, Q_ARRAYSIZE( wbuf ), L"%lld", ( int64_t )( size_t )dat->m_pValue );
				SetWString( keyName, wbuf );
				break;
			case TYPE_INT:
				swprintf( wbuf, Q_ARRAYSIZE( wbuf ), L"%d", dat->m_iValue );
				SetWString( keyName, wbuf );
				break;
			case TYPE_UINT64:
			{
				swprintf( wbuf, Q_ARRAYSIZE( wbuf ), L"%lld", *( ( unsigned long long* )( dat->m_sValue ) ) );
				SetWString( keyName, wbuf );
			}
			break;

			case TYPE_WSTRING:
				break;
			case TYPE_STRING:
			{
				int bufSize = strlen( dat->m_sValue ) + 1;
				wchar_t* pWBuf = new wchar_t[ bufSize ];
				int result = UTF8ToUnicode( dat->m_sValue, pWBuf, bufSize * sizeof( wchar_t ) );
				if ( result >= 0 )
				{
					SetWString( keyName, pWBuf );
				}
				else
				{
					delete[ ] pWBuf;
					return defaultValue;
				}
				delete[ ] pWBuf;
				break;
			}
			default:
				return defaultValue;
		};

		return ( const wchar_t* )dat->m_wsValue;
	}
	return defaultValue;
}

void* KeyValues::GetPtr( const char* keyName, void* defaultValue )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
			case TYPE_PTR:
				return dat->m_pValue;

			case TYPE_WSTRING:
			case TYPE_STRING:
			case TYPE_FLOAT:
			case TYPE_INT:
			case TYPE_UINT64:
			default:
				return NULL;
		};
	}
	return defaultValue;
}

bool KeyValues::GetBool( const char* keyName, bool defaultValue )
{
	if ( FindKey( keyName ) )
	{
		/*if (optGotDefault)
			(*optGotDefault) = false;*/

		return 0 != GetInt( keyName, 0 );
	}

	/*if (optGotDefault)
		(*optGotDefault) = true;*/

	return defaultValue;
}

Color KeyValues::GetColor( const char* keyName )
{
	Color color{ 0, 0, 0, 0 };
	KeyValues* dat = FindKey( keyName, false );
	if ( dat )
	{
		if ( dat->m_iDataType == TYPE_COLOR )
		{
			color.r = dat->m_Color[ 0 ];
			color.g = dat->m_Color[ 1 ];
			color.b = dat->m_Color[ 2 ];
			color.a = dat->m_Color[ 3 ];
		}
		else if ( dat->m_iDataType == TYPE_FLOAT )
		{
			color.r = dat->m_flValue;
		}
		else if ( dat->m_iDataType == TYPE_INT )
		{
			color.r = dat->m_iValue;
		}
		else if ( dat->m_iDataType == TYPE_STRING )
		{
			float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
			sscanf_s( dat->m_sValue, "%f %f %f %f", &a, &b, &c, &d );
			color.r = ( unsigned char )a;
			color.g = ( unsigned char )b;
			color.b = ( unsigned char )c;
			color.a = ( unsigned char )d;
		}
	}
	return color;
}

const char* KeyValues::GetName( )
{
	static auto FN = Memory->FindSignature< const char* ( __thiscall* )( KeyValues* ) >( "client.dll", "FF 31 FF 15 ? ? ? ? 83 C4" );
	return FN( this );
}

bool KeyValues::IsEmpty( const char* keyName )
{
	KeyValues* dat = FindKey( keyName, false );
	if ( !dat )
		return true;

	if ( dat->m_iDataType == TYPE_NONE && dat->m_pSub == NULL )
		return true;

	return false;
}


void KeyValues::SetWString( const char* keyName, const wchar_t* value )
{
	KeyValues* dat = FindKey( keyName, true );
	if ( dat )
	{
		delete[ ] dat->m_wsValue;
		delete[ ] dat->m_sValue;
		dat->m_sValue = NULL;

		if ( !value )
		{
			value = L"";
		}

		int len = wcslen( value );
		dat->m_wsValue = new wchar_t[ len + 1 ];
		memcpy( dat->m_wsValue, value, ( len + 1 ) * sizeof( wchar_t ) );

		dat->m_iDataType = TYPE_WSTRING;
	}
}

void KeyValues::SetString( const char* keyName, const char* value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		if ( dat->m_iDataType == TYPE_STRING && dat->m_sValue == value )
		{
			return;
		}

		delete[ ] dat->m_sValue;
		delete[ ] dat->m_wsValue;
		dat->m_wsValue = NULL;

		if ( !value )
		{
			value = "";
		}

		int len = strlen( value );
		dat->m_sValue = new char[ len + 1 ];
		memcpy( dat->m_sValue, value, len + 1 );

		dat->m_iDataType = TYPE_STRING;
	}
}

void KeyValues::SetInt( const char* keyName, int value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->m_iValue = value;
		dat->m_iDataType = TYPE_INT;
	}
}

void KeyValues::SetUint64( const char* keyName, unsigned long long value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		delete[ ] dat->m_sValue;
		delete[ ] dat->m_wsValue;
		dat->m_wsValue = NULL;

		dat->m_sValue = new char[ sizeof( unsigned long long ) ];
		*( ( unsigned long long* )dat->m_sValue ) = value;
		dat->m_iDataType = TYPE_UINT64;
	}
}

void KeyValues::SetFloat( const char* keyName, float value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->m_flValue = value;
		dat->m_iDataType = TYPE_FLOAT;
	}
}

void KeyValues::SetPtr( const char* keyName, void* value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->m_pValue = value;
		dat->m_iDataType = TYPE_PTR;
	}
}

void KeyValues::SetColor( const char* keyName, Color value )
{
	KeyValues* dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->m_iDataType = TYPE_COLOR;
		dat->m_Color[ 0 ] = value.r;
		dat->m_Color[ 1 ] = value.g;
		dat->m_Color[ 2 ] = value.b;
		dat->m_Color[ 3 ] = value.a;
	}
}