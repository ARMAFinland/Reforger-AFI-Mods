[ComponentEditorProps(category: "GameScripted/Character", description: "Add label for observers", color: "0 0 255 255", icon: HYBRID_COMPONENT_ICON)]
class AFI_PolyZoneClass: PS_PolyZoneClass
{
};

[ComponentEditorProps(icon: HYBRID_COMPONENT_ICON)]
class AFI_PolyZone : PS_PolyZone
{
	override void CreateMapWidget(MapConfiguration mapConfig)
	{
		if (m_bLineMode)
			m_MapDrawCommands = { m_LinePolygon };
		else
			m_MapDrawCommands = { m_DrawPolygon, m_LinePolygon };
		
		if (!IsCurrentVisibility())
			return;
		
		if (!m_MapEntity)
			m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		// Get map frame
		Widget mapFrame = m_MapEntity.GetMapMenuRoot().FindAnyWidget(SCR_MapConstants.MAP_FRAME_NAME);
		if (!mapFrame) mapFrame = m_MapEntity.GetMapMenuRoot();
		if (!mapFrame) return; // Somethig gone wrong
		
		m_wCanvasWidget = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets(m_sPolyMarkerLayout, mapFrame));
				
		if (m_mPolygonTexture != "")
			m_TextureSharedItem = m_wCanvasWidget.LoadTexture(m_mPolygonTexture);
		else
			m_TextureSharedItem = null;
		
		if (m_mPolygonTextureBorder != "")
			m_TextureBorderSharedItem = m_wCanvasWidget.LoadTexture(m_mPolygonTextureBorder);
		else
			m_TextureBorderSharedItem = null;
		
		m_DrawPolygon.m_pTexture = m_TextureSharedItem;
		m_DrawPolygon.m_fUVScale = m_fPolygonUVScale;
		m_DrawPolygon.m_iColor = m_cPolygonColor.PackToInt();
		
		m_LinePolygon.m_pTexture = m_TextureBorderSharedItem;
		m_LinePolygon.m_UVScale = Vector(1, m_fPolygonBorderUVScale, 0.01);
		m_LinePolygon.m_iColor = m_cPolygonBorderColor.PackToInt();
		m_LinePolygon.m_fWidth = m_fPolygonBorderWidth;
		m_LinePolygon.m_bShouldEnclose = true; // ONLY MODDED PART
		
		m_wCanvasWidget.SetDrawCommands(m_MapDrawCommands);
		//GetGame().GetCallqueue().CallLater(Update, 0, true);
		SetEventMask(GetOwner(), EntityEvent.POSTFRAME);
	}
}



