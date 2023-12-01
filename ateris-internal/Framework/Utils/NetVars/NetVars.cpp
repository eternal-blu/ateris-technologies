#include "NetVars.h"
#include "../../SDK/Interfaces/Interfaces.h"

int CNetVars::GetOffset(RecvTable* Table, const char* Name)
{
	for (int i = 0; i < Table->PropsCount; i++)
	{
		auto Prop = &Table->Props[i];

		if (std::string_view(Prop->VarName).compare(Name) == 0) // Matches our name
		{
			return Prop->Offset;
		}

		if (auto DataTable = Prop->DataTable)
		{
			auto Offset = GetOffset(DataTable, Name); // Recursively loop through data table in prop

			if (Offset)
			{
				return Offset + Prop->Offset;
			}
		}
	}

	return 0;
}

int CNetVars::GetNetVar(const char* Class, const char* NetVar)
{
	const auto& Classes = gClient->GetAllClasses();

	for (auto Current = Classes; Current; Current = Current->Next)
	{
		if (std::string_view(Class).compare(Current->NetworkTable) == 0)
		{
			return GetOffset(Current->RecvTable, NetVar);
		}
	}

	return 0;
}

RecvProp* CNetVars::GetNetVarProp( RecvTable* table, const char* name )
{
	// Loop through props in RecvTable
	for ( int i = 0; i < table->PropsCount; i++ )
	{
		RecvProp* Prop = &table->Props[ i ];

		if ( !Prop )
		{
			continue;
		}

		if ( std::string_view( Prop->VarName ).compare( name ) == 0 ) // Matches our name
		{
			return Prop;
		}

		if ( auto datatable = Prop->DataTable )
		{
			RecvProp* SubProp = GetNetVarProp( datatable, name );

			if ( SubProp && std::string_view( SubProp->VarName ).compare( name ) == 0 )
			{
				return SubProp;
			}
		}
	}

	return nullptr;
}

RecvProp* CNetVars::FindNetVarProp( const char* classname, const char* netvar )
{
	auto classes = gClient->GetAllClasses( );

	for ( auto current = classes; current; current = current->Next )
	{
		if ( std::string_view( classname ).compare( current->NetworkTable ) == 0 )
		{
			return GetNetVarProp( current->RecvTable, netvar );
		}
	}

	return nullptr;
}