// Based on SCR_ScenarioFrameworkSlotMarker

[EntityEditorProps(category: "AFI/Generic", description: "", dynamicBox: true)]
class AFI_MilitaryMapMarkerEntityClass : GenericEntityClass
{
}

class AFI_MilitaryMapMarkerEntity : GenericEntity
{
	
	[Attribute(defvalue: EMilitarySymbolIdentity.BLUFOR.ToString(), UIWidgets.ComboBox, "Marker Faction Icon. Not all of these combinations will work as they have to be properly defined in MapMarkerConfig.conf", "", ParamEnumArray.FromEnum(EMilitarySymbolIdentity), category: "Map Marker")]
	EMilitarySymbolIdentity m_eMapMarkerFactionIcon;
	
	[Attribute(defvalue: EMilitarySymbolDimension.LAND.ToString(), UIWidgets.ComboBox, "Marker Dimension. Not all of these combinations will work as they have to be properly defined in MapMarkerConfig.conf", "", ParamEnumArray.FromEnum(EMilitarySymbolDimension), category: "Map Marker")]
	EMilitarySymbolDimension m_eMapMarkerDimension;
	
	[Attribute(defvalue: EMilitarySymbolIcon.INFANTRY.ToString(), UIWidgets.ComboBox, "Marker Type 1 modifier. Not all of these combinations will work as they have to be properly defined in MapMarkerConfig.conf", "", ParamEnumArray.FromEnum(EMilitarySymbolIcon), category: "Map Marker")]
	EMilitarySymbolIcon m_eMapMarkerType1Modifier;
	
	[Attribute(defvalue: EMilitarySymbolIcon.INFANTRY.ToString(), UIWidgets.ComboBox, "Marker Type 2 modifier. Not all of these combinations will work as they have to be properly defined in MapMarkerConfig.conf", "", ParamEnumArray.FromEnum(EMilitarySymbolIcon), category: "Map Marker")]
	EMilitarySymbolIcon m_eMapMarkerType2Modifier;
	
	[Attribute("", category: "Map Marker")]
	string m_sDescription;
	
	protected ref SCR_MapMarkerBase m_MapMarker;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!mapMarkerMgr)
			return;
		
		m_MapMarker = mapMarkerMgr.PrepareMilitaryMarker(m_eMapMarkerFactionIcon, m_eMapMarkerDimension, m_eMapMarkerType1Modifier | m_eMapMarkerType2Modifier);
		
		vector worldPos = GetOrigin();
		m_MapMarker.SetWorldPos(worldPos[0], worldPos[2]);
		
		if (m_sDescription != "")
		{
			m_MapMarker.SetCustomText(m_sDescription);
		}		
		
		m_MapMarker.OnCreateMarker();
		
		mapMarkerMgr.InsertStaticMarker(m_MapMarker, false, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void AFI_MilitaryMapMarkerEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}