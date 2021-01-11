#include "stdafx.h"
#include "ExamInterfaceWrapper.h"

bool ExamInterfaceWrapper::Quick_RemoveItem(UINT slotId)
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

	exes -= max(1, m_pistols);
	exes -= max(1, m_medKits);
	exes -= max(1, m_food);

	// return true of there are exes slots
	return (exes > 0);
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

int ExamInterfaceWrapper::SearchEmptyItemSlot()
{
	auto slotIt = std::find_if(m_ItemVec.begin(), m_ItemVec.end(), [](eItemType info)
		{
			return (info == eItemType::RANDOM_DROP);
		});
	if (slotIt != m_ItemVec.end())
	{
		return  slotIt - m_ItemVec.begin();
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
	if(!success)
		m_ItemVec[slot] = eItemType::RANDOM_DROP;
	else {
		ItemInfo item{};
		Inventory_GetItem(slot, item);
		// delete if it is not a gun, if it is a gun, delete if ammo reaches 0
		switch (type)
		{
		case eItemType::PISTOL:
			if (Weapon_GetAmmo(item) == 0)
			{
				Quick_RemoveItem(slot);
				m_pistols--;
			}
			break;
		case eItemType::MEDKIT:
			Quick_RemoveItem(slot);
			m_medKits--;
			break;
		case eItemType::FOOD:
			Quick_RemoveItem(slot);
			m_food--;
			break;

		default:
			break;
		}
	}
	return success;
}

void ExamInterfaceWrapper::AddItemToMemory(ItemInfo item)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [item](ItemRecord i)
		{return ((i.Location == item.Location)); });
	if (ItemIt == m_ItemMemory.end())
	{
		ItemRecord newItem{};
		newItem.Location = item.Location;
		newItem.Type = item.Type;
		m_ItemMemory.emplace_back(newItem);

		switch (item.Type)
		{
		case eItemType::PISTOL:
			m_RememberPistol = true;
			break;

		case eItemType::MEDKIT:
			m_RememberMedKit = true;
			break;
		case eItemType::FOOD:
			m_RememberFood = true;
			break;

		default:
			break;
		}
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
bool ExamInterfaceWrapper::IsItemInMemory(Elite::Vector2 itemPos)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [itemPos](ItemRecord item) {return (item.Location == itemPos); });
	return (ItemIt != m_ItemMemory.end());
}

void ExamInterfaceWrapper::DeleteItemFromMemory(Elite::Vector2 itemPos)
{
	auto ItemIt = std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), [itemPos](ItemRecord i)
		{return (i.Location == itemPos); });
	if (ItemIt != m_ItemMemory.end())
	{
		eItemType type = ItemIt->Type;
		m_ItemMemory.erase(std::remove(m_ItemMemory.begin(), m_ItemMemory.end(), *ItemIt)); // erase-remove idiom

		switch (type)
		{
		case eItemType::PISTOL:
			m_RememberPistol = IsItemInMemory(type);
			break;

		case eItemType::MEDKIT:
			m_RememberMedKit = IsItemInMemory(type);
			break;
		case eItemType::FOOD:
			m_RememberFood = IsItemInMemory(type);
			break;
		default:
			break;
		}
	}
	else
		cout << "delete failed" << endl;
}

bool ExamInterfaceWrapper::SearchedHouseBefore(Elite::Vector2 pos)
{
	return (std::find(m_HouseMemory.begin(), m_HouseMemory.end(), pos) != m_HouseMemory.end());
}

void ExamInterfaceWrapper::UpdateHouseMemory(Elite::Vector2 pos)
{
	if (std::find(m_HouseMemory.begin(), m_HouseMemory.end(), pos) == m_HouseMemory.end())
	{
		m_HouseMemory.push_back(pos);
		if (m_HouseMemory.size() > 2)
			m_HouseMemory.pop_front();
	}
}