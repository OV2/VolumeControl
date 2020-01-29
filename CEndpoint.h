#ifndef _H_CENDPOINT
#define _H_CENDPOINT

#include <mmdeviceapi.h>
#include <endpointvolume.h>

enum direction_enum {out,in};

class CEndpointVolume;
class CRenderEndpointVolume;
class CCaptureEndpointVolume;
class CLevelVolumeControl;

class CEndpoint
{
protected:
    IMMDeviceEnumerator *deviceEnumerator;
    IMMDeviceCollection *renderDevices;
    IMMDeviceCollection *captureDevices;

public:
    CEndpoint();
    ~CEndpoint();
    CRenderEndpointVolume *GetDefaultRenderEndpointVolume();
    CCaptureEndpointVolume *GetDefaultCaptureEndpointVolume();
    UINT getRenderEndpointCount();
    UINT getCaptureEndpointCount();
    CRenderEndpointVolume *GetRenderEndpointVolume(UINT id);
    CCaptureEndpointVolume *GetCaptureEndpointVolume(UINT id);
};

class CEndpointVolume
{
private:
    void traverseDevicePathRecursive(IPart *part,LPWSTR pathString,direction_enum direction,UINT tabs);
    void tryPartForInterfaces(IPart *part,LPWSTR pathString,UINT tabs);

protected:
    IAudioEndpointVolume *endpointVolume;
    IMMDevice *endpointDevice;
    float minDb;
    float maxDb;
    float stepDb;
	int deviceId;
    void traverseDevicePath(LPWSTR *pathStringP,direction_enum direction);

public:
    CEndpointVolume(IAudioEndpointVolume *endpointVolume,IMMDevice *endpointDevice, int devId=-1);
    ~CEndpointVolume();
    UINT volumeStepUp();
    UINT volumeStepDown();
    UINT getStepCount();
    UINT getCurrentStep();
    float getMinDb() { return minDb; }
    float getMaxDb() { return maxDb; }
    float getStepDb() { return stepDb; }
    float setVolumeLevel(float newLevelDb);
    float setVolumeLevelScalar(float newVolume);
	float setChannelVolumeLevel(UINT channel,float newLevelDb);
	float setChannelVolumeLevelScalar(UINT channel,float newVolume);
    float getVolumeLevel();
    float getVolumeLevelScalar();
	float getChannelVolumeLevel(UINT channel);
    float getChannelVolumeLevelScalar(UINT channel);
    bool setMute(bool muted);
    bool getMute();
    bool toggleMute();
	virtual void setDefault()=0;
    void getDeviceName(LPWSTR *deviceName);
    virtual void traverseDevicePath(LPWSTR *pathStringP)=0;
};

class CCaptureEndpointVolume: public CEndpointVolume
{ 
private:
    bool tryMicrophoneBoostVar(float newBoostLevel,bool scalar);
public:
    CCaptureEndpointVolume(IAudioEndpointVolume *endpointVolume,IMMDevice *endpointDevice,int devId = -1): CEndpointVolume(endpointVolume,endpointDevice,devId) {}
    bool tryMicrophoneBoost(bool boost);
    bool tryMicrophoneBoostScalar(float newBoostLevel);
    bool tryMicrophoneBoostdB(float newBoostLevel);
    void traverseDevicePath(LPWSTR *pathStringP);
	void setDefault();
};

class CRenderEndpointVolume: public CEndpointVolume
{ 
public:
    CRenderEndpointVolume(IAudioEndpointVolume *endpointVolume,IMMDevice *endpointDevice,int devId = -1): CEndpointVolume(endpointVolume,endpointDevice,devId) {}
    void traverseDevicePath(LPWSTR *pathStringP);
    UINT getPathCount();
    CLevelVolumeControl *GetLevelVolumeControl(UINT path);
	void setDefault();
};

class CLevelVolumeControl
{
private:
    IAudioVolumeLevel *audioVolumeLevel;
    IAudioMute *audioMute;
    UINT nChannel;
    float minDb;
    float maxDb;
    float stepDb;

public:
    CLevelVolumeControl(IAudioVolumeLevel *audioVolumeLevel,IAudioMute *audioMute);
    ~CLevelVolumeControl();
    float getMinDb() { return minDb; }
    float getMaxDb() { return maxDb; }
    float getStepDb() { return stepDb; }
    UINT getChannelCount() { return nChannel; }
    float getCurrentChannelDb(UINT channel);
	void setCurrentChannelDb(UINT channel,float level);
	void setCurrentChannelScalar(UINT channel,float level);
    void setVolumeLevelDb(float level);
    void setVolumeLevelScalar(float level);
    bool mutePresent();
    bool setMute(bool muted);
    bool getMute();
    bool toggleMute();
};


#endif