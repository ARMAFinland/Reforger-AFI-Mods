modded class CVON_VONGameModeComponent: SCR_BaseGameModeComponent
{    
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        
        SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
        gameMode.GetOnPlayerConnected().Insert(OnPlayerConnected);
    }
}