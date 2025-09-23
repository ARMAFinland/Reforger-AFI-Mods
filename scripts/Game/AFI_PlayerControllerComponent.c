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
}