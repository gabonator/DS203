class CMIVolume : public CMIProgress
{
public:
	virtual int GetValue()
	{
		return Settings.Runtime.m_nVolume / 4;
	}
	virtual void SetValue(int nValue)
	{
		Settings.Runtime.m_nVolume = nValue * 4;
		CCoreSettings::Update();
		BIOS::SYS::Beep(100);
	}
	virtual int GetMin()
	{
		return 0;
	}
	virtual int GetMax()
	{
		return 25;
	}
};
