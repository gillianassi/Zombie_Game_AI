#pragma once
#include "IExamInterface.h"

class ExtraInterfaceInfo : public IExamInterface
{
public:
	//--- Constructor & Destructor ---
	ExtraInterfaceInfo() = default;
	virtual ~ExtraInterfaceInfo() = default;

	virtual bool Agent_HasGun() { return m_hasGun; };
	virtual bool Agent_HasMedKit() { return m_hasMedKit; };
	virtual bool Agent_HasFood() { return m_hasFood; };

	virtual void Quick_AddItem(ItemInfo i);
	virtual int SearchItemSlot(ItemInfo i);

protected:
	std::vector<ItemInfo> m_ItemVec[5];
	int m_CurrentSlot = 0;
	bool m_hasGun = false;
	bool m_hasMedKit = false;
	bool m_hasFood = false;
};