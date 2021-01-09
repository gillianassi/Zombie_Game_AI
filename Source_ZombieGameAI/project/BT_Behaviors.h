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

/// Check if enemies are in sight 
/// 
/// If enemies are in the line of sight, register their data inside an EntityInfo vector.
/// This data is inserted into the blackboard.
bool ItemInSight(Elite::Blackboard* pBlackboard)
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

	ItemInfo item = {};
	for (auto& entity : *pEntitiesInFOV)
	{
		if (entity.Type == eEntityType::ITEM)
		{

			pInterface->Item_GetInfo(entity, item);
		}
	}

	//pBlackboard->ChangeData("AvoidVec", avoidVec);
	return true;
}


/// Check if a house is in sight 
/// 
/// If a house is in the line of sight insert the closest path point into the blackboard
/// As long as the agent is not in the house, store it's location to have an exit point
bool HouseInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
	vector<HouseInfo>* pHousesInFOV = nullptr;
	Elite::Vector2 exit{};
	float* houseTimer= nullptr;

	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pHousesInFOV", pHousesInFOV) &&
		pBlackboard->GetData("ExitPos",exit) &&
		pBlackboard->GetData("HouseTimer", houseTimer);

	// check if there is interface and fov data
	// + don't commence if the house timer is initiated or no houses are in sight.
	if ((!pInterface)||(!pHousesInFOV)||(*houseTimer > 0.f )|| (pHousesInFOV->size() == 0))
		return false;
	

	HouseInfo house = pHousesInFOV->front();
	AgentInfo agent = pInterface->Agent_GetInfo();
	if (!agent.IsInHouse)
	{
		pBlackboard->ChangeData("ExitPos", agent.Position);
		pBlackboard->ChangeData("HouseCenterReached", false);
		cout << "Seek exit" << endl;
	}

	pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(house.Center));
	pBlackboard->ChangeData("HouseInfo", house);
	cout << "Seek House center" << endl;

	pInterface->Draw_Point(pInterface->NavMesh_GetClosestPathPoint(house.Center), 3.f, { 0,0,1 });
	pInterface->Draw_Point(pInterface->NavMesh_GetClosestPathPoint(exit), 3.f, { 0,1,0 });
	return true;
}

/// Check if a house is in sight 
/// 
/// If a house is in the line of sight insert the closest path point into the blackboard
/// As long as the agent is not in the house, store it's location to have an exit point
bool AgentInHouse(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExtraInterfaceInfo* pInterface = nullptr;
	bool reached = false;
	Elite::Vector2 exit{};
	HouseInfo house{};

	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("HouseCenterReached", reached) &&
		pBlackboard->GetData("ExitPos", exit) &&
		pBlackboard->GetData("HouseInfo", house)
		;

	if (!pInterface)
		return false;
	AgentInfo agent = pInterface->Agent_GetInfo();
	if (!agent.IsInHouse)
		return false;
	if (reached)
	{
		pBlackboard->ChangeData("TargetPos", exit);
		cout << "center reached: going to exit" << endl;
	}
	else
	{
		if (agent.Position.DistanceSquared(pInterface->NavMesh_GetClosestPathPoint(house.Center)) < 0.5)
			pBlackboard->ChangeData("HouseCenterReached", true);
		pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(house.Center));
	}
	pInterface->Draw_Point(pInterface->NavMesh_GetClosestPathPoint(house.Center), 3.f, { 0,0,1 });
	pInterface->Draw_Point(pInterface->NavMesh_GetClosestPathPoint(exit), 3.f, { 0,1,0 });
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

	pSteering->SetToWander();
	cout << "ChangToWander" << endl;
	return Success;
}

BehaviorState ChangeToPanic(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering);

	if ((!pSteering))
		return Failure;

	pSteering->SetToWander();
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

	cout << "ChangToSeek" << endl;
	pSteering->SetToSeek(pTargetPos);

	return Success;
}

BehaviorState ChangeToEnter(Elite::Blackboard* pBlackboard)
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

	pSteering->SetToAvoid(avoidVec);
	cout << "ChangToAvoid" << endl;
	return Success;
}
#endif