#pragma once
#include "../../../SDK/TF2/TF2.h"

enum {
	PARTITION_ENGINE_SOLID_EDICTS = (1 << 0),		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
	PARTITION_ENGINE_TRIGGER_EDICTS = (1 << 1),		// every edict_t that IS SOLID_TRIGGER
	PARTITION_CLIENT_SOLID_EDICTS = (1 << 2),
	PARTITION_CLIENT_RESPONSIVE_EDICTS = (1 << 3),		// these are client-side only objects that respond to being forces, etc.
	PARTITION_ENGINE_NON_STATIC_EDICTS = (1 << 4),		// everything in solid & trigger except the static props, includes SOLID_NOTs
	PARTITION_CLIENT_STATIC_PROPS = (1 << 5),
	PARTITION_ENGINE_STATIC_PROPS = (1 << 6),
	PARTITION_CLIENT_NON_STATIC_EDICTS = (1 << 7),		// everything except the static props
};


class CEntitySphereQuery
{
public:
	CEntitySphereQuery(const Vector Center, const float Radius, const int FLAG_MASK = 0, const int PARTITION_MASK = PARTITION_CLIENT_NON_STATIC_EDICTS)
	{
		static auto func = Memory->FindSignature< void(__thiscall*)(void*, const Vector&, float, int, int)>("client.dll", "55 8B EC 83 EC 14 D9 45 0C");
		if (func)
			func(this, Center, Radius, FLAG_MASK, PARTITION_MASK);
	}

	CBaseEntity* CurrentEntity()
	{
		return (ListIndex < ListCount) ? List[ListIndex] : nullptr;
	}

	void NextEntity()
	{
		ListIndex++;
	}

private:
	int ListIndex {}, ListCount {};
	CBaseEntity* List[MAX_SPHERE_QUERY] {};
};

class CAutoDetonation
{
public:
	void Run(CUserCmd*);
};

inline CAutoDetonation* AutoDetonation = new CAutoDetonation;