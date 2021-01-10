#include "stdafx.h"
#include "ExamInterfaceWrapper.h"

bool ExamInterfaceWrapper::Inventory_RemoveItem(UINT slotId)
{
	m_ItemVec[slotId] = eItemType::RANDOM_DROP;
	return m_pInterface->Inventory_RemoveItem(slotId);
}

void ExamInterfaceWrapper::Quick_AddItem(EntityInfo i)
{
	ItemInfo item{};
	Item_Grab(i, item);
	int slot = SearchEmptyItemSlot();
	if (slot == -1)
		cout << "No Free Inventory Slots" << endl;
	else 
	{
		Inventory_AddItem(slot, item);
		m_ItemVec[slot] = item.Type;
		if (item.Type == eItemType::PISTOL)
		{
			m_pistols++;
		}
		else if (item.Type == eItemType::MEDKIT)
		{
			m_medKits++;
		}
		else if (item.Type == eItemType::FOOD)
		{
			m_food++;
		}
		m_ItemVec[slot] = item.Type;
	}
}

void ExamInterfaceWrapper::UpdateHouseMemory(Elite::Vector2 pos)
{
	m_HouseMemory.push_back(pos);
	if (m_HouseMemory.size() > 3)
		m_HouseMemory.pop_front();
}

bool ExamInterfaceWrapper::SearchedHouseBefore(Elite::Vector2 pos)
{
	return (std::find(m_HouseMemory.begin(), m_HouseMemory.end(), pos) != m_HouseMemory.end());
}

int ExamInterfaceWrapper::GetItemStats(eItemType type)
{
	int data = 0;
	ItemInfo item;
	Inventory_GetItem(SearchSlotWithItem(type), item);
	if (type == eItemType::PISTOL)
		data = Weapon_GetAmmo(item);
	else if (type == eItemType::MEDKIT)
		data = Medkit_GetHealth(item);
	else
		data = Food_GetEnergy(item);

	return data;
}

bool ExamInterfaceWrapper::ExcessSlots()
{
	// check all spots 1 by 1 and reserve a spot for each
	int exes = 5;
	
	exes -= max(1,m_pistols);
	exes -= max(1, m_medKits);
	exes -= max(1, m_food);

	// return true of there are exes slots
	return exes > 0;
}

int ExamInterfaceWrapper::SearchEmptyItemSlot()
{
	auto slotIT = std::find_if(m_ItemVec.begin(), m_ItemVec.end(),[](eItemType slot)
		{
			return (slot == eItemType::RANDOM_DROP);
		});
	if (slotIT != m_ItemVec.end())
	{
		return  slotIT - m_ItemVec.begin();
	}
	return -1;

}

int ExamInterfaceWrapper::SearchSlotWithItem(eItemType type)
{
	auto itemIT = std::find_if(m_ItemVec.begin(), m_ItemVec.end(), [&type](eItemType info)
		{
			return info == type;
		});
	if (itemIT != m_ItemVec.end())
	{
		return  itemIT - m_ItemVec.begin();
	}
	return -1;

}

bool ExamInterfaceWrapper::CanGrab(ItemInfo i)
{
	switch (i.Type)
	{
	case eItemType::PISTOL:
		return(!Agent_HasGun() || ExcessSlots());
		break;

	case eItemType::MEDKIT:
		return (!Agent_HasMedKit() || ExcessSlots());
		break;

	case eItemType::FOOD:
		return (!Agent_HasFood() || ExcessSlots());
		break;

	default:
		return false;
		break;
	}
	return false;
}

bool ExamInterfaceWrapper::UseItem(eItemType type)
{
	int slot = SearchSlotWithItem(type);
	if (slot == -1) {
		cout << "Warning! :: Item not in Inventory" << endl;
		return false;
	}
	bool success = Inventory_UseItem(slot);
	ItemInfo item{};
	Inventory_GetItem(slot, item);
	// delete if it is not a gun, if it is a gun, delete if ammo reaches 0
	switch (type)
	{
	case eItemType::PISTOL:
		if (Weapon_GetAmmo(item) == 0)
		{
			Inventory_RemoveItem(slot);
			m_pistols--;
		}
		break;

	case eItemType::FOOD:
		Inventory_RemoveItem(slot);
		m_food--;
		break;

	case eItemType::MEDKIT:
		Inventory_RemoveItem(slot);
		m_medKits--;
		break;

	default:
		break;
	}
	return success;
}
