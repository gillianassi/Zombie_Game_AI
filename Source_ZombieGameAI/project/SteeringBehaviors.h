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

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) = 0;

	//Seek Functions
	void SetTarget(const EntityInfo& target) { m_Target = target; }


	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	EntityInfo m_Target;
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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;


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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

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
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

	void SetEvasionRadius(float evasionRadius) { m_EvasionRadius = evasionRadius; }

protected:
	Elite::Vector2 m_Predict;
	float m_PredictionScale = 1.f;
	float m_VelocityLength{};

private:
	float m_EvasionRadius = 15.f;
};
#endif


