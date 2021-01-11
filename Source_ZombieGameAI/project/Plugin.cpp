// precompiled Header
#include "stdafx.h"

// includes
#include "Plugin.h"
#include "IExamInterface.h"
#include "ExamInterfaceWrapper.h"
#include "BT_Behaviors.h"

Plugin::~Plugin()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pAgentsteering);
	SAFE_DELETE(m_pInterface);
	SAFE_DELETE(m_pInterfaceWrapper);
	SAFE_DELETE(m_pBlackboard);
}
//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);
	m_pInterfaceWrapper = new ExamInterfaceWrapper(m_pInterface);
	m_pInterfaceWrapper->Agent_HasFood();
	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Champion";
	info.Student_FirstName = "Gillian";
	info.Student_LastName = "Assi";
	info.Student_Class = "2DAE01";

	// Steering behaviour
	m_pAgentsteering = new AgentSteering();
	m_EntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)
	m_pBlackboard = CreateBlackboard(m_pAgentsteering);
	BehaviorTree* pBT = new BehaviorTree(m_pBlackboard,
		new BehaviorSelector
		({
			new BehaviorSequence(
				{
					new BehaviorConditional(NeedFood),
					new BehaviorAction(ChangeToEat)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(NeedHeal),
					new BehaviorAction(ChangeToHeal)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(HasGun),
					new BehaviorSelector(
					{
						new BehaviorSequence(
						{
							new BehaviorConditional(EnemyInSight),
							new BehaviorSelector(
							{
								new BehaviorSequence(
								{
									new BehaviorConditional(EnemyAligned),
									new BehaviorAction(ChangeToShoot)
								}),
								new BehaviorAction(ChangeToFace)
							})
						}),
						new BehaviorSequence(
						{
							new BehaviorConditional(IsBitten),
							new BehaviorAction(ChangeToRotate)
						})
					})
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(PurgeZoneInSight),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(ItemInSight),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(RememberNececity),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(AgentInHouse),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(HouseInSight),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(EnemyInSight),
					new BehaviorAction(ChangeToAvoid)
				}),
			new BehaviorSequence(
				{
					new BehaviorConditional(OutOfBounds),
					new BehaviorAction(ChangeToSeek)
				}),
			new BehaviorAction(ChangeToWander)
		})
	);
	m_pAgentsteering->SetDecisionMaking(pBT);
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called when the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_Run = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_Run = false;
	}
}

//Update
//This function calculates the new SteeringPlugin_Output, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterfaceWrapper->Agent_GetInfo();
	auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance
	m_HousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	m_EntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	// Handle steeirng
	m_pAgentsteering->CalculateSteering(dt, agentInfo);
	steering = m_pAgentsteering->GetAgentSteering();

	// Handle Timers
	m_TimeNow = std::chrono::system_clock::now();
	if (!agentInfo.Bitten)
	{
		float elapsedBiteTime = float(std::chrono::duration_cast<std::chrono::seconds>(m_TimeNow - m_lastBitten).count());
		// Run if you have enough stamina, or you are bitten
		if (agentInfo.Stamina < 8.f)
			m_CanRun = false;
		else if (agentInfo.Stamina == 10.f)
			m_CanRun = true;
		m_Run = ((elapsedBiteTime < 1.5f)||(m_CanRun));
		std::cout << agentInfo.Stamina << endl;
		std::cout << m_CanRun << endl;
		m_AgentRage = (elapsedBiteTime < 2.f);
	}
	else
		m_lastBitten = std::chrono::system_clock::now();
	
	bool inPurgeZone = false;
	m_pBlackboard->GetData("InPurgeZone", inPurgeZone);
	if (!inPurgeZone)
	{
		float elapsedFleeTime = float(std::chrono::duration_cast<std::chrono::seconds>(m_TimeNow - m_LastInsidePurge).count());
		m_FleePurge = (elapsedFleeTime < 4.f);
	}
	else
		m_LastInsidePurge = std::chrono::system_clock::now();

	// check if agent is out of bounds + timer
	if (DistanceSquared(m_pInterfaceWrapper->Agent_GetInfo().Position, m_pInterfaceWrapper->World_GetInfo().Center) > 
		Square(m_pInterfaceWrapper->World_GetInfo().Dimensions.x - 75) || m_Returning)
	{
		float elapsedReturnTime = float(std::chrono::duration_cast<std::chrono::seconds>(m_TimeNow - m_LastOutOfBounds).count());
		m_Returning = (elapsedReturnTime < 3.f);
	}
	else 
		m_LastOutOfBounds = std::chrono::system_clock::now();

	m_pInterfaceWrapper->Draw_Circle(m_pInterfaceWrapper->World_GetInfo().Center,
		m_pInterfaceWrapper->World_GetInfo().Dimensions.x -75, { 1,0,0 });
	// Let agent run when nececarry
	steering.RunMode = m_Run;
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	//m_pInterfaceWrapper->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterfaceWrapper->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterfaceWrapper->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

Blackboard* Plugin::CreateBlackboard(AgentSteering* pSteering)
{
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	pBlackboard->AddData("pAgentSteering", pSteering);
	pBlackboard->AddData("pInterface", m_pInterfaceWrapper);
	pBlackboard->AddData("pEntitiesInFOV", &m_EntitiesInFOV);
	pBlackboard->AddData("pHousesInFOV", &m_HousesInFOV);
	pBlackboard->AddData("TargetPos", Elite::Vector2{});
	pBlackboard->AddData("ExitPos", Elite::Vector2{});
	pBlackboard->AddData("AvoidVec", std::vector<EntityInfo>{});
	pBlackboard->AddData("HouseInfo", HouseInfo{});
	pBlackboard->AddData("pAgentRage", &m_AgentRage);
	pBlackboard->AddData("WanderAngle", 1.7f);
	pBlackboard->AddData("InPurgeZone", false);
	pBlackboard->AddData("NeedItem", false);
	pBlackboard->AddData("ItemNeededPos", Elite::Vector2{});
	pBlackboard->AddData("ItemNeededType", eItemType{});
	pBlackboard->AddData("SearchBuilding", false);
	pBlackboard->AddData("pReturning", &m_Returning);
	pBlackboard->AddData("pFleePurge", &m_FleePurge);
	pBlackboard->AddData("HouseCenterReached", false);
	//pBlackboard->AddData("DebugRender", false);

	return pBlackboard;
}