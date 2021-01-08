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
	void SetTarget(Elite::Vector2 target) { m_pTarget = target; }


	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	Elite::Vector2 m_pTarget{};
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
	float m_Offset = 6.f; //Offset (Agent Direction)
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
//AVOID
//****
class Avoid : public ISteeringBehavior
{
public:
	Avoid() = default;
	virtual ~Avoid() = default;

	//Flee Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;
	void SetEntitiesToAvoid(vector<EntityInfo> avoidVec) { m_avoidVec = avoidVec; }

private:
	vector<EntityInfo> m_avoidVec{};
};

///////////////////////////////////////
//ARRIVE
//****
class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Arrive Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;
	void SetSlowRadius(float slowRadius) { m_SlowRadius = slowRadius; }
	void SetTargetRadius(float targetRadius) { m_TargetRadius = targetRadius; }

protected:
	float m_MaxDistance = 10.0f; 
	float m_Distance = 0.f; //Internal
	float m_SlowRadius = 3.0f; // When it starts slowing down
	float m_TargetRadius = 1.0f; // When it starts slowing down

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


protected:
	float m_desired = 0.f; // Internal
	float m_current = 0.f; //Internal

};

///////////////////////////////////////
//PURSUIT
//****
class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	//Pursuit Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;

protected:
	Elite::Vector2 m_Predict;
	float m_PredictionScale = 1.5f;
};

///////////////////////////////////////
//EVADE
//****
class Evade : public ISteeringBehavior
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Evade Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo agentInfo) override;

	void SetEvasionRadius(float evasionRadius) { m_EvasionRadius = evasionRadius; }

protected:
	Elite::Vector2 m_Predict;
	float m_PredictionScale = 1.f;
	float m_VelocityLength{};

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
	void SetToSeek(Elite::Vector2 targetPos);
	void SetToFlee(Elite::Vector2 targetPos);
	void SetToAvoid(vector<EntityInfo> avoidVec);

protected:
	SteeringPlugin_Output m_Agentsteering{};
	Elite::IDecisionMaking* m_DecisionMaking = nullptr;
	ISteeringBehavior* m_pSteeringBehavior = nullptr;
	Wander* m_pWander = nullptr;
	Seek* m_pSeek = nullptr;
	Flee* m_pFlee = nullptr;
	Avoid* m_pAvoid = nullptr;
};
#endif


