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
#include "SteeringBehaviors.h"
#include <chrono>

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
	ExamInterfaceWrapper* pInterface = nullptr;
	vector<EntityInfo>* pEntitiesInFOV = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pEntitiesInFOV", pEntitiesInFOV);
	if ((!pInterface)||(!pEntitiesInFOV))
		return false;
	
	// Check for enemies in FOV
	if (pEntitiesInFOV->size() == 0)
		return false;

	std::vector<EntityInfo> avoidVec{};
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

bool PurgeZoneInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	vector<EntityInfo>* pEntitiesInFOV = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pEntitiesInFOV", pEntitiesInFOV);
	if ((!pInterface)||(!pEntitiesInFOV))
		return false;
	
	// Check for enemies in FOV
	if (pEntitiesInFOV->size() == 0)
		return false;

	for (auto& entity : *pEntitiesInFOV)
	{
		if (entity.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo purgeZoneInfo{};
			pInterface->PurgeZone_GetInfo(entity, purgeZoneInfo);
			Elite::Vector2 dir = -(purgeZoneInfo.Center - pInterface->Agent_GetInfo().Position);
			Elite::Vector2 targetPos = dir.GetNormalized()*(purgeZoneInfo.Radius+5) + pInterface->Agent_GetInfo().Position;
			pInterface->Draw_Point(pInterface->NavMesh_GetClosestPathPoint(targetPos), 3.f, { 1,0,0 });
			pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(targetPos));
			return true;
		}
	}
	return false;
}

/// Check if enemies are in sight 
/// 
/// If enemies are in the line of sight, register their data inside an EntityInfo vector.
/// This data is inserted into the blackboard.
bool ItemInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	vector<EntityInfo>* pEntitiesInFOV = nullptr;
	Elite::Vector2 itemNeededPos{};
	eItemType itemNeededType{};
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pEntitiesInFOV", pEntitiesInFOV)&&
		pBlackboard->GetData("ItemNeededPos", itemNeededPos)&&
		pBlackboard->GetData("ItemNeededType", itemNeededType);
	if ((!pInterface)||(!pEntitiesInFOV))
		return false;
	
	// Check for enemies in FOV
	if (pEntitiesInFOV->size() == 0)
		return false;
	//pInterface->Agent_HasFood();
	ItemInfo item = {};
	EntityInfo closestEntity{};
	AgentInfo agent = pInterface->Agent_GetInfo();
	bool success = false;
	for (auto& entity : *pEntitiesInFOV)
	{
		if (entity.Type == eEntityType::ITEM)
		{
			pInterface->Item_GetInfo(entity, item);
			if (pInterface->CanGrab(item))
			{
				if (agent.Position.DistanceSquared(entity.Location) < Square(agent.GrabRange))
				{
					if ((itemNeededPos == item.Location) && (itemNeededType == item.Type))
					{
						pBlackboard->ChangeData("NeedItem", false);
						pInterface->DeleteItemInMemory(item);
					}
					cout << "Grabbed Item" << endl;
					pInterface->Quick_AddItem(entity);
				}
				else
				{
					closestEntity = entity;
					success = true;
				}
			}
			else if (item.Type != eItemType::GARBAGE)
				pInterface->AddItemToMemory(item);

		}
	}
	if(success)
		pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(closestEntity.Location));
	return success;
}


/// Check if a house is in sight 
/// 
/// If a house is in the line of sight insert the closest path point into the blackboard
/// As long as the agent is not in the house, store it's location to have an exit point
bool HouseInSight(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	vector<HouseInfo>* pHousesInFOV = nullptr;
	Elite::Vector2 exit{};
	bool searching = false;
	HouseInfo houseInfo{};

	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pHousesInFOV", pHousesInFOV) &&
		pBlackboard->GetData("ExitPos", exit) &&
		pBlackboard->GetData("SearchBuilding", searching) &&
		pBlackboard->GetData("HouseInfo", houseInfo);

	// check if the data is succesfully feched
	if ((!pInterface)||(!pHousesInFOV))
		return false;
	// don't commence if the house timer is initiated or no houses are in sight.
	if ((pHousesInFOV->size() == 0) && (!searching))
		return false;

	HouseInfo house{};
	if (searching)
		house = houseInfo;
	else
		house = pHousesInFOV->front();
	
	AgentInfo agent = pInterface->Agent_GetInfo();
	//don't mind houses that the agent already visited
	if (pInterface->SearchedHouseBefore(house.Center))
		return false;
	else
		pBlackboard->ChangeData("SearchBuilding", true);
	// agent will only do this when he is outside of a house
	pBlackboard->ChangeData("ExitPos", agent.Position);
	pBlackboard->ChangeData("HouseCenterReached", false);
		//cout << "Seek exit" << endl;

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
	ExamInterfaceWrapper* pInterface = nullptr;
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
	if (agent.Position.DistanceSquared(house.Center) < 0.5)
	{
		// Agent can go to exit
		pBlackboard->ChangeData("HouseCenterReached", true);
		// push in house memory
		pInterface->UpdateHouseMemory(house.Center);
		//Stop search
		pBlackboard->ChangeData("SearchBuilding", false);
	}
	if (reached)
		pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(exit));
	else
	{
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
	bool* pAgentRage = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentRage", pAgentRage);
	if (!pAgentRage)
		return false;

	return *pAgentRage;
}

bool RememberNececity(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	bool needItem = false;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface)&&
		pBlackboard->GetData("NeedItem",needItem);

	if (!pInterface)
		return false;
	Elite::Vector2 itemPos{};
	eItemType type{};
	if (!needItem) {
		if (!pInterface->Agent_HasGun() && pInterface->IsItemInMemory(eItemType::PISTOL))
		{
			itemPos = pInterface->FindClosestItemInMemory(eItemType::PISTOL);
			type = eItemType::PISTOL;
			needItem = true;
		}
		else if (!pInterface->Agent_HasMedKit() && pInterface->IsItemInMemory(eItemType::MEDKIT))
		{
			itemPos = pInterface->FindClosestItemInMemory(eItemType::MEDKIT);
			type = eItemType::MEDKIT;
			needItem = true;
		}
		else if (!pInterface->Agent_HasFood() && pInterface->IsItemInMemory(eItemType::FOOD))
		{
			itemPos = pInterface->FindClosestItemInMemory(eItemType::FOOD);
			type = eItemType::FOOD;
			needItem = true;
		}
		if (needItem)
		{
			std::cout << "Need Item!" << endl;
			pBlackboard->ChangeData("NeedItem", true);
			pBlackboard->ChangeData("ItemNeededPos", itemPos);
			pBlackboard->ChangeData("ItemNeededType", itemPos);
			pBlackboard->ChangeData("TargetPos", pInterface->NavMesh_GetClosestPathPoint(itemPos));
			return true;
		}
	}
	return false;

}


bool NeedHeal(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;

	if (pInterface->Agent_HasMedKit())
	{
		AgentInfo agent = pInterface->Agent_GetInfo();
		int heal = pInterface->GetItemStats(eItemType::MEDKIT);
		if ((10 - agent.Health) > heal)
			return true;
	}
	return false;
}
bool NeedFood(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;

	if (pInterface->Agent_HasFood())
	{
		AgentInfo agent = pInterface->Agent_GetInfo();
		int boost = pInterface->GetItemStats(eItemType::FOOD);
		if ((10 - agent.Energy) > boost)
			return true;
	}
	return false;
}
bool OutOfBounds(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	bool* pReturning = false;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pReturning", pReturning);
	if ((!pInterface)&&(!pReturning))
		return false;
	
	if (!*pReturning)
		return false;
	cout << "Returning" << endl;
	pBlackboard->ChangeData("TargetPos", pInterface->World_GetInfo().Center);
	return true;
}
bool HasGun(Elite::Blackboard* pBlackboard)
{
	//Get data from blackboard
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);
	if (!pInterface)
		return false;
	if(pInterface->Agent_HasGun())
		return true;
	else
		return false;
}
bool EnemyAligned(Elite::Blackboard* pBlackboard)
{
	ExamInterfaceWrapper* pInterface = nullptr;
	std::vector<EntityInfo> avoidVec{};
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("AvoidVec", avoidVec) ;

	if ((!pInterface)||(avoidVec.size() == 0))
		return false;


	float closestDist = FLT_MAX;
	EntityInfo closestEnemy{};
	AgentInfo agent = pInterface->Agent_GetInfo();
	Elite::Vector2 pos = agent.Position;
	for (auto& enemy : avoidVec)
	{
		if (Elite::DistanceSquared(enemy.Location, pos) < closestDist)
		{
			closestEnemy = enemy;
			closestDist = Elite::DistanceSquared(enemy.Location, pos);
		}
	}
	// Change param for face behavior if it's not alligned
	// Just enemy location, not closest path, Could cause agent to incorrectly face if the agent is next to a corner
	pBlackboard->ChangeData("TargetPos", closestEnemy.Location);
	// Get vector to agent
	Elite::Vector2 dir = closestEnemy.Location - agent.Position;
	// Get agent orientation
	float oriRad = float(agent.Orientation - (double)b2_pi * 0.5);
	Elite::Vector2 orientation = { cos(oriRad), sin(float(oriRad)) };

	return (Dot(orientation.GetNormalized(), dir.GetNormalized()) >= 0.998);
}
//-----------------------------------------------------------------
// Behavior States
//-----------------------------------------------------------------


BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	ExamInterfaceWrapper* pInterface = nullptr;
	AgentSteering* pSteering = nullptr;
	float wanderAngle{};
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) && 
		pBlackboard->GetData("pAgentSteering", pSteering)&&
		pBlackboard->GetData("WanderAngle", wanderAngle);

	if ((!pSteering))
		return Failure;

	//Calculate a wander point
	float offset = 8.f; //Offset (Agent Direction)
	float radius = 4.f; //WanderRadius
	float angleChange = ToRadians(45); //Max Wander Angle change per frame
	AgentInfo agentInfo = pInterface->Agent_GetInfo();

	float random = ((float)rand()) / (float)RAND_MAX;
	wanderAngle = (wanderAngle - angleChange / 2) + (random * angleChange);
	pBlackboard->ChangeData("WanderAngle", wanderAngle);
	Elite::Vector2 origin = agentInfo.Position + agentInfo.LinearVelocity.GetNormalized() * offset;
	Elite::Vector2 wanderPoint = Elite::Vector2(origin.x + radius * cos(wanderAngle), origin.y + angleChange * sin(wanderAngle));
	// Use getClosestPathPoint to avoid running into walls
	pInterface->Draw_Point(wanderPoint, 3.f, { 1,0,0 });
	
	pSteering->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(wanderPoint));
	cout << "Wander" << endl;
	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	Elite::Vector2 TargetPos{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", TargetPos);

	if (!pSteering)
		return Failure;

	cout << "Seek" << endl;
	pSteering->SetToSeek(TargetPos);

	return Success;
}

BehaviorState ChangeToEnter(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	Elite::Vector2 TargetPos{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", TargetPos);

	if (!pSteering)
		return Failure;

	pSteering->SetToSeek(TargetPos);

	return Success;
}

BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	Elite::Vector2 TargetPos{};
	auto dataAvailable = pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", TargetPos);

	if (!pSteering)
		return Failure;

	pSteering->SetToFlee(TargetPos);
	cout << "Flee" << endl;
	return Success;
}

BehaviorState ChangeToAvoid(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	std::vector<EntityInfo> avoidVec{};
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("AvoidVec", avoidVec);

	if ((!pSteering)||(!pInterface))
		return Failure;

	float distSqrd = 0;
	Elite::Vector2 vec{};
	Elite::Vector2 direction{};
	AgentInfo agentInfo = pInterface->Agent_GetInfo();
	Elite::Vector2 linearVel = agentInfo.LinearVelocity.GetNormalized();
	for (auto& enemy : avoidVec)
	{
		distSqrd = Elite::DistanceSquared(enemy.Location, agentInfo.Position);
		// Vector to the enemy
		vec = (enemy.Location - agentInfo.Position).GetNormalized();
		// closer targets have more influence on the turn direction 
		// *10 is to avoid too small numbers dur to the inversional proportionality
		direction += (linearVel - vec) * agentInfo.MaxAngularSpeed * 30 / distSqrd;
	}
	float oriRad = float(agentInfo.Orientation - (double)b2_pi * 0.5);
	Elite::Vector2 orientation = { cos(oriRad), sin(float(oriRad)) };

	linearVel = direction + orientation; //Desired Velocity
	linearVel.Normalize(); // get unit vector
	Elite::Vector2 target =pInterface->NavMesh_GetClosestPathPoint(linearVel*8.f + agentInfo.Position);
	pInterface->Draw_Point(target, 3.f, { 1,0,0 });
	pSteering->SetToSeek(target);
	cout << "Avoid" << endl;
	return Success;
}
BehaviorState ChangeToFace(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	ExamInterfaceWrapper* pInterface = nullptr;
	Elite::Vector2 targetPos{};
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) && 
		pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", targetPos);

	if ((!pSteering) || (!pInterface))
		return Failure;

	//pInterface->Draw_Point(closestEnemy.Location, 20.f, { 1,0,0 });
	pSteering->SetToFace(targetPos);
	cout << "Face" << endl;
	return Success;
}

BehaviorState ChangeToShoot(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	ExamInterfaceWrapper* pInterface = nullptr;
	Elite::Vector2 targetPos{};
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) && 
		pBlackboard->GetData("pAgentSteering", pSteering) &&
		pBlackboard->GetData("TargetPos", targetPos);

	if ((!pSteering) || (!pInterface))
		return Failure;

	//pInterface->Draw_Point(closestEnemy.Location, 20.f, { 1,0,0 });
	pInterface->UseItem(eItemType::PISTOL);
	pSteering->SetToFace(targetPos);
	cout << "Shoot" << endl;
	return Success;
}

BehaviorState ChangeToRotate(Elite::Blackboard* pBlackboard)
{
	AgentSteering* pSteering = nullptr;
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface) &&
		pBlackboard->GetData("pAgentSteering", pSteering);

	if ((!pSteering) || (!pInterface))
		return Failure;

	pSteering->SetToRotate();
	cout << "Rotate" << endl;
	return Success;
}
BehaviorState ChangeToHeal(Elite::Blackboard* pBlackboard)
{
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);

	if (!pInterface)
		return Failure;

	if (!pInterface->UseItem(eItemType::MEDKIT))
		return Failure;

	return Success;
}
BehaviorState ChangeToEat(Elite::Blackboard* pBlackboard)
{
	ExamInterfaceWrapper* pInterface = nullptr;
	auto dataAvailable = pBlackboard->GetData("pInterface", pInterface);

	if (!pInterface)
		return Failure;

	if (!pInterface->UseItem(eItemType::FOOD))
		return Failure;

	return Success;
}
#endif