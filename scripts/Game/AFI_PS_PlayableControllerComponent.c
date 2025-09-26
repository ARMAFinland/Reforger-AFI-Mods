modded class PS_PlayableControllerComponent
{
	protected SCR_VONController m_vonController;
	
	protected SCR_VONController GetVonController()
	{
		if (m_vonController == null)
		{
			m_vonController = SCR_VONController.Cast(GetOwner().FindComponent(SCR_VONController));
		}
		
		return m_vonController;
	}
	
	override void LobbyVoNEnable()
	{
		GetVonController().ActivateCVONSR();
		
		return;
	}
	
	override void LobbyVoNFactionEnable()
	{
		GetVonController().ActivateCVONLR();
		
		return;
	}
	
	override void LobbyVoNAdminEnable()
	{
		GetVonController().ActivateCVONLR2();
		
		return;
	}
	
	override void LobbyVoNDisable()
	{
		GetVonController().DeactivateCVON();
	}
	
	override void LobbyVoNDisableDelayed()
	{
		return;
	}
}