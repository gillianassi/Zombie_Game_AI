#pragma once
#include "IExamInterface.h"

class ExamInterfaceWrapper
{
public:
	//--- Constructor & Destructor ---
	ExamInterfaceWrapper(IExamInterface* pInterface) { m_pInterface = pInterface; }
	virtual ~ExamInterfaceWrapper()
	{
		SAFE_DELETE(m_pInterface);
	};
	/////////////////////////////////////
	//			Wraped Functions
	/////////////////////////////////////
		//WORLD & ENTITIES
	virtual WorldInfo World_GetInfo() { return m_pInterface->World_GetInfo(); }
	virtual StatisticsInfo World_GetStats() { return m_pInterface->World_GetStats(); }

	virtual bool Fov_GetHouseByIndex(UINT index, HouseInfo& houseInfo) { return m_pInterface->Fov_GetHouseByIndex(index,houseInfo); }
	virtual bool Fov_GetEntityByIndex(UINT index, EntityInfo& enemyInfo) { return m_pInterface->Fov_GetEntityByIndex(index, enemyInfo); }

	virtual AgentInfo Agent_GetInfo() { return m_pInterface->Agent_GetInfo(); };
	virtual bool Enemy_GetInfo(EntityInfo entity, EnemyInfo& enemy) { return m_pInterface->Enemy_GetInfo(entity, enemy); }

	//NAVMESH
	virtual Elite::Vector2 NavMesh_GetClosestPathPoint(Elite::Vector2 goal) { return m_pInterface->NavMesh_GetClosestPathPoint(goal); }

	//INVENTORY
	virtual bool Inventory_AddItem(UINT slotId, ItemInfo item) { return m_pInterface->Inventory_AddItem(slotId,item); }
	virtual bool Inventory_UseItem(UINT slotId) { return m_pInterface->Inventory_UseItem(slotId); }
	virtual bool Inventory_RemoveItem(UINT slotId) { return m_pInterface->Inventory_RemoveItem(slotId); }
	virtual bool Inventory_GetItem(UINT slotId, ItemInfo& item) { return m_pInterface->Inventory_GetItem(slotId, item); }
	virtual UINT Inventory_GetCapacity() { return m_pInterface->Inventory_GetCapacity(); }


	virtual bool Item_GetInfo(EntityInfo entity, ItemInfo& item) { return m_pInterface->Item_GetInfo(entity, item); }
	virtual bool Item_Grab(EntityInfo entity, ItemInfo& item) { return m_pInterface->Item_Grab(entity,item); }
	virtual bool Item_Destroy(EntityInfo entity) { return m_pInterface->Item_Destroy(entity); }

	virtual int Weapon_GetAmmo(ItemInfo& item) { return m_pInterface->Weapon_GetAmmo(item); }
	virtual int Medkit_GetHealth(ItemInfo& item) { return m_pInterface->Medkit_GetHealth(item); }
	virtual int Food_GetEnergy(ItemInfo& item) { return m_pInterface->Food_GetEnergy(item); }


	//PURGEZONE
	virtual bool PurgeZone_GetInfo(EntityInfo entity, PurgeZoneInfo& zone) { return m_pInterface->PurgeZone_GetInfo(entity,zone); }

	//DEBUG
	virtual Elite::Vector2 Debug_ConvertScreenToWorld(Elite::Vector2 screenPos) { return m_pInterface->Debug_ConvertScreenToWorld(screenPos); }
	virtual Elite::Vector2 Debug_ConvertWorldToScreen(Elite::Vector2 worldPos) { return m_pInterface->Debug_ConvertWorldToScreen(worldPos); }

	//INPUT
	virtual bool Input_IsKeyboardKeyDown(Elite::InputScancode key) { return m_pInterface->Input_IsKeyboardKeyDown(key); }
	virtual bool Input_IsKeyboardKeyUp(Elite::InputScancode key) { return m_pInterface->Input_IsKeyboardKeyUp(key); }
	virtual bool Input_IsMouseButtonDown(Elite::InputMouseButton button) { return m_pInterface->Input_IsMouseButtonDown(button); }
	virtual bool Input_IsMouseButtonUp(Elite::InputMouseButton button) { return m_pInterface->Input_IsMouseButtonUp(button); }
	virtual Elite::MouseData Input_GetMouseData(Elite::InputType type, Elite::InputMouseButton button = Elite::InputMouseButton(0)) { return m_pInterface->Input_GetMouseData(type, button); }

	//EVENT
	virtual void RequestShutdown() { return m_pInterface->RequestShutdown(); };

	//RENDERER
	virtual void Draw_Polygon(const Elite::Vector2* points, int count, const Elite::Vector3& color, float depth) { return m_pInterface->Draw_Polygon(points,count,color,depth); }
	void Draw_Polygon(const Elite::Vector2* points, int count, const Elite::Vector3& color) { return m_pInterface->Draw_Polygon(points, count, color); }
	virtual void Draw_SolidPolygon(const Elite::Vector2* points, int count, const Elite::Vector3& color, float depth, bool triangulate = false) { return m_pInterface->Draw_SolidPolygon(points, count, color); }
	void Draw_SolidPolygon(const Elite::Vector2* points, int count, const Elite::Vector3& color) { return m_pInterface->Draw_SolidPolygon(points, count, color); }
	virtual void Draw_Circle(const Elite::Vector2& center, float radius, const Elite::Vector3& color, float depth) { return m_pInterface->Draw_Circle(center, radius, color,depth); }
	void Draw_Circle(const Elite::Vector2& center, float radius, const Elite::Vector3& color) { return m_pInterface->Draw_Circle(center, radius, color); }
	virtual void Draw_SolidCircle(const Elite::Vector2& center, float32 radius, const Elite::Vector2& axis, const Elite::Vector3& color, float depth) { return m_pInterface->Draw_SolidCircle(center,radius,axis,color,depth); }
	void Draw_SolidCircle(const Elite::Vector2& center, float32 radius, const Elite::Vector2& axis, const Elite::Vector3& color) { return m_pInterface->Draw_SolidCircle(center,radius,axis,color); }
	virtual void Draw_Segment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Vector3& color, float depth) { return m_pInterface->Draw_Segment(p1,p2,color,depth); }
	void Draw_Segment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Vector3& color) { return m_pInterface->Draw_Segment(p1, p2, color); }
	virtual void Draw_Direction(const Elite::Vector2& p, Elite::Vector2 dir, float length, const Elite::Vector3& color, float depth = 0.9f) { return m_pInterface->Draw_Direction(p,dir,length,color,depth); }
	virtual void Draw_Transform(const b2Transform& xf, float depth) { return m_pInterface->Draw_Transform(xf,depth); }
	virtual void Draw_Point(const Elite::Vector2& p, float size, const Elite::Vector3& color, float depth) { return m_pInterface->Draw_Point(p, size, color, depth); }
	void Draw_Point(const Elite::Vector2& p, float size, const Elite::Vector3& color) { return m_pInterface->Draw_Point(p,size,color); }
	/////////////////////////////////////
	//			Wraped Functions
	/////////////////////////////////////
	virtual bool Agent_HasGun() { return m_hasGun; };
	virtual bool Agent_HasMedKit() { return m_hasMedKit; };
	virtual bool Agent_HasFood() { return m_hasFood; };

	virtual void Quick_AddItem(EntityInfo i);
	virtual void Quick_DropItem(int itemSlot);
	virtual bool ExesSlots();
	virtual int SearchSlotWithItem(ItemInfo i);
	virtual int SearchEmptyItemSlot();
	virtual bool CanGrab(ItemInfo i);
	virtual bool Shoot();

protected:
	IExamInterface* m_pInterface = nullptr;
	std::vector<ItemInfo> m_ItemVec = std::vector<ItemInfo>(5);
	std::vector<bool> m_freeSlots = std::vector<bool>(5, true);
	int m_CurrentSlot = 0;
	bool m_hasGun =false, m_hasFood=false, m_hasMedKit = false;
	int m_pistols = 0, m_food = 0, m_medKits = 0;
};