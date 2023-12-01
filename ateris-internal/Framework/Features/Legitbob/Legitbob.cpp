#include "Legitbob.h"

void CLegitBob::GatherTargets()
{
	Targets.clear();

	Vector ViewAngles = gEngineClient->GetViewAngles();
	if (!gLocalPlayer || !gLocalPlayer->IsAlive())
		return;

	for (auto& Entity : gPlayers)
	{
		auto Player = reinterpret_cast<CBasePlayer*>(Entity);
		if (!Player || Player == gLocalPlayer || Player->IsDormant())
			continue;

		if (!Player->IsAlive())
			continue;

		if (Player->IsPlayer() && !Player->IsEnemy())
				continue;

		float CurrentFov = gMath->CalcFov(ViewAngles, gMath->CalcAngle(gLocalPlayer->EyePosition(), Player->WorldSpaceCenter()));

		if (CurrentFov > Config->Aimbot->Hitscan->LegitBob->FOV || CurrentFov < Config->Aimbot->Hitscan->LegitBob->DeadZone)
			continue;

		CTarget Target;
		Target.Target = Player;

		Targets.emplace_back(Target);
	}

	std::sort(Targets.begin(), Targets.end(), [](const CTarget& t1, const CTarget& t2) {
		return t1.Fov < t2.Fov;
		});
}

void CLegitBob::Run(CUserCmd* pCmd)
{
	if (!gLocalPlayer || !gLocalPlayer->IsAlive() || !gLocalWeapon || !Config->Aimbot->Hitscan->LegitBob->Enabled)
		return;

	if (!TF2->IsHitscanWeapon(gLocalWeapon))
		return;

	GatherTargets();
	CBasePlayer* BestPlayer = nullptr;
	for (auto& Player : Targets)
	{
		if (!Player.Target)
			continue;

		if (TF2->IsVisible(gLocalPlayer, Player.Target, gLocalPlayer->EyePosition(), Player.Target->GetHitboxPos(0), MASK_SHOT))
		{
			BestPlayer = Player.Target;
			break;
		}
	}

	static ConVar* Sensitivity = gCvar->FindVar("sensitivity");
	if (Sensitivity)
	{
		static float OldSens = Sensitivity->GetFloat();

		if (BestPlayer)
		{
			float HelpStrenght = gMath->ReMapValClamped((Config->Aimbot->Hitscan->LegitBob->Strenght / 100),
				0.f, 1.f, 1.f, 0.f);

			Vector EndPos = {};
			gMath->AngleVectors(pCmd->viewangles, &EndPos);
			EndPos = gLocalPlayer->EyePosition() + (EndPos * 8192.f);

			CGameTrace Trace;
			Ray_t ray;
			ray.Init(gLocalPlayer->EyePosition(), EndPos);
			CTraceFilterHitscan Filter;
			Filter.Skip = gLocalPlayer;
			gEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &Filter, &Trace);

			if (Trace.Ent == BestPlayer)
			{
				float Fov = gMath->CalcFov(gEngineClient->GetViewAngles(),
					gMath->CalcAngle(gLocalPlayer->EyePosition(), BestPlayer->GetHitboxPos(Trace.Hitbox)));

				float NewSens = gMath->ReMapValClamped(Fov, 0.f, Config->Aimbot->Hitscan->LegitBob->FOV,
					OldSens / 2, OldSens * HelpStrenght);

				if (Trace.Hitbox == 0)
					NewSens -= HelpStrenght * 2;

				Sensitivity->SetValue(NewSens);
			}
		}
		else
		{
			Sensitivity->SetValue(OldSens);
		}
	}
}