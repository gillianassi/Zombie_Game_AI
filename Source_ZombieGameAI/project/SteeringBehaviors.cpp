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
	steering.LinearVelocity = m_pTarget - agentInfo.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//WANDER (base> SEEK)
//******
SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	
	float random = ((float)rand()) / (float)RAND_MAX;
	m_WanderAngle = (m_WanderAngle - m_AngleChange/2) + (random * m_AngleChange);
	Elite::Vector2 origin = agentInfo.Position + agentInfo.LinearVelocity.GetNormalized() * m_Offset;
	Elite::Vector2 wanderPoint = Elite::Vector2(origin.x + m_Radius * cos(m_WanderAngle), origin.y + m_Radius * sin(m_WanderAngle));
	steering.LinearVelocity = wanderPoint - agentInfo.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed
	
	return steering;
}

//FLEE
//****
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	
	steering.LinearVelocity = -(m_pTarget - agentInfo.Position); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed

	return steering;
}
//Avoid
//****
SteeringPlugin_Output Avoid::CalculateSteering(float deltaT, AgentInfo agentInfo)
{

	SteeringPlugin_Output steering{};
	float distSqrd = 0;
	Elite::Vector2 toEemy{};
	Elite::Vector2 direction{};
	for (auto& enemy : m_avoidVec)
	{
		distSqrd = Elite::DistanceSquared(enemy.Location, agentInfo.Position);
		toEemy = (enemy.Location - agentInfo.Position).GetNormalized();
		// closer targets have more influence on the turn direction 
		// *10 is to avoid too small numbers dur to the inversional proportionality
		direction += (agentInfo.LinearVelocity.GetNormalized() - toEemy) * agentInfo.MaxAngularSpeed * 10 / distSqrd;
	}
	//Get agent orientation
	float oriRad = float(agentInfo.Orientation - (double)b2_pi * 0.5);
	Elite::Vector2 orientation = { cos(oriRad), sin(float(oriRad)) };

	steering.LinearVelocity = direction+ orientation; //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; // Rescale to Max speed

	return steering;
}

//FACE
//****
SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	float distSqrd = FLT_MAX;
	Elite::Vector2 toEemy{};
	Elite::Vector2 direction{};
	EntityInfo closestEnemy{};
	for (auto& enemy : m_avoidVec)
	{
		if (distSqrd < Elite::DistanceSquared(enemy.Location, agentInfo.Position))
		{
			closestEnemy = enemy;
			distSqrd = Elite::DistanceSquared(enemy.Location, agentInfo.Position);
		}
	}
	Elite::Vector2 vec = closestEnemy.Location - agentInfo.Position; // desired orientation
	m_desired = atan2(vec.y, vec.x);
	/*m_current = agentInfo- ToRadians(90); 
	printf("current: %f,    desired: %f \n", ToDegrees(m_current),ToDegrees(m_desired));
	steering.AngularVelocity = (m_desired - m_current)*4; //Desired Velocity

	//DEBUG RENDERING
	if (agentInfo->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(agentInfo->GetPosition(), vec, 5, { 0,1,1 }, 0.4f);
	}
		*/
	return steering;
}

//PURSUIT
//****
SteeringPlugin_Output Pursuit::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	/*
	m_Predict = (m_Target).Position + (m_Target).LinearVelocity * m_PredictionScale;
	steering.LinearVelocity = m_Predict - agentInfo->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo->GetMaxLinearSpeed(); // Rescale to Max speed

	//DEBUG RENDERING
	if (agentInfo->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(agentInfo->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(m_Predict, 5.0f, { 1,0,0 }, 0.4f);
	}*/
	return steering;
}

//EVADE
//****
SteeringPlugin_Output Evade::CalculateSteering(float deltaT, AgentInfo agentInfo)
{
	SteeringPlugin_Output steering{};
	/*
	float distanceToTarget = Distance(agentInfo->GetPosition(), m_Target.Position);

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
	steering.LinearVelocity = -(m_Predict - agentInfo->GetPosition()); //Desired Velocity
	steering.LinearVelocity.Normalize(); // get unit vector
	steering.LinearVelocity *= agentInfo->GetMaxLinearSpeed(); // Rescale to Max speed

	//DEBUG RENDERING
	if (agentInfo->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(agentInfo->GetPosition(), steering.LinearVelocity, 5, { 1,0,0 }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(m_Predict, 5.0f, { 1,0,0 }, 0.4f);
	}*/
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
	m_pAvoid = new Avoid();
	m_pAvoid = new Face();
}

AgentSteering::~AgentSteering()
{
	SAFE_DELETE(m_DecisionMaking);
	SAFE_DELETE(m_pSteeringBehavior);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pAvoid);
	SAFE_DELETE(m_pFace);
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

void AgentSteering::SetToAvoid(vector<EntityInfo> avoidVec)
{
	m_pAvoid->SetEntitiesToAvoid(avoidVec);
	SetSteeringBehavior(m_pAvoid);
}

void AgentSteering::SetToAvoid(vector<EntityInfo> avoidVec)
{
	m_pAvoid->SetEntitiesToFace(avoidVec);
	SetSteeringBehavior(m_pAvoid);
}

