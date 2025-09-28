modded class SCR_VONController
{
	override void ComputeSpectatorLR(int playerId, out float outLeft, out float outRight, out int silencedDecibels = 0)
	{		
		if (PS_VoNRoomsManager.GetInstance().GetPlayerRoom(playerId) != PS_VoNRoomsManager.GetInstance().GetPlayerRoom(SCR_PlayerController.GetLocalPlayerId()))
		{
			outLeft = 0;
			outRight = 0;
			silencedDecibels = 0;
		}
		else 
		{
			outLeft = 1;
			outRight = 1;
			silencedDecibels = 0;
		}		
	}
	
	protected PS_GameModeCoop m_gameMode;
	protected PS_GameModeCoop getGameMode()
	{
		if (m_gameMode == null)
		{
			m_gameMode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		}
		
		return m_gameMode;
	}
	
	protected bool isSpectating(int playerId)
	{
		if (playerId < 1)
			return false;
		
		// While in game we are required to check if the player has faction.
		// If the player doesn't have faction he is in spectator
		if (getGameMode().GetState() == SCR_EGameModeState.GAME)
		{
			SCR_Faction faction = SCR_Faction.Cast(SCR_FactionManager.SGetPlayerFaction(playerId));
			if (!faction)
				return true;
			
			return false;
		}
		
		// In any other state we can expect spectator behaviour
		return true;
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		super.EOnFixedFrame(owner, timeSlice);
		if (m_fWriteTeamspeakClientIdCooldown > 0)
			m_fWriteTeamspeakClientIdCooldown -= timeSlice;
		else
			m_fWriteTeamspeakClientIdCooldown = 0;
		//Just in case....
		if (!CVON_VONGameModeComponent.GetInstance())
			return;
		if (!m_PlayerController)
		{
			m_PlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		}
		if (!m_CharacterController)
			if (SCR_PlayerController.GetLocalControlledEntity())
				m_CharacterController = SCR_CharacterControllerComponent.Cast(SCR_PlayerController.GetLocalControlledEntity().FindComponent(SCR_CharacterControllerComponent));
		if (!m_VONGameModeComponent)
			m_VONGameModeComponent = CVON_VONGameModeComponent.GetInstance();
		if (!m_PlayerManager)
			m_PlayerManager = GetGame().GetPlayerManager();
		
		CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
		if (!camera)
			return;
		
		ref array<int> playerIds = {};
		m_PlayerManager.GetPlayers(playerIds);
		int maxDistance = m_PlayerController.m_aVolumeValues.Get(4);
		
		bool localInSpec = isSpectating(SCR_PlayerController.GetLocalPlayerId());
		
		//When a player disconnects, they are no longer in the players array, so it just leaves an empty container.
		//This removes that container as when they reconnect they will no longer be heard.
		foreach (int playerId: m_PlayerController.m_aLocalActiveVONEntriesIds)
		{
			if (playerIds.Contains(playerId))
				continue;
			
			int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
			m_PlayerController.m_aLocalActiveVONEntriesIds.RemoveOrdered(index);
			m_PlayerController.m_aLocalActiveVONEntries.RemoveOrdered(index);
			continue;
		}
		
		foreach (int playerId: playerIds)
		{
			if (!SCR_PlayerController.GetLocalControlledEntity())
				continue;
			
			if (playerId == SCR_PlayerController.GetLocalPlayerId())
				continue;
			
			IEntity player = m_PlayerManager.GetPlayerControlledEntity(playerId);
			if (!player)
			{
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
				{
					//If this VON Transmission is radio, don't do shit
					if (m_PlayerController.m_aLocalActiveVONEntries.Get(m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId)).m_eVonType == CVON_EVONType.RADIO)
						continue;
					int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
					m_PlayerController.m_aLocalActiveVONEntriesIds.RemoveOrdered(index);
					m_PlayerController.m_aLocalActiveVONEntries.RemoveOrdered(index);
					continue;
				}
				else
					continue;
			}
			
			if (localInSpec && isSpectating(playerId))
			{
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
				{
					int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
					m_PlayerController.m_aLocalActiveVONEntries[index].m_bIsSpectator = true;
				}
				else
				{
					CVON_VONContainer container = new CVON_VONContainer();
					container.m_eVonType = CVON_EVONType.DIRECT;
					container.m_iVolume = m_VONGameModeComponent.GetPlayerVolume(playerId);
					container.m_SenderRplId = RplComponent.Cast(player.FindComponent(RplComponent)).Id();
					container.m_iClientId = m_PlayerController.GetPlayersTeamspeakClientId(playerId);
					container.m_iPlayerId = playerId;
					container.m_bIsSpectator = true;
					m_PlayerController.m_aLocalActiveVONEntries.Insert(container);
					m_PlayerController.m_aLocalActiveVONEntriesIds.Insert(playerId);
				}
				
				continue;
			} else
			{
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
				{
					int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
					m_PlayerController.m_aLocalActiveVONEntries[index].m_bIsSpectator = false;
				}
			}
			
			SCR_CharacterControllerComponent charCont = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(player).GetCharacterController());
			if (charCont.IsDead() || charCont.IsUnconscious())
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
				{
					int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
					m_PlayerController.m_aLocalActiveVONEntriesIds.RemoveOrdered(index);
					m_PlayerController.m_aLocalActiveVONEntries.RemoveOrdered(index);
					continue;
				}
				else
					continue;
			
			float distance = vector.Distance(player.GetOrigin(), camera.GetOrigin());
			if (distance > maxDistance)
			{
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
				{
					//If this VON Transmission is radio, don't do shit
					if (m_PlayerController.m_aLocalActiveVONEntries.Get(m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId)).m_eVonType == CVON_EVONType.RADIO)
						continue;
					int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
					m_PlayerController.m_aLocalActiveVONEntriesIds.RemoveOrdered(index);
					m_PlayerController.m_aLocalActiveVONEntries.RemoveOrdered(index);
					continue;
				}
				else
					continue;
			}
			else
			{
				if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
					continue;
				else
				{
					CVON_VONContainer container = new CVON_VONContainer();
					container.m_eVonType = CVON_EVONType.DIRECT;
					container.m_iVolume = m_VONGameModeComponent.GetPlayerVolume(playerId);
					container.m_SenderRplId = RplComponent.Cast(player.FindComponent(RplComponent)).Id();
					container.m_iClientId = m_PlayerController.GetPlayersTeamspeakClientId(playerId);
					container.m_iPlayerId = playerId;
					m_PlayerController.m_aLocalActiveVONEntries.Insert(container);
					m_PlayerController.m_aLocalActiveVONEntriesIds.Insert(playerId);
				}
				
			}
		}
		
		//Local processing of data being sent to us
		foreach (CVON_VONContainer container: m_PlayerController.m_aLocalActiveVONEntries)
		{
			if (!container.m_SoundSource)
				continue;
			
			container.m_iVolume = m_VONGameModeComponent.GetPlayerVolume(container.m_iPlayerId);
			
			float distance = vector.Distance(container.m_SoundSource.GetOrigin(), camera.GetOrigin());
			if (distance < maxDistance || localInSpec && container.m_bIsSpectator)
				container.m_fDistanceToSender = distance;
			else
				container.m_fDistanceToSender = -1;
			
			container.m_iVolume = m_VONGameModeComponent.GetPlayerVolume(container.m_iPlayerId);
		}
		
				
		//Handles broadcasting to other players
		if (m_bIsBroadcasting)
		{
			if (m_CharacterController.GetLifeState() != ECharacterLifeState.ALIVE)
			{
				if (m_bToggleBuffer)
				{
					m_bToggleBuffer = false;
					DeactivateCVON();
					m_VONHud.DirectToggleDelay();
				}
				else
					DeactivateCVON();
				return;
			}

			ref array<int> broadcastToPlayerIds = {};
			m_PlayerManager.GetPlayers(playerIds);
			foreach (int playerId: playerIds)
			{	
				#ifdef WORKBENCH
				#else
				if (playerId == SCR_PlayerController.GetLocalPlayerId())
					continue;
				#endif
				
				if (m_aPlayerIdsBroadcastedTo.Contains(playerId))
					continue;
				
				broadcastToPlayerIds.Insert(playerId);
				m_aPlayerIdsBroadcastedTo.Insert(playerId);
			}
			if (broadcastToPlayerIds.Count() > 0)
			{
				m_PlayerController.BroadcastLocalVONToServer(m_CurrentVONContainer, broadcastToPlayerIds, SCR_PlayerController.GetLocalPlayerId(), m_CurrentVONContainer.m_iRadioId);
			}
				
		}
		WriteJSON();
	}
}