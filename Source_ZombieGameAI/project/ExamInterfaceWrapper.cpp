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

int ExamInterfaceWrapper::SearchSlotWithItem(ItemInfo i)
{
	auto itemIT = std::find_if(m_ItemVec.begin(), m_ItemVec.end(), [&i](ItemInfo info)
		{
			return info.Type == i.Type;
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
		if (!m_hasGun)
			return true;
		if (ExesSlots())
			return true;
		break;

	case eItemType::FOOD:
		if (!m_hasFood)
			return true;
		if (ExesSlots())
			return true;
		break;

	case eItemType::MEDKIT:
		if (!m_hasMedKit)
			return true;
		if (ExesSlots())
			return true;
		break;

	default:
		return false;
		break;
	}
	return false;
}

bool ExamInterfaceWrapper::Shoot()
{
	return false;
}
