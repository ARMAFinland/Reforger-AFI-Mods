modded class PS_GameModeCoop
{
	override void Respawn(int playerId, PS_RespawnData respawnData)
	{		
		RplComponent rplComponent = RplComponent.Cast(Replication.FindItem(respawnData.m_Id));
		if (!rplComponent)
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(rplComponent.GetEntity());
		if (!character)
			return;

		EntityPrefabData prefab = character.GetPrefabData();
		if (!prefab)
			return;
		
		PS_PlayableComponent oldPlayableComponent = character.PS_GetPlayable();
		EntitySpawnParams params = new EntitySpawnParams();
		oldPlayableComponent.GetSpawnTransform(params.Transform);
		
		SCR_ChimeraCharacter newCharacter = SCR_ChimeraCharacter.Cast(GetGame().SpawnEntityPrefab(Resource.Load(prefab.GetPrefabName()), GetGame().GetWorld(), params));
		PS_PlayableComponent playableContainer = newCharacter.PS_GetPlayable();

		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		SCR_AIGroup aiGroup = playableManager.GetPlayerGroupByPlayable(oldPlayableComponent.GetRplId());
		SCR_AIGroup playabelGroup = aiGroup.m_BotsGroup;
		playabelGroup.AddAIEntityToGroup(newCharacter);
		playableManager.SetPlayablePlayerGroupId(playableContainer.GetRplId(), aiGroup.GetGroupID());

		playableContainer.SetPlayable(true);
		oldPlayableComponent.SetPlayable(false);

		character.GetDamageManager().Kill(Instigator.CreateInstigator(newCharacter));
		character.GetDamageManager().SetHealthScaled(0);
		GetGame().GetCallqueue().CallLater(RPC_ForceRespawnPlayerLate, 300, false, character, oldPlayableComponent, newCharacter, playableContainer);
	}
	
	void RPC_ForceRespawnPlayerLate(SCR_ChimeraCharacter character, PS_PlayableComponent oldPlayableComponent, SCR_ChimeraCharacter newCharacter, PS_PlayableComponent playableContainer)
	{
		character.GetDamageManager().Kill(Instigator.CreateInstigator(newCharacter));
		character.GetDamageManager().SetHealthScaled(0);
		if (!character.GetDamageManager().IsDestroyed())
		{
			GetGame().GetCallqueue().CallLater(RPC_ForceRespawnPlayerLate, 300, false, character, oldPlayableComponent, newCharacter, playableContainer);
			return;
		}

		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		PS_VoNRoomsManager VoNRoomsManager = PS_VoNRoomsManager.GetInstance();
		SCR_AIGroup aiGroup = playableManager.GetPlayerGroupByPlayable(oldPlayableComponent.GetRplId());
		SCR_AIGroup playabelGroup = aiGroup.GetSlave();
		playabelGroup.AddAIEntityToGroup(character);
		playableManager.SetPlayablePlayerGroupId(playableContainer.GetRplId(), aiGroup.GetGroupID());
		int playerId = playableManager.GetPlayerByPlayable(oldPlayableComponent.GetRplId());
		if (playerId > -1)
		{
			VoNRoomsManager.MoveToRoom(playerId, "", "");
			GetGame().GetCallqueue().CallLater(RPC_ForceRespawnPlayerLate2, 500, false, playerId, playableContainer);
		}
	}
	
	void RPC_ForceRespawnPlayerLate2(int playerId, PS_PlayableComponent playable)
	{
		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		playableManager.SetPlayerPlayable(playerId, playable.GetRplId());
		
		
		playableManager.ForceSwitch(playerId);
	}
}
