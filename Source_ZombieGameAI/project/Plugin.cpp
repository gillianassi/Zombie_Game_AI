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
	Blackboard* pB = CreateBlackboard(m_pAgentsteering);
	BehaviorTree* pBT = new BehaviorTree(pB, 
		new BehaviorSelector({
			new BehaviorSequence(
				{
					new BehaviorConditional(ItemInSight),
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
					new BehaviorAction(ChangeToFace)
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
		m_CanRun = true;
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
		m_CanRun = false;
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
	for (auto& e : m_EntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterfaceWrapper->PurgeZone_GetInfo(e, zoneInfo);
			std::cout << "Purge Zone in FOV:" << e.Location.x << ", " << e.Location.y << " ---EntityHash: " << e.EntityHash << "---Radius: " << zoneInfo.Radius << std::endl;
		}
	}

	//INVENTORY USAGE DEMO
	//********************
	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterfaceWrapper->Inventory_UseItem(0);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterfaceWrapper->Inventory_RemoveItem(0);
	}
	
	
	// Handle steeirng
	m_pAgentsteering->CalculateSteering(dt, agentInfo);
	steering = m_pAgentsteering->GetAgentSteering();

	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	//steering.AutoOrient = true; //Setting AutoOrientate to TRue overrides the AngularVelocity
	if (agentInfo.WasBitten)
	{
		m_CanRun = true;
		m_Runtimer = 2.f;
	}
	else if (m_Runtimer > 0)
		m_Runtimer -= dt;
	else
		m_CanRun = false;
	// timer before agent enters a new house
	if (agentInfo.IsInHouse)
		m_Housetimer = 4.f;
	else if (m_Housetimer > 0)
		m_Housetimer -= dt;
	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

								 //SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

								 //@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterfaceWrapper->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
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
	//pBlackboard->AddData("pTargetEntity", static_cast<EntityInfo*>(nullptr));
	pBlackboard->AddData("TargetPos", Elite::Vector2{});
	pBlackboard->AddData("ExitPos", Elite::Vector2{});
	pBlackboard->AddData("AvoidVec", std::vector<EntityInfo>{});
	pBlackboard->AddData("HouseInfo", HouseInfo{});
	pBlackboard->AddData("RunTimer", 0.0f);
	pBlackboard->AddData("HouseTimer", &m_Housetimer);
	pBlackboard->AddData("Running", false);
	pBlackboard->AddData("HouseCenterReached", false);
	//pBlackboard->AddData("DebugRender", false);

	return pBlackboard;
}