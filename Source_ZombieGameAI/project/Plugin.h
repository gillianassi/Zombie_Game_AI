#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "SteeringBehaviors.h"

class IBaseInterface;
class ExtraInterfaceInfo;

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin();

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	ExtraInterfaceInfo* m_pInterface = nullptr;
	vector<HouseInfo> GetHousesInFOV() const;
	vector<EntityInfo> GetEntitiesInFOV() const;
	Elite::Blackboard* CreateBlackboard(AgentSteering* steering);


	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose
	float m_timer = 0.f;

	AgentSteering* m_pAgentsteering = nullptr;
	Elite::IDecisionMaking* m_pDecisionMaking = nullptr;
	std::vector<EntityInfo> m_EntitiesInFOV{};
	std::vector<HouseInfo> m_HousesInFOV{};

};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}