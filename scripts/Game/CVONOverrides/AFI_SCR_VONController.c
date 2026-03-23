modded class SCR_VONController
{
	protected PS_GameModeCoop m_gameMode2;
	protected PS_GameModeCoop getGameMode2()
	{
		if (m_gameMode2 == null)
		{
			m_gameMode2 = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		}

		return m_gameMode2;
	}
	
	bool shouldBeInGame()
	{
		if (getGameMode2().GetState() != SCR_EGameModeState.GAME)
			return false;
		
		if (!m_CharacterController)
			return false;
		
		return m_CharacterController.GetLifeState() != ECharacterLifeState.DEAD;
	}
	
	override void WriteJSON()
	{
		if (!GetGame().GetPlayerController())
			return;
		SCR_JsonLoadContext VONLoad = new SCR_JsonLoadContext();
		if (!VONLoad.LoadFromFile("$profile:/VONServerData.json"))
		{
			bool realInGame = shouldBeInGame();
			SCR_JsonSaveContext VONServerData = new SCR_JsonSaveContext();
			VONServerData.StartObject("ServerData");
			VONServerData.SetMaxDecimalPlaces(1);
			VONServerData.WriteValue("InGame", realInGame);
			VONServerData.WriteValue("InGameName", m_PlayerManager.GetPlayerName(m_PlayerController.GetPlayerId()));
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
			
			bool realInGame = shouldBeInGame();
			if (gameName == "" || ChannelName != m_VONGameModeComponent.m_sTeamSpeakChannelName || ChannelPassword != m_VONGameModeComponent.m_sTeamSpeakChannelPassword || m_PlayerController.m_sTeamspeakPluginVersion != m_VONGameModeComponent.m_sTeamspeakPluginVersion || InGame != realInGame)
			{
				SCR_JsonSaveContext VONServerData = new SCR_JsonSaveContext();
				VONServerData.StartObject("ServerData");
				VONServerData.SetMaxDecimalPlaces(1);
				VONServerData.WriteValue("InGame", realInGame);
				VONServerData.WriteValue("InGameName", m_PlayerManager.GetPlayerName(m_PlayerController.GetPlayerId()));
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
			if (m_PlayerController.GetTeamspeakClientId() == 0 && !m_PlayerController.m_bHasConnectedToTeamspeakForFirstTime)
			{
				if (!m_MenuManager.GetTopMenu())
					m_MenuManager.OpenMenu(ChimeraMenuPreset.CVON_WarningMenu);
				else if (!m_MenuManager.GetTopMenu().IsInherited(CVON_WarningMenu) && !m_MenuManager.GetTopMenu().IsInherited(PauseMenuUI))
					m_MenuManager.OpenMenu(ChimeraMenuPreset.CVON_WarningMenu);
			}
				
		}
		#endif
		SCR_JsonSaveContext VONSave = new SCR_JsonSaveContext();
		VONSave.WriteValue("IsTransmitting", m_bIsBroadcasting);
		IEntity localEntity = m_Camera;
		if (!localEntity)
			return;
		foreach (int playerId, CVON_VONContainer container: m_PlayerController.m_aLocalEntries)
		{
			IEntity soundSource;
			float left = 0;
			float right = 0;
			float behindIntensity = 0;
			int loweredDecibels = 0;
			string frequency = container.m_sFrequency;
			if (!m_CharacterController)
			{
				//Cuts off all incoming audio, cause we're dead.
				left = 0;
				right = 0;
				frequency = "";
			}
			else if (m_CharacterController.GetLifeState() == ECharacterLifeState.DEAD)
			{
				//Cuts off all incoming audio, cause we're dead.
				left = 0;
				right = 0;
				frequency = "";
			}
			else if (container.m_bIsSpectator)
			{
				ComputeSpectatorLR(playerId, left, right)
			}
			else if (Replication.FindItem(container.m_SenderRplId) && !container.m_SoundSource && container.m_fDistanceToSender != -1)
			{
				soundSource = RplComponent.Cast(Replication.FindItem(container.m_SenderRplId)).GetEntity();
				container.m_SoundSource = soundSource;
				ShouldMuffleAudio(container.m_SoundSource, playerId, loweredDecibels);
				if (loweredDecibels < 0)
					ComputeStereoLR(localEntity, GetHeadHeight(soundSource), container.m_iVolume/1.25, playerId, behindIntensity, left, right);
				else
					ComputeStereoLR(localEntity, GetHeadHeight(soundSource), container.m_iVolume, playerId, behindIntensity, left, right);
			}
			else if (container.m_SoundSource && container.m_fDistanceToSender != -1)
			{
				ShouldMuffleAudio(container.m_SoundSource, playerId, loweredDecibels);
				if (loweredDecibels < 0)
					ComputeStereoLR(localEntity, GetHeadHeight(container.m_SoundSource), container.m_iVolume/1.25, playerId, behindIntensity, left, right);
				else
					ComputeStereoLR(localEntity, GetHeadHeight(container.m_SoundSource), container.m_iVolume, playerId, behindIntensity, left, right);
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
			
			if (!m_FactionManager)
				m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			
			if (!m_BaseGamemode)
				m_BaseGamemode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			
			bool sameLanguage = true;
			if (m_FactionManager)
			{
				if (m_FactionManager.GetPlayerFaction(m_PlayerController.GetPlayerId()) != m_FactionManager.GetPlayerFaction(container.m_iPlayerId) && m_BaseGamemode.IsBabbelEnabled())
					sameLanguage = false;
			}
				
			VONSave.StartObject(m_PlayerController.GetPlayersTeamspeakClientId(playerId).ToString());
			VONSave.SetMaxDecimalPlaces(3);
			VONSave.WriteValue("VONType", container.m_eVonType);
			VONSave.WriteValue("Frequency", frequency);
			VONSave.WriteValue("LeftGain", left);
			VONSave.WriteValue("RightGain", right);
			VONSave.WriteValue("MuffledDecibels", loweredDecibels);
			VONSave.WriteValue("ConnectionQuality", container.m_fConnectionQuality);
			VONSave.WriteValue("FactionKey", container.m_sFactionKey);
			VONSave.WriteValue("PlayerId", playerId);
			VONSave.WriteValue("BehindIntensity", behindIntensity);
			VONSave.WriteValue("SameLanguage", sameLanguage);
			VONSave.EndObject();
		}
		VONSave.SaveToFile("$profile:/VONData.json");
	}
}