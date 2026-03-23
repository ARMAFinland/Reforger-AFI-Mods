class AFI_VehicleSpawnerScriptedUserAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, desc: "Prefab to spawn")]
	protected ResourceName m_prefabToSpawn;
	
	[Attribute("0", desc: "Count of how many times this action can be performed")]
	protected int m_iMaxPerformCount;
	
	[Attribute("25", desc: "Spawn range")]
	protected int m_iRange;
	
	[RplProp()]
	protected int m_iPerformedCount = 0;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		if (!CanBePerformedScript(pUserEntity))
		{
			SendMessage(pUserEntity, "Vehicle spawn failed", "Max mount vehicles spawned");
			return;
		}
		
		vector area[4];
		pOwnerEntity.GetTransform(area);
		
		array<vector> availablePositions = new array<vector>();
		SCR_WorldTools.FindAllEmptyTerrainPositions(availablePositions, area[3], m_iRange, 5, 5, 10);
		
		if (availablePositions.Count() == 0)
		{
			SendMessage(pUserEntity, "Vehicle spawn failed", "No available spawn positions... Retry");
			return;
		}
		
		m_iPerformedCount++;
		
		Resource resource = Resource.Load(m_prefabToSpawn);
		EntitySpawnParams params = new EntitySpawnParams();
		
		// Select random position
		int posIdx = Math.RandomInt(0, availablePositions.Count() - 1);
		
		params.Transform[3] = availablePositions[posIdx] + {0, 0.25, 0};
		params.TransformMode = ETransformMode.WORLD;
		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		
		SendMessage(pUserEntity, "Vehicle spawned", "");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		return false;
		
		if (m_iMaxPerformCount <= 0)
			return false;
		
		UIInfo actionInfo = GetUIInfo();
		if (actionInfo)
		{
			outName = string.Format("%1 (%2/%3 used)", actionInfo.GetName(), m_iPerformedCount, m_iMaxPerformCount);
			return true;
		}
		
		return false;
	};

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{		
		if (m_iMaxPerformCount > 0 && m_iPerformedCount >= m_iMaxPerformCount)
		{
			return false;
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{ 
		return false;
	};
	
	//------------------------------------------------------------------------------------------------
	protected void SendMessage(IEntity pUserEntity, string title, string text)
	{
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
		
		afiComponent.ShowHint(title, text);
	}
}