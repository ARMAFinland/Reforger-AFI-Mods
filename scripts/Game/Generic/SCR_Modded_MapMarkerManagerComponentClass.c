modded class SCR_MapMarkerManagerComponent {
	//------------------------------------------------------------------------------------------------
	//! Insert customized static marker
	//! \param[in] marker is the subject
	//! \param[in] isLocal determines whether the marker is synchronised to other players or local
	//! \param[in] isServerMarker determines whether the marker is spawned by server and should not count towards marker limits (different from server-client spawning his own markers)
	void InsertStaticMarker(SCR_MapMarkerBase marker, bool isLocal, bool isServerMarker = false)
	{
		if (isLocal)	// local
		{
			// --- START MODDED ---
			int ownerId = -1;
			PlayerController playerController = GetGame().GetPlayerController();
			if (playerController)
				ownerId = playerController.GetPlayerId();
			
			marker.SetMarkerOwnerID(ownerId);
			// --- END MODDED ---
			
			m_aStaticMarkers.Insert(marker);
			marker.OnCreateMarker();
		}
		else 
		{
			if (isServerMarker)
			{
				AssignMarkerUID(marker);
				marker.SetMarkerOwnerID(-1);
				
				OnAddSynchedMarker(marker);	// add server side
				OnAskAddStaticMarker(marker);
			}
			else 
			{
				if (!m_MarkerSyncComp)
				{
					if (!FindMarkerSyncComponent())
						return;
				}
				
				m_MarkerSyncComp.AskAddStaticMarker(marker);
			}
		}
	}		
}
