#include "CEndpoint.h"
#include <strsafe.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <math.h>

#define MAXTRPATHLEN 4096
#define MAXTEMPLEN 80
#define DEVICENAMELEN 160

/* CEndpoint methods */
CEndpoint::CEndpoint()
{
    CoInitialize(NULL);
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    deviceEnumerator->EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE,&renderDevices);
    deviceEnumerator->EnumAudioEndpoints(eCapture,DEVICE_STATE_ACTIVE,&captureDevices);
}

CEndpoint::~CEndpoint()
{
    deviceEnumerator->Release();
    renderDevices->Release();
    captureDevices->Release();
    CoUninitialize();
}

CRenderEndpointVolume *CEndpoint::GetDefaultRenderEndpointVolume()
{
    IAudioEndpointVolume *endpointVolume = NULL;
    IMMDevice *defaultRenderDevice = NULL;
    if(deviceEnumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,&defaultRenderDevice)!=S_OK) {
        return NULL;
    }
    if(defaultRenderDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume)!=S_OK) {
        defaultRenderDevice->Release();
        return NULL;
    }
    return new CRenderEndpointVolume(endpointVolume,defaultRenderDevice);
}

CCaptureEndpointVolume *CEndpoint::GetDefaultCaptureEndpointVolume()
{
    IAudioEndpointVolume *endpointVolume = NULL; 
    IMMDevice *defaultCaptureDevice = NULL;
    if(deviceEnumerator->GetDefaultAudioEndpoint(eCapture,eMultimedia,&defaultCaptureDevice)!=S_OK) {
        return NULL;
    }
    if(defaultCaptureDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume)!=S_OK) {
        defaultCaptureDevice->Release();
        return NULL;
    }
    return new CCaptureEndpointVolume(endpointVolume,defaultCaptureDevice);
}

UINT CEndpoint::getRenderEndpointCount()
{
    UINT count;
    renderDevices->GetCount(&count);
    return count;
}

UINT CEndpoint::getCaptureEndpointCount()
{
    UINT count;
    captureDevices->GetCount(&count);
    return count;
}

CRenderEndpointVolume *CEndpoint::GetRenderEndpointVolume(UINT id)
{
    IAudioEndpointVolume *endpointVolume = NULL;
    IMMDevice *renderDevice = NULL;
    if(renderDevices->Item(id,&renderDevice)!=S_OK) {
        return NULL;
    }
    if(renderDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume)!=S_OK) {
        renderDevice->Release();
        return NULL;
    }
    return new CRenderEndpointVolume(endpointVolume,renderDevice,id);
}

CCaptureEndpointVolume *CEndpoint::GetCaptureEndpointVolume(UINT id)
{
    IAudioEndpointVolume *endpointVolume = NULL;
    IMMDevice *captureDevice = NULL;
    if(captureDevices->Item(id,&captureDevice)!=S_OK) {
        return NULL;
    }
    if(captureDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume)!=S_OK) {
        captureDevice->Release();
        return NULL;
    }
    return new CCaptureEndpointVolume(endpointVolume,captureDevice,id);
}


/* CEndpointVolume methods */
CEndpointVolume::CEndpointVolume(IAudioEndpointVolume *endpointVolume,IMMDevice *endpointDevice,int devId)
{
    this->endpointDevice = endpointDevice;
    this->endpointVolume = endpointVolume;
	this->deviceId = devId;
    endpointVolume->GetVolumeRange(&minDb,&maxDb,&stepDb);
}
CEndpointVolume::~CEndpointVolume()
{
    this->endpointDevice->Release();
    this->endpointVolume->Release();
}

UINT CEndpointVolume::volumeStepUp()
{
    UINT curStep;
    UINT stepCount;
    endpointVolume->VolumeStepUp(NULL);
    endpointVolume->GetVolumeStepInfo(&curStep,&stepCount);
    return curStep;
}

UINT CEndpointVolume::volumeStepDown()
{
    UINT curStep;
    UINT stepCount;
    endpointVolume->VolumeStepDown(NULL);
    endpointVolume->GetVolumeStepInfo(&curStep,&stepCount);
    return curStep;
}

UINT CEndpointVolume::getStepCount()
{
    UINT curStep;
    UINT stepCount;
    endpointVolume->GetVolumeStepInfo(&curStep,&stepCount);
    return stepCount;
}

UINT CEndpointVolume::getCurrentStep()
{
    UINT curStep;
    UINT stepCount;
    endpointVolume->GetVolumeStepInfo(&curStep,&stepCount);
    return curStep;
}

float CEndpointVolume::setVolumeLevel(float newLevelDb)
{
    float afterChangeDb;
    endpointVolume->SetMasterVolumeLevel(newLevelDb,NULL);
    endpointVolume->GetMasterVolumeLevel(&afterChangeDb);
    return afterChangeDb;
}

float CEndpointVolume::setVolumeLevelScalar(float newVolume)
{
    float afterChange;
    endpointVolume->SetMasterVolumeLevelScalar(newVolume,NULL);
    endpointVolume->GetMasterVolumeLevelScalar(&afterChange);
    return afterChange;
}

float CEndpointVolume::setChannelVolumeLevel(UINT channel,float newLevelDb)
{
    float afterChangeDb;
	endpointVolume->SetChannelVolumeLevel(channel,newLevelDb,NULL);
	endpointVolume->GetChannelVolumeLevel(channel,&afterChangeDb);
    return afterChangeDb;
}

float CEndpointVolume::setChannelVolumeLevelScalar(UINT channel,float newVolume)
{
    float afterChange;
    endpointVolume->SetChannelVolumeLevelScalar(channel,newVolume,NULL);
	endpointVolume->GetChannelVolumeLevelScalar(channel,&afterChange);
    return afterChange;
}

float CEndpointVolume::getVolumeLevel()
{
    float volume;
    endpointVolume->GetMasterVolumeLevel(&volume);
    return volume;
}

float CEndpointVolume::getVolumeLevelScalar()
{
    float volumeScalar;
    endpointVolume->GetMasterVolumeLevelScalar(&volumeScalar);
    return volumeScalar;
}

float CEndpointVolume::getChannelVolumeLevel(UINT channel)
{
    float volume;
    endpointVolume->GetChannelVolumeLevel(channel,&volume);
    return volume;
}

float CEndpointVolume::getChannelVolumeLevelScalar(UINT channel)
{
    float volumeScalar;
	endpointVolume->GetChannelVolumeLevelScalar(channel,&volumeScalar);
    return volumeScalar;
}

bool CEndpointVolume::setMute(bool muted)
{
    endpointVolume->SetMute(muted,NULL);
    return getMute();
}

bool CEndpointVolume::getMute()
{
    BOOL mutedB;
    endpointVolume->GetMute(&mutedB);
    if(mutedB)
        return true;
    else
        return false;
}

bool CEndpointVolume::toggleMute()
{
    if(getMute())
        return setMute(false);
    else
        return setMute(true);
}

void CEndpointVolume::getDeviceName(LPWSTR *deviceName)
{
    IPropertyStore *propStore;
    PROPVARIANT propVar;
    *deviceName = (LPWSTR)CoTaskMemAlloc(DEVICENAMELEN * sizeof(WCHAR));
    *deviceName[0] = L'\0';
    PropVariantInit(&propVar);
    endpointDevice->OpenPropertyStore(STGM_READ,&propStore);
    propStore->GetValue(PKEY_Device_FriendlyName,&propVar);
    if(propVar.vt == VT_LPWSTR)
        StringCchPrintf(*deviceName,DEVICENAMELEN,L"%s",propVar.pwszVal);
    PropVariantClear(&propVar);
}

void tabIndent(LPWSTR pathString,UINT count)
{
    for(UINT i=0;i<count;i++) {
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("  "));
    }
}

void CEndpointVolume::traverseDevicePathRecursive(IPart *part,LPWSTR pathString,direction_enum direction,UINT tabs)
{
    IPartsList *pList;
    PartType pType;
    IPart *pNew;
        
    LPWSTR idString;
    UINT pCount;

    tabIndent(pathString,tabs);
    part->GetName(&idString);
    StringCchCat(pathString,MAXTRPATHLEN,idString);
    StringCchCat(pathString,MAXTRPATHLEN,TEXT("\n"));

    part->GetPartType(&pType);
    if(pType == Connector){
        CoTaskMemFree(idString);
        return;
    }

    
    
    tryPartForInterfaces(part,pathString,tabs);

    if(direction == in)
        part->EnumPartsIncoming(&pList);
    else
        part->EnumPartsOutgoing(&pList);
    pList->GetCount(&pCount);
    for(UINT i=0;i<pCount;i++)
    {
        pList->GetPart(i,&pNew);        
        /*if(pCount>1) {
            tabIndent(pathString,tabs+1);
            StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("%s - Path %u:\n"),idString,i);
            StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        }*/
        
        traverseDevicePathRecursive(pNew,pathString,direction,tabs+1);
        pNew->Release();
    }
    pList->Release();
    CoTaskMemFree(idString);

}

void CEndpointVolume::tryPartForInterfaces(IPart *part,LPWSTR pathString,UINT tabs)
{
    IAudioLoudness *audioLoudness;
    IAudioAutoGainControl *autoGainControl;
    IAudioMute *audioMute;
    IAudioVolumeLevel *audioVolumeLevel;
    IAudioInputSelector *audioInputSelector;
    IAudioBass *audioBass;
    IAudioChannelConfig *audioChannelConfig;
    IAudioMidrange *audioMidrange;
    IAudioOutputSelector *audioOutputSelector;
    IAudioPeakMeter *audioPeakMeter;
    IAudioTreble *audioTreble;
    IDeviceSpecificProperty *deviceSpecificProperty;
    IKsJackDescription *ksJackDescription;
    IKsFormatSupport *ksFormatSupport;

    UINT pCount;
    BOOL loudEnabled;
    wchar_t tempStr[MAXTEMPLEN];

    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioLoudness),(LPVOID *)&audioLoudness)==S_OK)
    {
        audioLoudness->GetEnabled(&loudEnabled);
        tabIndent(pathString,tabs);
        if(loudEnabled)
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioLoudness - enabled\n"));
        else
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioLoudness - disabled\n"));
        audioLoudness->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioAutoGainControl),(LPVOID *)&autoGainControl)==S_OK)
    {
        autoGainControl->GetEnabled(&loudEnabled);
        tabIndent(pathString,tabs);
        if(loudEnabled)
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioAutoGainControl - enabled\n"));
        else
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioAutoGainControl - disabled\n"));
        autoGainControl->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioMute),(LPVOID *)&audioMute)==S_OK)
    {
        audioMute->GetMute(&loudEnabled);
        tabIndent(pathString,tabs);
        if(loudEnabled)
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioMute - enabled\n"));
        else
            StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioMute - disabled\n"));
        audioMute->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioVolumeLevel),(LPVOID *)&audioVolumeLevel)==S_OK)
    {
        float minDb,maxDb,stepDb;
        audioVolumeLevel->GetChannelCount(&pCount);
        tabIndent(pathString,tabs);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("IAudioVolumeLevel - Channel Count: %u\n"),pCount);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        tabIndent(pathString,tabs);
        audioVolumeLevel->GetLevelRange(0,&minDb,&maxDb,&stepDb);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT(" Channel 0 minDB: %.3f, maxDB: %.3f, stepDB: %.3f\n"),minDb,maxDb,stepDb);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        audioVolumeLevel->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioInputSelector),(LPVOID *)&audioInputSelector)==S_OK)
    {
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioInputSelector\n"));
        audioInputSelector->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioBass),(LPVOID *)&audioBass)==S_OK)
    {
        audioBass->GetChannelCount(&pCount);
        tabIndent(pathString,tabs);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("IAudioBass - Channel Count: %u\n"),pCount);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        audioBass->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioChannelConfig),(LPVOID *)&audioChannelConfig)==S_OK)
    {
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioChannelConfig\n"));
        audioChannelConfig->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioMidrange),(LPVOID *)&audioMidrange)==S_OK)
    {
        audioMidrange->GetChannelCount(&pCount);
        tabIndent(pathString,tabs);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("IAudioMidrange - Channel Count: %u\n"),pCount);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        audioMidrange->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioOutputSelector),(LPVOID *)&audioOutputSelector)==S_OK)
    {
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IAudioOutputSelector\n"));
        audioOutputSelector->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioPeakMeter),(LPVOID *)&audioPeakMeter)==S_OK)
    {
        audioPeakMeter->GetChannelCount(&pCount);
        tabIndent(pathString,tabs);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("IAudioPeakMeter - Channel Count: %u\n"),pCount);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        audioPeakMeter->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioTreble),(LPVOID *)&audioTreble)==S_OK)
    {
        audioTreble->GetChannelCount(&pCount);
        tabIndent(pathString,tabs);
        StringCchPrintf(tempStr,MAXTEMPLEN,TEXT("IAudioTreble - Channel Count: %u\n"),pCount);
        StringCchCat(pathString,MAXTRPATHLEN,tempStr);
        audioTreble->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IDeviceSpecificProperty),(LPVOID *)&deviceSpecificProperty)==S_OK)
    {
        VARTYPE devPropType;
        deviceSpecificProperty->GetType(&devPropType);
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IDeviceSpecificProperty - Type: "));
        switch(devPropType) {
            case VT_EMPTY:      StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_EMPTY"));
                                break;
            case VT_NULL:       StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_NULL"));
                                break;
            case VT_I2:         StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_I2"));
                                break;
            case VT_I4:         StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_I4"));
                                break;
            case VT_R4:         StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_R4"));
                                break;
            case VT_R8:         StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_R8"));
                                break;
            case VT_CY:         StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_CY"));
                                break;
            case VT_DATE:       StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_DATE"));
                                break;
            case VT_BSTR:       StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_BSTR"));
                                break;
            case VT_DISPATCH:   StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_DISPATCH"));
                                break;
            case VT_ERROR:      StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_ERROR"));
                                break;
            case VT_BOOL:       StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_BOOL"));
                                break;
            case VT_VARIANT:    StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_VARIANT"));
                                break;
            case VT_UNKNOWN:    StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_UNKNOWN"));
                                break;
            case VT_UI1:        StringCchCat(pathString,MAXTRPATHLEN,TEXT("VT_UI1"));
                                break;
        }
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("\n"));
        deviceSpecificProperty->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IKsJackDescription),(LPVOID *)&ksJackDescription)==S_OK)
    {
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IKsJackDescription\n"));
        ksJackDescription->Release();
    }
    if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IKsFormatSupport),(LPVOID *)&ksFormatSupport)==S_OK)
    {
        tabIndent(pathString,tabs);
        StringCchCat(pathString,MAXTRPATHLEN,TEXT("IKsFormatSupport\n"));
        ksFormatSupport->Release();
    } 
}

void CEndpointVolume::traverseDevicePath(LPWSTR *pathStringP,direction_enum direction)
{
    IDeviceTopology *topology;
    IConnector *connectorFrom;
    IConnector *connectorTo;
    IPart *part;
    IPartsList *pList;
    
    LPWSTR pathString;
    LPWSTR idString;

    *pathStringP = (LPWSTR)CoTaskMemAlloc(MAXTRPATHLEN * sizeof(WCHAR));
    pathString = *pathStringP;
    *pathString = L'\0';

    endpointDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&topology);
    topology->GetConnector(0,&connectorFrom);
    connectorFrom->GetConnectedTo(&connectorTo);
    connectorFrom->Release();
    connectorTo->QueryInterface(__uuidof(IPart),(LPVOID *)&part);
    connectorTo->Release();

    part->GetName(&idString);
    StringCchCat(pathString,MAXTRPATHLEN,idString);
    StringCchCat(pathString,MAXTRPATHLEN,TEXT("\n"));
    CoTaskMemFree(idString);

    if(direction == in)
        part->EnumPartsIncoming(&pList);
    else
        part->EnumPartsOutgoing(&pList);
    part->Release();
    pList->GetPart(0,&part);
    pList->Release();
    
    traverseDevicePathRecursive(part,pathString,direction,0);

    part->Release();    
    topology->Release();
}



/* CCaptureEndpointVolume methods */
bool CCaptureEndpointVolume::tryMicrophoneBoost(bool boost)
{
    IDeviceTopology *topology;
    IConnector *connectorFrom;
    IConnector *connectorTo;
    IPart *part;
    PartType pType;
    IPartsList *pList;
    IAudioLoudness *audioLoudness;
    IAudioAutoGainControl *autoGainControl;
    BOOL loudEnabled = FALSE;

    endpointDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&topology);
    topology->GetConnector(0,&connectorFrom);
    connectorFrom->GetConnectedTo(&connectorTo);
    connectorFrom->Release();
    connectorTo->QueryInterface(__uuidof(IPart),(LPVOID *)&part);
    connectorTo->Release();

    part->EnumPartsOutgoing(&pList);
    part->Release();
    pList->GetPart(0,&part);
    pList->Release();

    while(true) {
        part->GetPartType(&pType);
        if(pType == Connector){
            part->Release();
            break;
        }
        if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioLoudness),(LPVOID *)&audioLoudness)==S_OK)
        {
            audioLoudness->SetEnabled(boost,NULL);
            audioLoudness->GetEnabled(&loudEnabled);
            audioLoudness->Release();
            part->Release();
            break;
        }
        if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioAutoGainControl),(LPVOID *)&autoGainControl)==S_OK)
        {
            autoGainControl->SetEnabled(boost,NULL);
            autoGainControl->GetEnabled(&loudEnabled);
            autoGainControl->Release();
            part->Release();
            break;
        }
        part->EnumPartsOutgoing(&pList);
        part->Release();
        pList->GetPart(0,&part);
        pList->Release();
    }

    
    topology->Release();
    if(loudEnabled)
        return true;
    else
        return false;
}

bool CCaptureEndpointVolume::tryMicrophoneBoostVar(float newBoostLevel,bool scalar)
{
    IDeviceTopology *topology;
    IConnector *connectorFrom;
    IConnector *connectorTo;
    IPart *part;
    PartType pType;
    IPartsList *pList;
    IAudioVolumeLevel *audioVolumeLevel;
    float micBoostLevel;
    float minDb,maxDb,stepDb;
    LPWSTR idString;
    bool found=false;

    endpointDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&topology);
    topology->GetConnector(0,&connectorFrom);
    connectorFrom->GetConnectedTo(&connectorTo);
    connectorFrom->Release();
    connectorTo->QueryInterface(__uuidof(IPart),(LPVOID *)&part);
    connectorTo->Release();

    part->EnumPartsOutgoing(&pList);
    part->Release();
    pList->GetPart(0,&part);
    pList->Release();

    while(true) {
        part->GetPartType(&pType);
        if(pType == Connector){
            part->Release();
            break;
        }
        if(part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioVolumeLevel),(LPVOID *)&audioVolumeLevel)==S_OK)
        {
            part->GetName(&idString);
            if(lstrcmpi(idString,TEXT("Microphone Boost"))==0) {
                audioVolumeLevel->GetLevelRange(0,&minDb,&maxDb,&stepDb);
                micBoostLevel = newBoostLevel;
                if(scalar)
                    micBoostLevel = maxDb + 10 * log10(newBoostLevel * newBoostLevel);
                audioVolumeLevel->SetLevelUniform(micBoostLevel,NULL);
                audioVolumeLevel->Release();
                CoTaskMemFree(idString);
                part->Release();
                found=true;
                break;
            }
            CoTaskMemFree(idString);
        }
        part->EnumPartsOutgoing(&pList);
        part->Release();
        pList->GetPart(0,&part);
        pList->Release();
    }

    
    topology->Release();
    return found;
}

bool CCaptureEndpointVolume::tryMicrophoneBoostScalar(float newBoostLevel)
{
    return tryMicrophoneBoostVar(newBoostLevel,true);
}
bool CCaptureEndpointVolume::tryMicrophoneBoostdB(float newBoostLevel)
{
    return tryMicrophoneBoostVar(newBoostLevel,false);
}

void CCaptureEndpointVolume::traverseDevicePath(LPWSTR *pathStringP)
{
    CEndpointVolume::traverseDevicePath(pathStringP,out);
}

void CCaptureEndpointVolume::setDefault()
{
	HWND sHandle=NULL;
	HWND tabHandle;
	HWND pHandle;
	HWND listView;
	HWND setDefault;
	HWND okButton;
	HINSTANCE ret;
	WCHAR fName[MAX_PATH]=L"%systemroot%\\system32\\rundll32.exe";
	WCHAR params[MAX_PATH]=L"%systemroot%\\system32\\shell32.dll,Control_RunDLL \"%systemroot%\\system32\\mmsys.cpl\",Sound";

	if(this->deviceId==-1)
		return;

	if(sHandle = FindWindow(L"#32770",L"Sound")) {
		SendMessage(sHandle,WM_CLOSE,0,0);
		sHandle = NULL;
	}

	DoEnvironmentSubst(fName,MAX_PATH);
	DoEnvironmentSubst(params,MAX_PATH);
	ret = ShellExecute(NULL,L"open",fName,params,NULL,SW_SHOW);	
	
	while(!(sHandle = FindWindow(L"#32770",L"Sound")));
	while(!(tabHandle = FindWindowEx(sHandle,NULL,L"SysTabControl32",L"")));
	SendMessage(tabHandle,4912,1,0);

	while(!(pHandle = FindWindowEx(sHandle,NULL,L"#32770",L"Recording")));
	while(!(listView = FindWindowEx(pHandle,NULL,L"SysListView32",L"")));
	while(!(setDefault = FindWindowEx(pHandle,NULL,L"Button",L"&Set Default")));
	while(!(okButton = FindWindowEx(sHandle,NULL,L"Button",L"OK")));

	for(int i=0;i<(this->deviceId + 1);i++)
		SendMessage(listView,WM_KEYDOWN,VK_DOWN,0);
	
	SendMessage(setDefault,BM_CLICK,0,0);
	SendMessage(okButton,BM_CLICK,0,0);
}




/* CRenderEndpointVolume methods */
void CRenderEndpointVolume::traverseDevicePath(LPWSTR *pathStringP)
{
    CEndpointVolume::traverseDevicePath(pathStringP,in);
}

UINT CRenderEndpointVolume::getPathCount()
{
    IDeviceTopology *topology;
    IConnector *connectorFrom;
    IConnector *connectorTo;
    IPart *part;
    IPartsList *pList;
    PartType pType;
    UINT pCount;

    endpointDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&topology);
    topology->GetConnector(0,&connectorFrom);
    connectorFrom->GetConnectedTo(&connectorTo);
    connectorFrom->Release();
    connectorTo->QueryInterface(__uuidof(IPart),(LPVOID *)&part);
    connectorTo->Release();
    part->EnumPartsIncoming(&pList);
    pList->GetCount(&pCount);
    while(pCount==1) {
        part->Release();
        pList->GetPart(0,&part);
        pList->Release();
        part->GetPartType(&pType);
        if(pType == Connector){
            part->Release();
            topology->Release();
            return 1;
        }
        part->EnumPartsIncoming(&pList);
        pList->GetCount(&pCount);
    }
    part->Release();
    pList->Release();
    return pCount;
}

CLevelVolumeControl *CRenderEndpointVolume::GetLevelVolumeControl(UINT path)
{
    IDeviceTopology *topology;
    IConnector *connectorFrom;
    IConnector *connectorTo;
    IPart *part;
    IPartsList *pList;
    IAudioVolumeLevel *audioVolumeLevel = NULL;
    IAudioMute *audioMute = NULL;

    PartType pType;
    UINT pCount;

    endpointDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&topology);
    topology->GetConnector(0,&connectorFrom);
    connectorFrom->GetConnectedTo(&connectorTo);
    connectorFrom->Release();
    connectorTo->QueryInterface(__uuidof(IPart),(LPVOID *)&part);
    connectorTo->Release();
    part->EnumPartsIncoming(&pList);
    pList->GetCount(&pCount);
    while(pCount==1) {
        part->Release();
        pList->GetPart(0,&part);
        pList->Release();
        part->GetPartType(&pType);
        if(pType == Connector){
            part->Release();
            topology->Release();
            return NULL;
        }
        part->EnumPartsIncoming(&pList);
        pList->GetCount(&pCount);
    }
    part->Release();
    pList->GetPart(path,&part);
    pList->Release();
    
    while(audioMute == NULL || audioVolumeLevel == NULL) {
        part->GetPartType(&pType);
        if(pType == Connector) {
            break;
        }
        if(audioMute == NULL)
        {
            part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioMute),(LPVOID *)&audioMute);
        }
        else if(audioVolumeLevel == NULL)
        {
            part->Activate(CLSCTX_INPROC_SERVER,__uuidof(IAudioVolumeLevel),(LPVOID *)&audioVolumeLevel);
        }
        part->EnumPartsIncoming(&pList);
        part->Release();
        pList->GetPart(0,&part);
        pList->Release();
    }
    part->Release();
    topology->Release();
    if(audioVolumeLevel==NULL)
        return NULL;
    return new CLevelVolumeControl(audioVolumeLevel,audioMute);
    
}

void CRenderEndpointVolume::setDefault()
{
	HWND sHandle=NULL;
	HWND pHandle;
	HWND listView;
	HWND setDefault;
	HWND okButton;
	HINSTANCE ret;
	WCHAR fName[MAX_PATH]=L"%systemroot%\\system32\\rundll32.exe";
	WCHAR params[MAX_PATH]=L"%systemroot%\\system32\\shell32.dll,Control_RunDLL \"%systemroot%\\system32\\mmsys.cpl\",Sound";

	if(this->deviceId==-1)
		return;

	if(sHandle = FindWindow(L"#32770",L"Sound")) {
		SendMessage(sHandle,WM_CLOSE,0,0);
		sHandle = NULL;
	}

	DoEnvironmentSubst(fName,MAX_PATH);
	DoEnvironmentSubst(params,MAX_PATH);
	ret = ShellExecute(NULL,L"open",fName,params,NULL,SW_SHOW);	
	
	while(!(sHandle = FindWindow(L"#32770",L"Sound")));
	while(!(pHandle = FindWindowEx(sHandle,NULL,L"#32770",L"Playback")));
	while(!(listView = FindWindowEx(pHandle,NULL,L"SysListView32",L"")));
	while(!(setDefault = FindWindowEx(pHandle,NULL,L"Button",L"&Set Default")));
	while(!(okButton = FindWindowEx(sHandle,NULL,L"Button",L"OK")));

	for(int i=0;i<(this->deviceId + 1);i++)
		SendMessage(listView,WM_KEYDOWN,VK_DOWN,0);
	
	SendMessage(setDefault,BM_CLICK,0,0);
	SendMessage(okButton,BM_CLICK,0,0);
	return;
}

/* CLevelVolumeControl methods */
CLevelVolumeControl::CLevelVolumeControl(IAudioVolumeLevel *audioVolumeLevel,IAudioMute *audioMute)
{
    this->audioVolumeLevel = audioVolumeLevel;
    this->audioMute = audioMute;
    audioVolumeLevel->GetLevelRange(0,&minDb,&maxDb,&stepDb);
    audioVolumeLevel->GetChannelCount(&nChannel);
}

CLevelVolumeControl::~CLevelVolumeControl()
{
    audioVolumeLevel->Release();
    if(audioMute)
        audioMute->Release();
}

float CLevelVolumeControl::getCurrentChannelDb(UINT channel)
{
    float level;
    audioVolumeLevel->GetLevel(channel,&level);
    return level;
}

void CLevelVolumeControl::setCurrentChannelDb(UINT channel,float level)
{
	audioVolumeLevel->SetLevel(channel,level,NULL);
}

void CLevelVolumeControl::setCurrentChannelScalar(UINT channel,float level)
{
	float newDb = this->maxDb + 10 * log10(level * level);
	audioVolumeLevel->SetLevel(channel,newDb,NULL);
}

void CLevelVolumeControl::setVolumeLevelDb(float level)
{
    audioVolumeLevel->SetLevelUniform(level,NULL);
}

void CLevelVolumeControl::setVolumeLevelScalar(float level)
{
    float newDb = this->maxDb + 10 * log10(level * level);
    audioVolumeLevel->SetLevelUniform(newDb,NULL);
}

bool CLevelVolumeControl::mutePresent()
{
    return (audioMute!=NULL);
}

bool CLevelVolumeControl::setMute(bool muted)
{
    if(!mutePresent())
        return false;

    audioMute->SetMute(muted,NULL);
    return getMute();
}

bool CLevelVolumeControl::getMute()
{
    BOOL mutedB;

    if(!mutePresent())
        return false;

    audioMute->GetMute(&mutedB);
    if(mutedB)
        return true;
    else
        return false;
}

bool CLevelVolumeControl::toggleMute()
{
    if(!mutePresent())
        return false;

    if(getMute())
        return setMute(false);
    else
        return setMute(true);
}