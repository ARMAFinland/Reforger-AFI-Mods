[ComponentEditorProps(category: "AFI/GameMode/Components", description: "Base for gamemode scripted component.")]
class AFI_VON_GameModeComponentClass : SCR_BaseGameModeComponentClass
{
}

class AFI_VON_GameModeComponent : SCR_BaseGameModeComponent
{	
	override void OnPostInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		PS_VoNRoomsManager.GetInstance().m_eOnRoomChanged.Insert(MovePlayer);
	}
	
	void MovePlayer(int playerId, int roomId, int oldRoomId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		if (playerController.m_aRadios.Count() == 0)
		{
			// Radios were not initialized yet. Just try again later
			GetGame().GetCallqueue().CallLater(MovePlayer, 1000, false, playerId, roomId, oldRoomId);
			return;
		}
		
		CVON_RadioComponent radioComp = CVON_RadioComponent.Cast(playerController.m_aRadios.Get(0).FindComponent(CVON_RadioComponent));
		if (!radioComp)
		{
			return;
		}
			
		
		radioComp.UpdateChannelServer(1);
		radioComp.UpdateFrequncyServer(roomId.ToString());
	}
	
	int CreateFreqFromRoomId()
	{
		return 0;
	}
}