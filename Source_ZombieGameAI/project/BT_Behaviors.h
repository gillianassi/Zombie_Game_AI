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
// Behavior Conditions
//-----------------------------------------------------------------

/// Check if enemies are in sight 
/// 
/// If enemies are in the line of sight, register their data inside an EntityInfo vector.
/// This data is inserted into the blackboard.
bool EnemyInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
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

/// Check if a house is in sight 
/// 
/// If a house is in the line of sight insert this data into the blackboard.
bool HouseInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
	vector<HouseInfo>* pHousesInFOV = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pHousesInFOV", pHousesInFOV);
	if ((!pInterface)||(!pHousesInFOV))
		return false;
	
	// Check for enemies in FOV
	if (pHousesInFOV->size() == 0)
		return false;

	HouseInfo house = pHousesInFOV->front();

	pBlackboard->ChangeData("House", house);
	return true;
}

/// Check if agent was bitten 
/// 
/// Return true, if the agent was bitten.
bool IsBitten(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;

	AgentInfo agentInfo = pInterface->Agent_GetInfo();
	if(agentInfo.WasBitten)
		return true;

	return false;
}
/// Check if agent is safe 
/// 
/// An agent is concidered safe when the kill countdown is high enough, it was not bitten,
/// it has a gun and is inside of a house.
bool IsSafe(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;
	StatisticsInfo stats = pInterface->World_GetStats();
	AgentInfo agentInfo = pInterface->Agent_GetInfo();
	cout << stats.KillCountdown << endl;
	if (stats.KillCountdown > 60.f && !agentInfo.WasBitten && agentInfo.IsInHouse)
		return true;

	return false;
}
bool IsCloseToBetaAgent(Elite::Blackboard* pBlackboard)
{
	
	return false;
}
//-----------------------------------------------------------------
// Behavior States
//-----------------------------------------------------------------


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