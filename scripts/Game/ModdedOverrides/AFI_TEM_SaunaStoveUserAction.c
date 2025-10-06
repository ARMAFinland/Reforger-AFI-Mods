modded class TEM_SaunaStoveUserAction
{
	[Attribute("", UIWidgets.Auto)]
	ref SCR_AudioSourceConfiguration m_PalkkiAudioSourceConfiguration;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		AFI_Counter counter = AFI_Counter.Cast(pOwnerEntity.FindComponent(AFI_Counter));
		if (!counter)
			return;
		
	    if (counter.GetCount() == 4)
	    {
	        PlayPalkki(pOwnerEntity);
	    }
	
	    if (!Replication.IsServer())
	        return;
	
	    counter.IncreaseByOne();
	}
	
	//------------------------------------------------------------------------------------------------
	private void PlayPalkki(IEntity pOwnerEntity)
	{
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (!soundManagerEntity)
			return;
		
		if (!m_PalkkiAudioSourceConfiguration || !m_PalkkiAudioSourceConfiguration.IsValid())
			return;
		
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(pOwnerEntity, m_PalkkiAudioSourceConfiguration);
		if (!audioSource)
			return;
		
		soundManagerEntity.PlayAudioSource(audioSource);
	}
}