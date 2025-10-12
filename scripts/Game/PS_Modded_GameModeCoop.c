// Coop game mode
// Open lobby on game start
// Disable respawn logic

modded class PS_GameModeCoop : SCR_BaseGameMode
{
	override void StartGame()
	{
		m_iReconnectTime = m_iReconnectTimeAfterBriefing;
		if (m_bReserveSlots)
			ReserveSlots();
		if (m_bRemoveRedundantUnits)
		{
			// --- START MODDED ---
			AFI_JIPGameModeComponent jipComponent = AFI_JIPGameModeComponent.Cast(FindComponent(AFI_JIPGameModeComponent));
			if (!jipComponent) // In case AFI JIP isn't registered we can just remove the unites. Otherwise AFI JIP will handle it
				PS_PlayableManager.GetInstance().RemoveRedundantUnits();				
			// --- END MODDED ---
		}
		restrictedZonesTimer(m_iFreezeTime);
		StartGameMode();
	}
};

