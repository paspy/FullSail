#ifndef GameSetting_h__
#define GameSetting_h__

class GameSetting {
public:
    enum eRESOLUTION {
        eRES_1920x1080 = 0, eRES_1280x720, eRES_MAX
    };

    enum eINPUT_DEVICE {
        eINDEV_Keyboard = 0, eINDEV_GamePad, eINDEV_MAX
    };

    static GameSetting & GetRef() { static GameSetting instance; return instance; }

    // accessors
    bool FirstTimePlayer() const { return m_firstTimePlayer; }
    bool Windowed() const { return m_windowed; }
    unsigned int MusicVolume() const { return m_musicVolume; }
    unsigned int SfxVolume() const { return m_sfxVolume; }
    float GammaValue() const { return m_gammaValue; }
    eRESOLUTION Resolution() const { return m_resolution; }
    eINPUT_DEVICE InputDevice() const { return m_inputDevice; }
    bool GamePadIsConnected() const { return m_inputDevice == eINDEV_GamePad; }

    // mutators
    void FirstTimePlayer( bool val ) { m_firstTimePlayer = val; }
    void Windowed( bool val ) { m_windowed = val; }
    void MusicVolume( unsigned int val ) { m_musicVolume = val; }
    void SfxVolume( unsigned int val ) { m_sfxVolume = val; }
    void GammaValue( float val ) { m_gammaValue = val; }
    void Resolution( eRESOLUTION val ) { m_resolution = val; }
    void InputDevice( eINPUT_DEVICE val ) { m_inputDevice = val; }

    // main functions
    bool GenerateDefaultSetting();
    bool LoadExistingSetting();
    bool SaveCurrentSetting();
    bool SaveScreeshotToFile(const std::wstring & path);

private:
    bool            m_firstTimePlayer;
    bool            m_windowed;
    unsigned int    m_musicVolume;
    unsigned int    m_sfxVolume;
    float           m_gammaValue;
    eRESOLUTION     m_resolution;
    eINPUT_DEVICE   m_inputDevice;

    std::string GetCurrentDate() const;
    std::wstring GetCurrentDateW() const;

protected:
    GameSetting();
    ~GameSetting() = default;
    // Delete stuff for singleton
    GameSetting( const GameSetting & ) = delete;
    GameSetting( GameSetting&& ) = delete;
    GameSetting& operator=( GameSetting const& ) = delete;
    GameSetting& operator=( GameSetting && ) = delete;

};
#endif // GameSetting_h__
