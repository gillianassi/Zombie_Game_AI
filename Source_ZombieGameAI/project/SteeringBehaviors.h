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
#include "SteeringHelpers.h"
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }


	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//Seek Functions
	virtual void SetTarget(const TargetData& target) { m_Target = target; }

protected:
	const TargetData* m_pTargetRef = nullptr;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;


protected:
	const TargetData* m_pTargetRef = nullptr;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetSlowRadius(float slowRadius) { m_SlowRadius = slowRadius; }
	void SetTargetRadius(float targetRadius) { m_TargetRadius = targetRadius; }

protected:
	const TargetData* m_pTargetRef = nullptr;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;


protected:
	const TargetData* m_pTargetRef = nullptr;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	const TargetData* m_pTargetRef = nullptr;
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
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetEvasionRadius(float evasionRadius) { m_EvasionRadius = evasionRadius; }

protected:
	const TargetData* m_pTargetRef = nullptr;
	Elite::Vector2 m_Predict;
	float m_PredictionScale = 1.f;
	float m_VelocityLength{};

private:
	float m_EvasionRadius = 15.f;
};
#endif


