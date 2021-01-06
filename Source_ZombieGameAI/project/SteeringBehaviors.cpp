//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "IExamInterface.h"

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	steering.LinearVelocity = (m_Target).Location - pAgent.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= pAgent.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//WANDER (base> SEEK)
//******
SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	
	float random = ((float)rand()) / (float)RAND_MAX;
	m_WanderAngle = (m_WanderAngle - m_AngleChange/2) + (random * m_AngleChange);
	Elite::Vector2 origin = pAgent.Position + pAgent.LinearVelocity.GetNormalized() * m_Offset;
	Elite::Vector2 wanderPoint = Elite::Vector2(origin.x + m_Radius * cos(m_WanderAngle), origin.y + m_Radius * sin(m_WanderAngle));
	steering.LinearVelocity = wanderPoint - pAgent.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= pAgent.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//FLEE
//****
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	
	steering.LinearVelocity = -((m_Target).Location - pAgent.Position); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= pAgent.MaxLinearSpeed; // Rescale to Max speed

	return steering;
}

//ARRIVE
//****
SteeringPlugin_Output Arrive::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	
	SteeringPlugin_Output steering{};
	/*
	m_Distance = (m_Target).Position.Distance(pAgent->GetPosition()) - m_TargetRadius;
	Elite::Vector2 desiredVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	desiredVelocity.Normalize(); // get unit vector
	if (m_Distance < m_SlowRadius)
		desiredVelocity *= pAgent->GetMaxLinearSpeed() * (m_Distance / m_SlowRadius);
		//(pAgent->GetMaxLinearSpeed()-pAgent->GetMaxLinearSpeed()*4/ m_Distance); // Rescale to var speed
	else
		desiredVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to Max speed

	steering.LinearVelocity = desiredVelocity - steering.LinearVelocity;
	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	*/
	return steering;
}

//FACE
//****
SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	/*
	Elite::Vector2 vec = (m_Target).Position - pAgent->GetPosition(); // desired orientation
	m_desired = atan2(vec.y, vec.x);
	m_current = pAgent->GetRotation() - ToRadians(90); 
	printf("current: %f,    desired: %f \n", ToDegrees(m_current),ToDegrees(m_desired));
	steering.AngularVelocity = (m_desired - m_current)*4; //Desired Velocity

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), vec, 5, { 0,1,1 }, 0.4f);
	}
		*/
	return steering;
}

//PURSUIT
//****
SteeringPlugin_Output Pursuit::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	/*
	m_Predict = (m_Target).Position + (m_Target).LinearVelocity * m_PredictionScale;
	steering.LinearVelocity = m_Predict - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to Max speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(m_Predict, 5.0f, { 1,0,0 }, 0.4f);
	}*/
	return steering;
}

//EVADE
//****
SteeringPlugin_Output Evade::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};
	/*
	float distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);

	if (distanceToTarget > m_EvasionRadius)
	{
		SteeringPlugin_Output steering;
		steering.IsValid = false;
		return steering;
	}

	m_VelocityLength = (m_Target).LinearVelocity.Magnitude();
	if (distanceToTarget < m_VelocityLength)
	{
		m_PredictionScale = distanceToTarget / (m_VelocityLength);
	}
	else { m_PredictionScale = 1.f; }
	
	m_Predict = (m_Target).Position + (m_Target).LinearVelocity * m_PredictionScale;
	steering.LinearVelocity = -(m_Predict - pAgent->GetPosition()); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to Max speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 1,0,0 }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(m_Predict, 5.0f, { 1,0,0 }, 0.4f);
	}*/
	return steering;
}