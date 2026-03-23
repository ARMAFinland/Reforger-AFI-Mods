class AFI_CheckTicketsAction : ScriptedUserAction
{
	[Attribute()]
	FactionKey m_factionKey;
	
	protected PS_GameModeCoop m_gameMode;
	protected PS_GameModeCoop GetGameMode()
	{
		if (!m_gameMode)
			m_gameMode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		
		return m_gameMode;
	}
	
	override bool CanBroadcastScript() { return false; };
	
	override bool GetActionNameScript(out string outName)
	{ 
		if (!m_factionKey || m_factionKey == "")
		{
			outName = "Check respawn info";
			return true;
		}
		
		outName = "Check respawn info for " + m_factionKey;
		return true; 
	};
	
	protected string FormatTimeMS(int timeMs)
	{
	    int totalSeconds = timeMs / 1000;
	    int minutes = totalSeconds / 60;
	    int seconds = totalSeconds % 60;
	
	    string minLabel;
	    string secLabel;
	
	    if (minutes == 1)
	    {
	        minLabel = "minute";
	    }
	    else
	    {
	        minLabel = "minutes";
	    }
	
	    if (seconds == 1)
	    {
	        secLabel = "second";
	    }
	    else
	    {
	        secLabel = "seconds";
	    }
	
	    if (minutes > 0)
	    {
	        return string.Format("%1 %2 %3 %4", minutes, minLabel, seconds, secLabel);
	    }
	
	    return string.Format("%1 %2", seconds, secLabel);
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		FactionKey factionKey = m_factionKey;
		if (!m_factionKey || m_factionKey == "")
		{
			FactionAffiliationComponent factionAffliationComponent = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
			if (factionAffliationComponent)
			{
				factionKey = factionAffliationComponent.GetAffiliatedFactionKey();
			}
		}
		
		if (!factionKey)
		{
			Print("Unable to find FactionKey from this script or pUserEntity", LogLevel.WARNING);
			return;
		}
		
		PS_FactionRespawnCount factionRespawns = GetGameMode().GetFactionRespawnCountPub(factionKey);
		if (!factionRespawns)
		{
			PrintFormat("Unable to find faction respawn count for %1", factionKey);
			return;
		}
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		int playerId = playerManager.GetPlayerIdFromControlledEntity(pUserEntity);
		if (playerId == 0)
			return;
		
		PlayerController playerController = playerManager.GetPlayerController(playerId);
		if (!playerController)
			return;
		
		AFI_PlayerControllerComponent afiComponent = AFI_PlayerControllerComponent.Cast(playerController.FindComponent(AFI_PlayerControllerComponent));
		if (!afiComponent)
			return;
		
		string message = string.Format("Tickets: %1", factionRespawns.m_iCount.ToString());
		
		if (factionRespawns.m_bWaveMode)
		{
			int time = factionRespawns.m_iTime;
			time = factionRespawns.m_iTime - Math.Mod(GetGame().GetWorld().GetWorldTime(), time);
			
			message = string.Format("%1\nTime left: %2", message, FormatTimeMS(time));
		}
		
		afiComponent.ShowHint("Respawn info " + factionKey, message);
	}
}