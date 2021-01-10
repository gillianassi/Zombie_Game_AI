//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "IExamInterface.h"

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	steering.LinearVelocity = m_Target - agentInfo.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//WANDER (base> SEEK)
//******
SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	steering.LinearVelocity = m_Target - agentInfo.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//FLEE
//****
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	
	steering.LinearVelocity = -(m_Target - agentInfo.Position); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed

	return steering;
}

//FACE
//****
SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};

	const Elite::Vector2 toTarget{ m_Target - agentInfo.Position };

	const  float to{ atan2f(toTarget.y, toTarget.x) + float(E_PI_2) };
	float from{ agentInfo.Orientation };
	from = atan2f(sinf(from), cosf(from));
	float desired = to - from;

	const float Pi2 = float(E_PI) * 2.f;
	if (desired > E_PI)
		desired -= Pi2;
	else if (desired < -E_PI)
		desired += Pi2;

	// multiply desired by some value to make it go as fast as possible (30.f)
	steering.AngularVelocity = desired * 30.f;

	steering.AutoOrient = false;

	return steering;
}
//Rotate
//****
SteeringPlugin_Output Rotate::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	// Let him turn
	steering.AutoOrient = false;
	steering.AngularVelocity = agentInfo.MaxAngularSpeed;

	return steering;

}

//STEERING FUNCTIONS
//*******************
AgentSteering::AgentSteering()
{
	//Create the possible steering behaviors for the agent
	m_pWander = new Wander();
	m_pSeek = new Seek();
	m_pFlee = new Flee();
	m_pFace = new Face();
	m_pRotate = new Rotate();
}

AgentSteering::~AgentSteering()
{
	SAFE_DELETE(m_DecisionMaking);
	SAFE_DELETE(m_pSteeringBehavior);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pFace);
	SAFE_DELETE(m_pRotate);
}

void AgentSteering::CalculateSteering(float dt, AgentInfo agentInfo)
{
	if (m_DecisionMaking)
		m_DecisionMaking->Update(dt);

	if (m_pSteeringBehavior)
		m_Agentsteering = m_pSteeringBehavior->CalculateSteering(dt, agentInfo);
}

void AgentSteering::SetToWander()
{
	SetSteeringBehavior(m_pWander);
}

void AgentSteering::SetToRotate()
{
	SetSteeringBehavior(m_pRotate);
}

void AgentSteering::SetToSeek(Elite::Vector2 targetPos)
{
	m_pSeek->SetTarget(targetPos);
	SetSteeringBehavior(m_pSeek);
}

void AgentSteering::SetToFlee(Elite::Vector2 targetPos)
{
	m_pFlee->SetTarget(targetPos);
	SetSteeringBehavior(m_pFlee);
}

void AgentSteering::SetToFace(Elite::Vector2 TargetPos)
{
	m_pFace->SetTarget(TargetPos);
	SetSteeringBehavior(m_pFace);
}

