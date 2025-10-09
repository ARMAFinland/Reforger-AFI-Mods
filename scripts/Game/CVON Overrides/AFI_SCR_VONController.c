modded class SCR_VONController
{
	override void ComputeSpectatorLR(int playerId, out float outLeft, out float outRight, out int silencedDecibels = 0)
	{		
		if (IsInOurVONRoom(playerId) && !IsMuted(playerId))
		{
			outLeft = 1;
			outRight = 1;
			silencedDecibels = 0;
		}
		else 
		{
			outLeft = 0;
			outRight = 0;
			silencedDecibels = 0;
		}		
	}
	
	protected bool IsInOurVONRoom(int playerId)
	{
		return PS_VoNRoomsManager.GetInstance().GetPlayerRoom(playerId) == PS_VoNRoomsManager.GetInstance().GetPlayerRoom(SCR_PlayerController.GetLocalPlayerId());
	}
	
	protected bool IsMuted(int playerId)
	{
		SocialComponent socialComp = SocialComponent.Cast(GetGame().GetPlayerController().FindComponent(SocialComponent));
		return socialComp.IsMuted(playerId);
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
			
			if (isSpectating(playerId))
			{
				if (localInSpec && IsInOurVONRoom(playerId))
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
				}
				else 
				{
					// If target player in spec and we are not or they are not in our room, we can safely remove them from our list.
					if (m_PlayerController.m_aLocalActiveVONEntriesIds.Contains(playerId))
					{
						int index = m_PlayerController.m_aLocalActiveVONEntriesIds.Find(playerId);
						m_PlayerController.m_aLocalActiveVONEntriesIds.RemoveOrdered(index);
						m_PlayerController.m_aLocalActiveVONEntries.RemoveOrdered(index);
					}
				}
				
				continue;
			}
			else
			{
				// Just ensures that if there was anyone marked as spectator they will be marked now non spectators
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
			if (distance < maxDistance)
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
				
//				if (m_CurrentVONContainer.m_eVonType == CVON_EVONType.DIRECT)
//				{
//					IEntity player = m_PlayerManager.GetPlayerControlledEntity(playerId);
//					if (!player)
//						continue;
//
//					if (vector.Distance(player.GetOrigin(), SCR_PlayerController.GetLocalControlledEntity().GetOrigin()) > maxDistance)
//					{
//						if (m_aPlayerIdsBroadcastedTo.Contains(playerId))
//						{
//							m_aPlayerIdsBroadcastedTo.RemoveItem(playerId);
//							m_PlayerController.BroadcastRemoveLocalVONToServer(playerId, SCR_PlayerController.GetLocalPlayerId());
//						}
//						continue;
//					}
//				}
				
				if (m_aPlayerIdsBroadcastedTo.Contains(playerId))
					continue;
				
				broadcastToPlayerIds.Insert(playerId);
				m_aPlayerIdsBroadcastedTo.Insert(playerId);
			}
			if (broadcastToPlayerIds.Count() > 0)
			{
//				if (m_CurrentVONContainer.m_eVonType == CVON_EVONType.DIRECT)
//					m_PlayerController.BroadcastLocalVONToServer(m_CurrentVONContainer, broadcastToPlayerIds, SCR_PlayerController.GetLocalPlayerId(), RplId.Invalid());
//				else
				m_PlayerController.BroadcastLocalVONToServer(m_CurrentVONContainer, broadcastToPlayerIds, SCR_PlayerController.GetLocalPlayerId(), m_CurrentVONContainer.m_iRadioId);
			}
				
		}
		WriteJSON();
	}
	
	override void WriteJSON()
	{
		if (!GetGame().GetPlayerController())
			return;
		SCR_JsonLoadContext VONLoad = new SCR_JsonLoadContext();
		if (!VONLoad.LoadFromFile("$profile:/VONServerData.json"))
		{
			bool realInGame = getGameMode().GetState() == SCR_EGameModeState.GAME;
			SCR_JsonSaveContext VONServerData = new SCR_JsonSaveContext();
			VONServerData.StartObject("ServerData");
			VONServerData.SetMaxDecimalPlaces(1);
			VONServerData.WriteValue("InGame", realInGame);
			VONServerData.WriteValue("InGameName", m_PlayerManager.GetPlayerName(SCR_PlayerController.GetLocalPlayerId()));
			VONServerData.WriteValue("TSClientID", m_PlayerController.GetTeamspeakClientId());
			VONServerData.WriteValue("TSPluginVersion", m_PlayerController.m_sTeamspeakPluginVersion);
			VONServerData.WriteValue("VONChannelName", m_VONGameModeComponent.m_sTeamSpeakChannelName);
			VONServerData.WriteValue("VONChannelPassword", m_VONGameModeComponent.m_sTeamSpeakChannelPassword);
			VONServerData.WriteValue("TSServerIp", m_VONGameModeComponent.m_sTeamSpeakServerIP);
			VONServerData.WriteValue("TSServerPassword", m_VONGameModeComponent.m_sTeamSpeakServerPassword);
			VONServerData.EndObject();
			VONServerData.SaveToFile("$profile:/VONServerData.json");
		}
		else
		{
			string ChannelName;
			string ChannelPassword;
			int TSClientId = 0;
			bool InGame;
			string gameName;
			VONLoad.StartObject("ServerData");
			VONLoad.ReadValue("InGame", InGame);
			VONLoad.ReadValue("VONChannelName", ChannelName);
			VONLoad.ReadValue("VONChannelPassword", ChannelPassword);
			VONLoad.ReadValue("TSPluginVersion", m_PlayerController.m_sTeamspeakPluginVersion);
			VONLoad.ReadValue("TSClientID", TSClientId);
			VONLoad.ReadValue("InGameName", gameName);
			if (m_PlayerController.GetTeamspeakClientId() != TSClientId && m_fWriteTeamspeakClientIdCooldown <= 0)
			{
				m_fWriteTeamspeakClientIdCooldown = 1;
				m_PlayerController.SetTeamspeakClientId(TSClientId);
			}
				
			VONLoad.EndObject();
			if (gameName == "" || ChannelName != m_VONGameModeComponent.m_sTeamSpeakChannelName || ChannelPassword != m_VONGameModeComponent.m_sTeamSpeakChannelPassword || m_PlayerController.m_sTeamspeakPluginVersion != m_VONGameModeComponent.m_sTeamspeakPluginVersion || InGame != (getGameMode().GetState() == SCR_EGameModeState.GAME))
			{
				bool realInGame = getGameMode().GetState() == SCR_EGameModeState.GAME;
				SCR_JsonSaveContext VONServerData = new SCR_JsonSaveContext();
				VONServerData.StartObject("ServerData");
				VONServerData.SetMaxDecimalPlaces(1);
				VONServerData.WriteValue("InGame", realInGame);
				VONServerData.WriteValue("InGameName", m_PlayerManager.GetPlayerName(SCR_PlayerController.GetLocalPlayerId()));
				VONServerData.WriteValue("TSClientID", m_PlayerController.GetTeamspeakClientId());
				VONServerData.WriteValue("TSPluginVersion", m_PlayerController.m_sTeamspeakPluginVersion);
				VONServerData.WriteValue("VONChannelName", m_VONGameModeComponent.m_sTeamSpeakChannelName);
				VONServerData.WriteValue("VONChannelPassword", m_VONGameModeComponent.m_sTeamSpeakChannelPassword);
				VONServerData.WriteValue("TSServerIp", m_VONGameModeComponent.m_sTeamSpeakServerIP);
				VONServerData.WriteValue("TSServerPassword", m_VONGameModeComponent.m_sTeamSpeakServerPassword);
				VONServerData.EndObject();
				VONServerData.SaveToFile("$profile:/VONServerData.json");
			}
		}
		#ifdef ENABLE_DIAG
		#else
		//Hijack this whole process to load the initial warning menu
		if (m_VONGameModeComponent.m_bTeamspeakChecks)
		{	
			if (m_PlayerController.GetTeamspeakClientId() == 0 && !m_PlayerController.m_bHasBeenGivenInitialWarning && SCR_PlayerController.GetLocalControlledEntity())
			{
				m_PlayerController.m_bHasBeenGivenInitialWarning = true;
				GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.CVON_WarningMenu);
			}
			else if (!m_PlayerController.m_bHasConnectedToTeamspeakForFirstTime && m_PlayerController.GetTeamspeakClientId() != 0)
				m_PlayerController.m_bHasBeenGivenInitialWarning = true;
		}
		#endif
		SCR_JsonSaveContext VONSave = new SCR_JsonSaveContext();
		VONSave.WriteValue("IsTransmitting", m_bIsBroadcasting);
		IEntity localEntity = GetGame().GetCameraManager().CurrentCamera();
		if (!localEntity)
			return;
		foreach (CVON_VONContainer container: m_PlayerController.m_aLocalActiveVONEntries)
		{
			IEntity soundSource;
			float left = 0;
			float right = 0;
			int loweredDecibels = 0;
			string frequency = container.m_sFrequency;
			if (m_CharacterController.GetLifeState() == ECharacterLifeState.DEAD)
			{
				//Cuts off all incoming audio, cause we're dead.
				left = 0;
				right = 0;
				frequency = "";
			}
			else if (container.m_bIsSpectator)
			{
				ComputeSpectatorLR(container.m_iPlayerId, left, right)
			}
			else if (Replication.FindItem(container.m_SenderRplId) && !container.m_SoundSource && container.m_fDistanceToSender != -1)
			{
				soundSource = RplComponent.Cast(Replication.FindItem(container.m_SenderRplId)).GetEntity();
				container.m_SoundSource = soundSource;
				ShouldMuffleAudio(container.m_SoundSource, container.m_iPlayerId, loweredDecibels);
				if (loweredDecibels < 0)
					ComputeStereoLR(localEntity, GetHeadHeight(soundSource), container.m_iVolume/1.25, container.m_iPlayerId, left, right);
				else
					ComputeStereoLR(localEntity, GetHeadHeight(soundSource), container.m_iVolume, container.m_iPlayerId, left, right);
			}
			else if (container.m_SoundSource && container.m_fDistanceToSender != -1)
			{
				ShouldMuffleAudio(container.m_SoundSource, container.m_iPlayerId, loweredDecibels);
				if (loweredDecibels < 0)
					ComputeStereoLR(localEntity, GetHeadHeight(container.m_SoundSource), container.m_iVolume/1.25, container.m_iPlayerId, left, right);
				else
					ComputeStereoLR(localEntity, GetHeadHeight(container.m_SoundSource), container.m_iVolume, container.m_iPlayerId, left, right);
			}
			
			
			if (container.m_eVonType == CVON_EVONType.RADIO)
			{
				if (container.m_SoundSource)
				{
					container.m_fConnectionQuality = GetSignalStrength(vector.Distance(localEntity.GetOrigin(), container.m_SoundSource.GetOrigin()), container.m_iMaxDistance);
				}
				else
					container.m_fConnectionQuality = GetSignalStrength(vector.Distance(localEntity.GetOrigin(), container.m_vSenderLocation), container.m_iMaxDistance);
			}
				
			VONSave.StartObject(m_PlayerController.GetPlayersTeamspeakClientId(container.m_iPlayerId).ToString());
			VONSave.SetMaxDecimalPlaces(3);
			VONSave.WriteValue("VONType", container.m_eVonType);
			VONSave.WriteValue("Frequency", frequency);
			VONSave.WriteValue("LeftGain", left);
			VONSave.WriteValue("RightGain", right);
			VONSave.WriteValue("MuffledDecibels", loweredDecibels);
			VONSave.WriteValue("ConnectionQuality", container.m_fConnectionQuality);
			VONSave.WriteValue("FactionKey", container.m_sFactionKey);
			VONSave.WriteValue("PlayerId", container.m_iPlayerId);
			VONSave.EndObject();
		}
		VONSave.SaveToFile("$profile:/VONData.json");
	}
}