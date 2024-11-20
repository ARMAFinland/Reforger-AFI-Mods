// Coop game mode
// Open lobby on game start
// Disable respawn logic

modded class PS_GameModeCoop : SCR_BaseGameMode
{
	void StartGame()
	{
		m_iReconnectTime = m_iReconnectTimeAfterBriefing;
		if (m_bReserveSlots)
			ReserveSlots();
		if (m_bRemoveRedundantUnits)
		{
			// --- START MODDED ---
			AFI_JIPGameModeComponent jipComponent = AFI_JIPGameModeComponent.Cast(FindComponent(AFI_JIPGameModeComponent));
			if (jipComponent)
				jipComponent.StartDeleteTimer(GetFreezeTime());
			else
				PS_PlayableManager.GetInstance().KillRedundantUnits();
			// --- END MODDED ---
		}
		restrictedZonesTimer(m_iFreezeTime);
		StartGameMode();
	}
};

