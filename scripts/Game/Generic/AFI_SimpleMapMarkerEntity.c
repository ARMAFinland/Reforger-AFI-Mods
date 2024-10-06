// Based on SCR_ScenarioFrameworkSlotMarker

[EntityEditorProps(category: "AFI/Generic", description: "", dynamicBox: true)]
class AFI_SimpleMapMarkerEntityClass : GenericEntityClass
{
}

class AFI_SimpleMapMarkerEntity : GenericEntity
{
	
	[Attribute("0", UIWidgets.ComboBox, "Marker Icon", "", ParamEnumArray.FromEnum(AFI_ESimpleMapMarkerCustom), category: "Map Marker")]
	AFI_ESimpleMapMarkerCustom m_eMapMarkerIcon;
	
	[Attribute("0", UIWidgets.ComboBox, "Marker Color", "", ParamEnumArray.FromEnum(AFI_ESimpleMapMarkerCustomColor), category: "Map Marker")]
	AFI_ESimpleMapMarkerCustomColor m_eMapMarkerColor;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, desc: "Rotation of the Map Marker", params: "-180 180 1", category: "Map Marker")]
	int m_iMapMarkerRotation;
	
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
		
		m_MapMarker = new SCR_MapMarkerBase();
		
		m_MapMarker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		m_MapMarker.SetIconEntry(m_eMapMarkerIcon);
		m_MapMarker.SetColorEntry(m_eMapMarkerColor);
		m_MapMarker.SetRotation(m_iMapMarkerRotation);
		
		if (m_sDescription != "")
		{
			m_MapMarker.SetCustomText(m_sDescription);
		}
		
		vector worldPos = GetOrigin();
		m_MapMarker.SetWorldPos(worldPos[0], worldPos[2]);
		m_MapMarker.SetRotation(m_iMapMarkerRotation);
		
		mapMarkerMgr.InsertStaticMarker(m_MapMarker, true, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void AFI_SimpleMapMarkerEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}

enum AFI_ESimpleMapMarkerCustom
{
	CIRCLE = 0,
	CIRCLE2,
	CROSS,
	CROSS2,
	DOT,
	DOT2,
	DROP_POINT,
	DROP_POINT2,
	ENTRY_POINT,
	ENTRY_POINT2,
	FLAG,
	FLAG2,
	FLAG3,
	FORTIFICATION,
	FORTIFICATION2,
	MARK_EXCLAMATION,
	MARK_EXCLAMATION2,
	MARK_EXCLAMATION3,
	MARK_QUESTION,
	MARK_QUESTION2,
	MARK_QUESTION3,
	MINE_FIELD,
	MINE_FIELD2,
	MINE_FIELD3,
	MINE_SINGLE,
	MINE_SINGLE2,
	MINE_SINGLE3,
	OBJECTIVE_MARKER,
	OBJECTIVE_MARKER2,
	OBSERVATION_POST,
	OBSERVATION_POST2,
	PICK_UP,
	PICK_UP2,
	POINT_OF_INTEREST,
	POINT_OF_INTEREST2,
	POINT_OF_INTEREST3,
	POINT_SPECIAL,
	POINT_SPECIAL2,
	RECON_OUTPOST,
	RECON_OUTPOST2,
	WAYPOINT,
	WAYPOINT2,
	DEFEND,
	DEFEND2,
	DESTROY,
	DESTROY2,
	HEAL,
	HELP,
	HELP2,
	ATTACK,
	ATTACK_MAIN,
	CONTAIN,
	CONTAIN2,
	CONTAIN3,
	RETAIN,
	RETAIN2,
	STRONG_POINT,
	STRONG_POINT2,
	TARGET_REFERENCE_POINT,
	TARGET_REFERENCE_POINT2,
	AMBUSH,
	AMBUSH2,
	RECONNAISSANCE,
	SEARCH_AREA,
	DIRECTION_OF_ATTACK,
	DIRECTION_OF_ATTACK_MAIN,
	DIRECTION_OF_ATTACK_PLANNED,
	FOLLOW_AND_SUPPORT,
	FOLLOW_AND_SUPPORT2,
	JOIN,
	JOIN2,
	JOIN3,
	ARROW_LARGE,
	ARROW_LARGE2,
	ARROW_LARGE3,
	ARROW_MEDIUM,
	ARROW_MEDIUM2,
	ARROW_MEDIUM3,
	ARROW_SMALL,
	ARROW_SMALL2,
	ARROW_SMALL3,
	ARROW_CURVE_LARGE,
	ARROW_CURVE_LARGE2,
	ARROW_CURVE_LARGE3,
	ARROW_CURVE_MEDIUM,
	ARROW_CURVE_MEDIUM2,
	ARROW_CURVE_MEDIUM3,
	ARROW_CURVE_SMALL,
	ARROW_CURVE_SMALL2,
	ARROW_CURVE_SMALL3
}

enum AFI_ESimpleMapMarkerCustomColor
{
	WHITE = 0,
	REFORGER_ORANGE,
	ORANGE,
	RED,
	OPFOR,
	INDEPENDENT,
	GREEN,
	BLUE,
	BLUFOR,
	DARK_BLUE,
	MAGENTA,
	CIVILIAN,
	DARK_PINK
}