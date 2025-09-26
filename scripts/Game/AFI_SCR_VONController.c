modded class SCR_VONController
{
	protected PS_GameModeCoop m_gameMode;
	
	protected PS_GameModeCoop getGameMode()
	{
		if (m_gameMode == null)
		{
			m_gameMode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		}
		
		return m_gameMode;
	}
	
	override void ComputeStereoLR(
	    IEntity listener,
	    vector  sourcePos,
	    float   volume_m,    
		int 	playerId,        
	    out float outLeft,
	    out float outRight,
	    out int  silencedDecibels = 0,
	    float   rearPanBoost   = 0.55,
	    float   rearShadow     = 0.12,
	    float   elevNarrow     = 0.25,
	    float   bleed          = 0.0,
	    bool    normalizePeak  = true
	)
	{
		if (getGameMode().GetState() == SCR_EGameModeState.GAME)
		{
			super.ComputeStereoLR(
				listener,
				sourcePos,
				volume_m,
				playerId,
				outLeft,
				outRight,
				silencedDecibels,
				rearPanBoost,
				rearShadow,
				elevNarrow,
				bleed,
				normalizePeak);
			
			return;
		}
		
		outLeft = 0;
		outRight = 0;
		silencedDecibels = 0;
	}
}