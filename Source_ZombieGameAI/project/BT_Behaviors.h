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
#include "EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
//#include "IExamInterface.h"
#include "SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool EnemyInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	IExamInterface* pInterface = nullptr;
	vector<EntityInfo>* pEntitiesInFOV = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pEntitiesInFOV", pEntitiesInFOV);
	if ((!pInterface)||(!pEntitiesInFOV))
		return false;
	
	// Check for enemies in FOV
	if (pEntitiesInFOV->size() == 0)
		return false;

	std::vector<EntityInfo> avoidVec = {};
	for (auto& entity : *pEntitiesInFOV)
	{
		if (entity.Type == eEntityType::ENEMY)
		{
			avoidVec.push_back(entity);
		}
	}
	if (avoidVec.size() == 0)
		return false;

	pBlackboard->ChangeData("AvoidVec", avoidVec);
	return true;
}

bool IsBitten(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	IExamInterface* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;

	AgentInfo agentInfo = pInterface->Agent_GetInfo();
	if(agentInfo.Bitten)
		return true;

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
	AgentSteering* pSteering = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering);

	if ((!pSteering))
		return Failure;

	//pSteering->SetToWander();
	cout << "ChangToWander" << endl;
	return Success;
}

BehaviorState ChangeToPanic(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering);

	if ((!pSteering))
		return Failure;

	//pSteering->SetToWander();
	cout << "ChangToWander" << endl;
	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	Elite::Vector2 pTargetPos{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", pTargetPos);

	if (!pSteering)
		return Failure;

	pSteering->SetToSeek(pTargetPos);

	return Success;
}

BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	Elite::Vector2 pTargetPos{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", pTargetPos);

	if (!pSteering)
		return Failure;

	pSteering->SetToFlee(pTargetPos);
	cout << "ChangToFlee" << endl;
	return Success;
}

BehaviorState ChangeToAvoid(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	std::vector<EntityInfo> avoidVec{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("AvoidVec", avoidVec);

	if (!pSteering)
		return Failure;

	//pSteering->SetToAvoid(avoidVec);
	cout << "ChangToAvoid" << endl;
	return Success;
}

#endif