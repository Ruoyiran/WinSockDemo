#include "stdafx.h"
#include "ClientSwitchingUnit.h"

ClientSwitchingUnit::ClientSwitchingUnit():
	m_bulbIsOn(FALSE)
{
}

void ClientSwitchingUnit::SetBulbState(BOOL isOn) {
	m_bulbIsOn = isOn;
}

BOOL ClientSwitchingUnit::GetBulbState()
{
	return m_bulbIsOn;
}
