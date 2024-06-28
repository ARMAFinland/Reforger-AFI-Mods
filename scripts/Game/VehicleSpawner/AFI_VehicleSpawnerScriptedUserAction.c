class AFI_VehicleSpawnerScriptedUserAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, desc: "Prefab to spawn")]
	protected ResourceName m_prefabToSpawn;
	
	[Attribute("", desc: "Name of the entity where to spawn vehicle")]
	protected string m_sTargetArea;
	
	[Attribute("0", desc: "Count of how many times this action can be performed")]
	protected int m_iMaxPerformCount;
	
	[RplProp()]
	protected int m_iPerformedCount = 0;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!CanBePerformedScript(pUserEntity))
			return;
		
		m_iPerformedCount++;
		
		if (!Replication.IsServer())
			return;
		
		IEntity targetAreaEntity = GetGame().FindEntity(m_sTargetArea);
		
		SCR_BaseTriggerEntity trigger = SCR_BaseTriggerEntity.Cast(targetAreaEntity);
		if (trigger != null)
		{
			array<IEntity> entitiesInside = {};
			trigger.QueryEntitiesInside();
			int entitiesInsideCount = trigger.GetEntitiesInside(entitiesInside);
			if (entitiesInsideCount > 0)
				return;
		}
		
		Resource resource = Resource.Load(m_prefabToSpawn);
		EntitySpawnParams params = new EntitySpawnParams();
		
		targetAreaEntity.GetTransform(params.Transform);
		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
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
}