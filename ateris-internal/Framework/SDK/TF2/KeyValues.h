#pragma once
#include "Color.h"

enum types_t
{
	TYPE_NONE = 0,
	TYPE_STRING,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_PTR,
	TYPE_WSTRING,
	TYPE_COLOR,
	TYPE_UINT64,
	TYPE_NUMTYPES,
};

class KeyValues
{
public:
	int m_iKeyName;
	char* m_sValue;
	wchar_t* m_wsValue;

	union
	{
		int m_iValue;
		float m_flValue;
		void* m_pValue;
		unsigned char m_Color[ 4 ];
	};

	char m_iDataType;
	char m_bHasEscapeSequences;
	char m_bEvaluateConditionals;
	char unused[ 1 ];

	KeyValues* m_pPeer;
	KeyValues* m_pSub;
	KeyValues* m_pChain;


	bool LoadFromBuffer( char const* resource_name, const char* buffer, void* file_system = 0, const char* path_id = 0 );
	void Initialize( char* name );
	KeyValues( const char* name );
	KeyValues* FindKey( const char* keyName, bool bCreate = false );
	KeyValues* AddSubkey( KeyValues* subKey );

	int GetInt( const char* keyName, int defaultValue = 0 );
	unsigned long long GetUint64( const char* keyName, unsigned long long defaultValue = 0 );
	float GetFloat( const char* keyName, float defaultValue = 0.0f );
	const char* GetString( const char* keyName, const char* defaultValue = "" );
	const wchar_t* GetWString( const char* keyName, const wchar_t* defaultValue = L"" );
	void* GetPtr( const char* keyName, void* defaultValue = ( void* )0 );
	bool GetBool( const char* keyName, bool defaultValue = false );
	Color GetColor( const char* keyName );
	const char* GetName( );
	bool IsEmpty( const char* keyName );

	void SetWString( const char* keyName, const wchar_t* value );
	void SetString( const char* keyName, const char* value );
	void SetInt( const char* keyName, int value );
	void SetUint64( const char* keyName, unsigned long long value );
	void SetFloat( const char* keyName, float value );
	void SetPtr( const char* keyName, void* value );
	void SetColor( const char* keyName, Color value );
	void SetBool( const char* keyName, bool value ) { SetInt( keyName, value ? 1 : 0 ); }
};