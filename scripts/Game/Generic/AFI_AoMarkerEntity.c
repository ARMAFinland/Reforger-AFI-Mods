[EntityEditorProps(category: "AFI/Generic", description: "", dynamicBox: true)]
class AFI_AoMarkerEntityClass : GenericEntityClass
{
}

class AFI_AoMarkerEntity : GenericEntity
{
	protected ref SCR_MapMarkerBase m_MapMarker;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!mapMarkerMgr)
			return;
		
		m_MapMarker = new SCR_MapMarkerBase();
		
		m_MapMarker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		m_MapMarker.SetIconEntry(4);
		m_MapMarker.SetRotation(0);
		m_MapMarker.SetColorEntry(3);
		
		vector worldPos = GetOrigin();
		m_MapMarker.SetWorldPos(worldPos[0], worldPos[2]);
		
		mapMarkerMgr.InsertStaticMarker(m_MapMarker, false, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void AFI_AoMarkerEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}