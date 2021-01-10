#include "stdafx.h"
#include "ExamInterfaceWrapper.h"

void ExamInterfaceWrapper::Quick_AddItem(EntityInfo i)
{
	ItemInfo item{};
	Item_Grab(i, item);
	int slot = SearchEmptyItemSlot();
	Inventory_AddItem(slot, item);
	m_freeSlots[slot] = false;
	if (item.Type == eItemType::PISTOL)
	{
		m_pistols++;
		m_hasGun = true;
	}
		
	else if (item.Type == eItemType::FOOD)
	{
		m_food++;
		m_hasFood = true;
	}
	else if (item.Type == eItemType::MEDKIT)
	{
		m_medKits++;
		m_hasMedKit = true;
	}

}

void ExamInterfaceWrapper::Quick_DropItem(int itemSlot)
{

}

bool ExamInterfaceWrapper::ExesSlots()
{
	// check all spots 1 by 1 and reserve a spot for each
	int exes = 5;
	if (m_hasGun)
		exes -= max(1,m_pistols);
	if (m_hasFood)
		exes -= max(1, m_food);
	if (m_hasMedKit)
		exes -= max(1, m_medKits);

	// return true of there are exes slots
	return exes > 0;
}

int ExamInterfaceWrapper::SearchEmptyItemSlot()
{
	auto slotIT = std::find_if(m_freeSlots.begin(), m_freeSlots.end(),[](bool free)
		{
			return (free == true);
		});
	if (slotIT != m_freeSlots.end())
	{
		return  slotIT - m_freeSlots.begin();
	}
	return -1;

}

int ExamInterfaceWrapper::SearchSlotWithItem(eItemType type)
{
	auto itemIT = std::find_if(m_ItemVec.begin(), m_ItemVec.end(), [&type](ItemInfo info)
		{
			return info.Type == type;
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
		return(!m_hasGun || ExesSlots());
		break;

	case eItemType::FOOD:
		return (!m_hasFood || ExesSlots());
		break;

	case eItemType::MEDKIT:
		return (!m_hasMedKit || ExesSlots());
		break;

	default:
		return false;
		break;
	}
	return false;
}

void ExamInterfaceWrapper::Shoot()
{
	if (!m_hasGun)
		cout << "No Gun" << endl;
	else
	{
		int slot = SearchSlotWithItem(eItemType::PISTOL);
		Inventory_UseItem(slot);
		Inventory_GetItem
	}

}
