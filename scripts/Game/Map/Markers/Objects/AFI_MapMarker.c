modded class SCR_MapMarkerBase
{
	override bool OnUpdate(vector visibleMin = vector.Zero, vector visibleMax = vector.Zero)
	{
		if (m_bIsServerSideDisabled || m_bIsDragged)
			return true;
		
		if (m_bTestVisibleFrame)
		{
			if ((m_iPosWorldX < visibleMin[0]) || (m_iPosWorldX > visibleMax[0]) || (m_iPosWorldY < visibleMin[2]) || (m_iPosWorldY > visibleMax[2]))
			{
				SetUpdateDisabled(true);
				return false;
			}
		}
		
		m_MapEntity.WorldToScreen(m_iPosWorldX, m_iPosWorldY, m_iScreenX, m_iScreenY, true);
		
		// --- START MODDED ---
		if (m_wRoot == null)
		{
			return false;
		}
		// --- END MODDED ---
		FrameSlot.SetPos(m_wRoot, GetGame().GetWorkspace().DPIUnscale(m_iScreenX), GetGame().GetWorkspace().DPIUnscale(m_iScreenY));	// needs unscaled coords
		
		return true;
	}
}