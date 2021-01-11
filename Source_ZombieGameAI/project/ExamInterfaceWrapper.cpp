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

void ExamInterfaceWrapper::AddItemToMemory(ItemInfo item)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [item](ItemRecord i)
		{return ((i.Location == item.Location) && (i.Type == item.Type)); });
	if (ItemIt == m_ItemMemory.end())
	{
		ItemRecord newItem{};
		newItem.Location = item.Location;
		newItem.Type = item.Type;
		m_ItemMemory.emplace_back(newItem);
	}
		
}


Elite::Vector2 ExamInterfaceWrapper::FindClosestItemInMemory(eItemType type)
{
	float distSqrd = FLT_MAX;
	ItemRecord closestItem{};
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [type](ItemRecord item) {return (item.Type == type); });
	while (ItemIt != m_ItemMemory.end()) {
		if (Elite::DistanceSquared(ItemIt->Location, Agent_GetInfo().Position) < distSqrd)
		{
			distSqrd = Elite::DistanceSquared(ItemIt->Location, Agent_GetInfo().Position);
			closestItem = *ItemIt;
		}
		ItemIt = std::find_if(std::next(ItemIt), m_ItemMemory.end(), [type](ItemRecord item) {return (item.Type == type); });
	}
	return closestItem.Location;
}

bool ExamInterfaceWrapper::IsItemInMemory(eItemType type)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [type](ItemRecord item) {return (item.Type == type); });
	return (ItemIt != m_ItemMemory.end());
}

void ExamInterfaceWrapper::DeleteItemInMemory(ItemInfo item)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [item](ItemRecord i)
		{return ((i.Location == item.Location) && (i.Type == item.Type)); });
	if (ItemIt != m_ItemMemory.end())
		m_ItemMemory.erase(std::remove(m_ItemMemory.begin(), m_ItemMemory.end(), *ItemIt)); // erase-remove idiom

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
	auto slotIt = std::find_if(m_ItemVec.begin(), m_ItemVec.end(),[](eItemType slot)
		{
			return (slot == eItemType::RANDOM_DROP);
		});
	if (slotIt != m_ItemVec.end())
	{
		return  slotIt - m_ItemVec.begin();
	}
	return -1;

}

int ExamInterfaceWrapper::SearchSlotWithItem(eItemType type)
{
	auto itemIt = std::find_if(m_ItemVec.begin(), m_ItemVec.end(), [&type](eItemType info)
		{
			return info == type;
		});
	if (itemIt != m_ItemVec.end())
	{
		return  itemIt - m_ItemVec.begin();
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
			m_ItemVec[slot] = eItemType::RANDOM_DROP;
			m_pistols--;
		}
		break;

	case eItemType::FOOD:
		Inventory_RemoveItem(slot);
		m_ItemVec[slot] = eItemType::RANDOM_DROP;
		m_food--;
		break;

	case eItemType::MEDKIT:
		Inventory_RemoveItem(slot);
		m_ItemVec[slot] = eItemType::RANDOM_DROP;
		m_medKits--;
		break;

	default:
		break;
	}
	return success;
}
