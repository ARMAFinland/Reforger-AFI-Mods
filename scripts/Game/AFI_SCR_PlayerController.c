modded class SCR_PlayerController
{
	override void InitializeRadios(IEntity to)
	{
		if (GetGame().GetPlayerController())
		{
			SCR_VONController vonController = SCR_VONController.Cast(GetGame().GetPlayerController().FindComponent(SCR_VONController));
			vonController.m_CharacterController = SCR_CharacterControllerComponent.Cast(to.FindComponent(SCR_CharacterControllerComponent));
		}
		m_aRadios.Clear();
		//Reforger Lobby bs
		m_aLocalActiveVONEntries.Clear();
		m_aLocalActiveVONEntriesIds.Clear();
		array<RplId> radios = CVON_VONGameModeComponent.GetInstance().GetRadios(to);
		if (!radios || radios.Count() == 0)
		{
			WriteRadioJSON(to);
			return;
		}
		ref array<IEntity> shortRangeRadios = {};
		ref array<IEntity> longRangeRadios = {};
		foreach (RplId radio: radios)
		{
			if (!Replication.FindItem(radio))
				continue;
			
			IEntity radioObject = RplComponent.Cast(Replication.FindItem(radio)).GetEntity();
			if (!radioObject)
				continue;
			
			CVON_RadioComponent radioComp = CVON_RadioComponent.Cast(radioObject.FindComponent(CVON_RadioComponent));
			
			switch (radioComp.m_eRadioType)
			{
				case CVON_ERadioType.SHORT:
				{
					if (!shortRangeRadios.Contains(radioObject))
						shortRangeRadios.Insert(radioObject);
					break;
				}
				case CVON_ERadioType.LONG:
				{
					if (!longRangeRadios.Contains(radioObject))
						longRangeRadios.Insert(radioObject);
					break;
				}	
			}
		}
		if (shortRangeRadios)
			m_aRadios.InsertAll(shortRangeRadios);
		if (longRangeRadios)
			m_aRadios.InsertAll(longRangeRadios);
		WriteRadioJSON(to);
	}
	
	void WriteRadioJSON(IEntity entity)
	{
		if (!GetGame().GetPlayerController())
			return;
		SCR_JsonSaveContext VONSave = new SCR_JsonSaveContext();
		ref array<RplId> radios = CVON_VONGameModeComponent.GetInstance().GetRadios(entity);
		if (!radios)
			return;
		foreach (RplId radio: radios)
		{
			IEntity radioEntity = RplComponent.Cast(Replication.FindItem(radio)).GetEntity();
			CVON_RadioComponent radioComp = CVON_RadioComponent.Cast(radioEntity.FindComponent(CVON_RadioComponent));
			if (!radioComp.m_bPower)
				continue;
			VONSave.StartObject(radio.ToString());
			VONSave.WriteValue("Freq", radioComp.m_sFrequency);
			VONSave.WriteValue("TimeDeviation", radioComp.m_iTimeDeviation);
			VONSave.WriteValue("Volume", radioComp.m_iVolume);
			VONSave.WriteValue("Stereo", radioComp.m_eStereo);
			if (CVON_VONGameModeComponent.GetInstance().m_bUseFactionEcncryption)
				VONSave.WriteValue("FactionKey", radioComp.m_sFactionKey);
			else
				VONSave.WriteValue("FactionKey", "");
			VONSave.EndObject();
		}
		VONSave.SaveToFile("$profile:/RadioData.json");
	}
}