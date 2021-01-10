/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "Exam_HelperStructs.h"
struct AgentInfo;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) = 0;

	//Seek Functions
	void SetTarget(Elite::Vector2 target) { m_Target = target; }


	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	Elite::Vector2 m_Target{};
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;
};

//////////////////////////
//WANDER
//******
class Wander : public ISteeringBehavior
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;

	//Wander Functions
	void SetWanderOffset(float offset) { m_Offset = offset; }

protected:
	float m_Offset = 10.f; //Offset (Agent Direction)
	float m_Radius = 4.f; //WanderRadius
	float m_AngleChange = ToRadians(45); //Max Wander Angle change per frame
	float m_WanderAngle = 0.f; //Internal

};

///////////////////////////////////////
//FLEE
//****
class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Flee Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;
};

///////////////////////////////////////
//FACE
//****
class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Face Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;


};
///////////////////////////////////////
//Rotate
//****
class Rotate : public ISteeringBehavior
{
public:
	Rotate() = default;
	virtual ~Rotate() = default;

	//Evade Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;

private:
	float m_EvasionRadius = 15.f;
};

class AgentSteering
{
public:
	AgentSteering();
	virtual ~AgentSteering();

	virtual void CalculateSteering(float dt, AgentInfo agentInfo);

	virtual void SetSteeringBehavior(ISteeringBehavior* pBehavior) { m_pSteeringBehavior = pBehavior; }
	ISteeringBehavior* GetSteeringBehavior() const { return m_pSteeringBehavior; }
	void SetDecisionMaking(Elite::IDecisionMaking* decisionMakingStructure) { m_DecisionMaking = decisionMakingStructure; }
	SteeringPlugin_Output GetAgentSteering() { return m_Agentsteering; }
	void SetToWander();
	void SetToRotate();
	void SetToSeek(Elite::Vector2 targetPos);
	void SetToFlee(Elite::Vector2 targetPos);
	void SetToFace(Elite::Vector2 targetPos);

protected:
	SteeringPlugin_Output m_Agentsteering{};
	Elite::IDecisionMaking* m_DecisionMaking = nullptr;
	ISteeringBehavior* m_pSteeringBehavior = nullptr;
	Wander* m_pWander = nullptr;
	Seek* m_pSeek = nullptr;
	Flee* m_pFlee = nullptr;
	Face* m_pFace = nullptr;
	Rotate* m_pRotate = nullptr;
};
#endif


