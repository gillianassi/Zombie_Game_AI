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

bool ExamInterfaceWrapper::ExesSlots()
{
	// check all spots 1 by 1 and reserve a spot for each
	int exes = 5;
	if (Agent_HasGun())
		exes -= max(1,m_pistols);

	if (Agent_HasFood())
		exes -= max(1, m_medKits);
	if (Agent_HasMedKit())
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
		return(!Agent_HasGun() || ExesSlots());
		break;

	case eItemType::MEDKIT:
		return (!Agent_HasMedKit() || ExesSlots());
		break;

	case eItemType::FOOD:
		return (!Agent_HasFood() || ExesSlots());
		break;

	default:
		return false;
		break;
	}
	return false;
}

void ExamInterfaceWrapper::UseItem(eItemType type)
{
	int slot = SearchSlotWithItem(type);
	if (slot == -1) {
		cout << "Warning! :: Item not in Inventory" << endl;
		return;
	}
	Inventory_UseItem(slot);
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

}
