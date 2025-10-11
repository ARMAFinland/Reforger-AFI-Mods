class AFI_PlayerControllerComponentClass : ScriptComponentClass {}

class AFI_PlayerControllerComponent: ScriptComponent
{
	static const float FADE_DURATION = 1.5;
	
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
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_BlackScreen()
	{
		SCR_ScreenEffectsManager manager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		if (manager)
		{
			SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(manager.GetEffect(SCR_FadeInOutEffect));
			if (fade)
				fade.FadeOutEffect(true, FADE_DURATION);
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
				fade.FadeOutEffect(false, FADE_DURATION);
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