class AFI_PlayerControllerComponentClass : ScriptComponentClass {}

class AFI_PlayerControllerComponent: ScriptComponent
{
	static const float FADE_DURATION = 1.5;
	protected bool m_bBlackScreenEnabled = false;
	
	//------------------------------------------------------------------------------------------------
	bool IsBlackScreenEnabled()
	{
		return m_bBlackScreenEnabled;
	}
	
	//------------------------------------------------------------------------------------------------
	void ShowHint(string title, string text)
	{
		Rpc(RpcDo_ShowHint, title, text);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ShowHint(string title, string text)
	{
		SCR_HintManagerComponent.GetInstance().ShowCustomHint(text, title);
	}
	
	//------------------------------------------------------------------------------------------------
	void BlackScreen()
	{
		Rpc(RpcDo_BlackScreen);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_BlackScreen()
	{
		SCR_ScreenEffectsManager manager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		if (manager)
		{
			SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(manager.GetEffect(SCR_FadeInOutEffect));
			if (fade)
			{
				fade.FadeOutEffect(true, FADE_DURATION);
				m_bBlackScreenEnabled = true;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Teleport(vector worldPosition, int delay)
	{
		Rpc(RpcDo_Teleport, worldPosition, delay);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Teleport(vector worldPosition, int delay)
	{
		// Wait for fade to black
		GetGame().GetCallqueue().CallLater(TeleportLocal, delay, false, worldPosition);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void TeleportLocal(vector worldPosition)
	{
		SCR_Global.TeleportLocalPlayer(worldPosition);
		
		GetGame().GetCallqueue().CallLater(RpcDo_OpenScreen, 3000, false, worldPosition);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_OpenScreen()
	{
		RpcDo_ShowHint("Teleport", "You have been teleported");
		
		SCR_ScreenEffectsManager manager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		if (manager)
		{
			SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(manager.GetEffect(SCR_FadeInOutEffect));
			if (fade)
			{
				fade.FadeOutEffect(false, FADE_DURATION);
				m_bBlackScreenEnabled = false;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		if (!chatPanelManager)
			return;
		
		ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("respawn");
		invoker.Insert(RequestRespawn_Callback);
		
		invoker = chatPanelManager.GetCommandInvoker("tickets");
		invoker.Insert(RequestTickets_Callback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RequestTickets_Callback(SCR_ChatPanel panel, string data)
	{
		if (data == "")
			return;
		
		Rpc(RpcAsk_Tickets, data);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Tickets(string data)
	{
		PlayerController thisPlayerController = PlayerController.Cast(GetOwner());
		if (!SCR_Global.IsAdmin(thisPlayerController.GetPlayerId()))
			return;
		
		array<string> splittedData = {};
		data.Split(" ", splittedData, true);
		
		if (splittedData.Count() == 0 || splittedData.Count() > 2)
		{
			ShowHint("Tickets Error", "Wrong amount of parameters.\nUsage: /tickets [faction] [new ticket count]");
			return;
		}
		
		PS_GameModeCoop gameMode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if (gameMode == null)
			return;
		
		PS_FactionRespawnCount factionRespawns = gameMode.GetFactionRespawnCountPub(splittedData[0]);
		if (!factionRespawns)
		{
			ShowHint("Tickets Error", string.Format("Unable to find faction respawn count for %1\nUsage: /tickets [faction] [new ticket count]", splittedData[0]));
			return;
		}
		
		if (splittedData.Count() == 2) // Requested set ticket count for a faction
		{
			int newTicketCount = splittedData[1].ToInt();
			if (newTicketCount <= 0)
			{
				ShowHint("Tickets Error", "Unable to parse new ticket count\nUsage: /tickets [faction] [new ticket count]");
				return;
			}
			factionRespawns.m_iCount = newTicketCount;
		}
		
		string message = string.Format("Tickets: %1", factionRespawns.m_iCount.ToString());
	
		if (factionRespawns.m_bWaveMode)
		{
			int time = factionRespawns.m_iTime;
			time = factionRespawns.m_iTime - Math.Mod(GetGame().GetWorld().GetWorldTime(), time);
			
			message = string.Format("%1\nTime left: %2", message, FormatTimeMS(time));
		}
		
		ShowHint("Respawn info " + splittedData[0], message);
	}
	
	//------------------------------------------------------------------------------------------------
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
	
	//------------------------------------------------------------------------------------------------
	protected void RequestRespawn_Callback(SCR_ChatPanel panel, string data)
	{
		if (data == "")
			return;
		
		int targetPlayerId = data.ToInt();
		if (targetPlayerId == 0)
			return;
		
		Rpc(RpcAsk_RespawnPlayer, targetPlayerId);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_RespawnPlayer(int targetPlayerId)
	{
		PlayerController thisPlayerController = PlayerController.Cast(GetOwner());
		if (!SCR_Global.IsAdmin(thisPlayerController.GetPlayerId()))
			return;
		
		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		if (!playableManager)
			return;
		
		RplId rplId = playableManager.GetPlayableByPlayerRemembered(targetPlayerId);
		if (!rplId || rplId == RplId.Invalid())
			return;
		
		PS_PlayableComponent playableComponent = playableManager.GetPlayableById(rplId).GetPlayableComponent();
		if (!playableComponent)
			return;
		
		if (!playableComponent.GetOwnerCharacter().GetCharacterController().IsDead())
			return;
		
		ResourceName prefabToSpawn = playableComponent.GetNextRespawn(false);
		if (!prefabToSpawn)
			return;
		
		PS_RespawnData respawnData = new PS_RespawnData(playableComponent, prefabToSpawn);
		
		PS_GameModeCoop gameMode = PS_GameModeCoop.	Cast(GetGame().GetGameMode());
		if (gameMode == null)
			return;
		
		PS_VoNRoomsManager VoNRoomsManager = PS_VoNRoomsManager.GetInstance();
		if (VoNRoomsManager)
			VoNRoomsManager.MoveToRoom(targetPlayerId, "", "");
		
		GetGame().GetCallqueue().CallLater(gameMode.Respawn, 1000, false, targetPlayerId, respawnData);
	}
}