#include "stdafx.h"
#include "ExtraInterfaceInfo.h"

void ExtraInterfaceInfo::Quick_AddItem(ItemInfo i)
{
	ItemInfo item;
	//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
	//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
	//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
	//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
	if (Item_Grab({}, item))
	{
		//Once grabbed, you can add it to a specific inventory slot
		//Slot must be empty
		Inventory_AddItem(0, i);
	}
}

int ExtraInterfaceInfo::SearchItemSlot(ItemInfo i)
{
	auto itemIT = std::find_if(m_ItemVec->begin(), m_ItemVec->end(), [&i](ItemInfo info)
		{
			return info.Type == i.Type;
		});
	if (itemIT != m_ItemVec->end())
	{
		return  itemIT - m_ItemVec->begin();
	}
	return -1;

}
