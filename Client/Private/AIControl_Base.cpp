#include "stdafx.h"
#include "AIControl_Base.h"

_int CAIControl_Base::Get_RandomNumber(_int MaxCount)
{
	
		random_device rd;
		mt19937 gen(rd());

		uniform_int_distribution<int> distributionID(0, MaxCount);

	return distributionID(gen);
}
