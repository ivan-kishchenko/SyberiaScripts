modded class LoginTimeBase
{
	override Widget Init()
	{
		Widget result = super.Init();		
		m_btnLeave.Show(false);
		return result;
	}
	
	override void Leave()
	{
		// DO NOTHING
	}
	
	override void Update(float timeslice)
	{
		if (IsRespawn())
		{
			return;
		}
		
		super.Update(timeslice);
	}
};

modded class DayZGame
{
	override void OnRespawnEvent(int time)
	{

	}
};