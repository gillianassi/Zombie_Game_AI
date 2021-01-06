/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for BT
/*=============================================================================*/
#pragma once
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../inc/EliteMath/EMath.h"
#include "../inc/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
//#include "IExamInterface.h"
#include "SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool EnemyInSight(Elite::Blackboard* pBlackboard)
{
	AgentInfo* pAgent = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
		return false;

	//TODO: Check if enemy is in sight
	/*
	const float closeToRange{ pAgent->GetRange() + 10 };
	auto foodIT = std::find_if(foodVec->begin(), foodVec->end(), [&pAgent, &closeToRange](AgarioFood* f)
		{
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange);
		});
	if (foodIT != foodVec->end())
	{
		pBlackboard->ChangeData("Target", (*foodIT)->GetPosition());
		return true;
	}*/
	return false;
}

bool IsCloseToAgent(Elite::Blackboard* pBlackboard)
{
	return false;
}

bool IsCloseToAlfaAgent(Elite::Blackboard* pBlackboard)
{
	
	return false;
}
bool IsCloseToBetaAgent(Elite::Blackboard* pBlackboard)
{
	
	return false;
}



BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	/*AgarioAgent* pAgent = nullptr;
	bool debugRender = false;
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("DebugRender", debugRender);

	if (!pAgent)
		return Failure;

	pAgent->SetToWander();
	if (debugRender)
	{
		std::cout << "Wandering..." << std::endl;
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), (pAgent->GetRange() + 10), { 0,1,0 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), (pAgent->GetRange() + 7), { 0,0,1 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), (pAgent->GetRange()), { 1,0,0 }, 0.4f);
	}*/
	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	
	return Success;
}

BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	
	return Success;
}

#endif