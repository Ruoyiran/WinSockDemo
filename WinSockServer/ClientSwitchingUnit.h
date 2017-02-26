#pragma once
class ClientSwitchingUnit {
public:
	ClientSwitchingUnit();
	void SetBulbState(BOOL isOn);
	BOOL GetBulbState();

private:
	BOOL m_bulbIsOn;
};