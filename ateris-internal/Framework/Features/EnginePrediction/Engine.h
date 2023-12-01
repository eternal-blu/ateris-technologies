#pragma once
#include "../../SDK/TF2/TF2.h"

class CEnginePrediction
{
public:
	void Predict(CUserCmd*);
	void End(CUserCmd*);
private:
	CMoveData MoveData = {};
	float OldCurtime = 0.f, OldFrameTime = 0.f;
	int OldTickcount = 0;
	int GetTickBase(CUserCmd*);

	bool OldInPrediction = false, OldFirstPrediction = false;
};

inline CEnginePrediction* Prediction = new CEnginePrediction;