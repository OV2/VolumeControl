// VolumeControl.cpp : Defines the entry point for the console application.
//

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#include "CEndpoint.h"
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlstr.h>

CEndpoint *endpoint;

int processRenderArg(CRenderEndpointVolume *renderEndpointVolume,TCHAR **argPoint,int remainingArgCount);
int processCaptureArg(CCaptureEndpointVolume *captureEndpointVolume,TCHAR **argPoint,int remainingArgCount);
int processEndpointArg(CEndpointVolume *endpointVolume,TCHAR **argPoint,int remainingArgCount);
int processLevelArg(CLevelVolumeControl *levelVolumeControl,TCHAR **argPoint,int remainingArgCount);
void displayHelp(TCHAR *exeName);

void argError(TCHAR *msg,TCHAR *arg)
{
    delete endpoint;
    wprintf(msg);
    wprintf(L"%s\n",arg);
    exit(1);
}

int _tmain(int argc, _TCHAR* argv[])
{
    endpoint = new CEndpoint();
    CRenderEndpointVolume *renderEndpointVolume;
    CCaptureEndpointVolume *captureEndpointVolume;
    UINT devNr;
    WCHAR *devName;
    int usedargs=0;

    if((argc==1) || (lstrcmpi(argv[1],L"/?")==0)) {
        displayHelp(argv[0]);
        exit(0);
    }

    for(int currentArg=1;currentArg<argc;) {
        if(lstrcmpi(argv[currentArg],L"rend")==0){
            if(argc-currentArg<2)
                argError(L"Too few arguments on: ",argv[currentArg]);
            if(lstrcmpi(argv[currentArg+1],L"count")==0){
                wprintf(L"Render endpoint count: %u\n",endpoint->getRenderEndpointCount());
                currentArg += 2;
                continue;
            } else if(lstrcmpi(argv[currentArg+1],L"def")==0){
                renderEndpointVolume = endpoint->GetDefaultRenderEndpointVolume();
            } else {
				wchar_t *end_ptr = NULL;
				devNr = wcstol(argv[currentArg + 1], &end_ptr, 10);
				if (*end_ptr != L'0')
				{
					devNr = -1;
					CString search_str(argv[currentArg + 1]);
					search_str.MakeLower();
					for (int i = 0; i < endpoint->getRenderEndpointCount(); i++)
					{
						CRenderEndpointVolume *tmp = endpoint->GetRenderEndpointVolume(i);
						tmp->getDeviceName(&devName);
						delete tmp;
						CString dev_name(devName);
						CoTaskMemFree(devName);
						dev_name.MakeLower();
						if (dev_name.Find(search_str) >= 0)
						{
							devNr = i;
							break;
						}
					}
				}

                if(devNr<0||devNr>=endpoint->getRenderEndpointCount()) {
                    argError(L"Invalid endpoint device index: ",argv[currentArg+1]);
                }
                renderEndpointVolume = endpoint->GetRenderEndpointVolume(devNr);
            }
            renderEndpointVolume->getDeviceName(&devName);
            wprintf(L"Operating on render endpoing: %s\n",devName);
            CoTaskMemFree(devName);
            if(!(usedargs = processRenderArg(renderEndpointVolume,&argv[currentArg + 2],argc-currentArg-2))) {
                delete renderEndpointVolume;
                argError(L"Invalid argument at this point: ",argv[currentArg + 2]);
            }
            currentArg += usedargs + 2;
            delete renderEndpointVolume;
        } else if(lstrcmpi(argv[currentArg],L"capt")==0){
            if(argc-currentArg<2)
                argError(L"Too few arguments on: ",argv[currentArg]);
            if(lstrcmpi(argv[currentArg+1],L"count")==0){
                wprintf(L"Capture endpoint count: %u\n",endpoint->getCaptureEndpointCount());
                currentArg += 2;
                continue;
            } else if(lstrcmpi(argv[currentArg+1],L"def")==0){
                captureEndpointVolume = endpoint->GetDefaultCaptureEndpointVolume();
            } else {
				wchar_t *end_ptr = NULL;
				devNr = wcstol(argv[currentArg + 1], &end_ptr, 10);
				if (*end_ptr != L'0')
				{
					devNr = -1;
					CString search_str(argv[currentArg + 1]);
					search_str.MakeLower();
					for (int i = 0; i < endpoint->getCaptureEndpointCount(); i++)
					{
						CCaptureEndpointVolume *tmp = endpoint->GetCaptureEndpointVolume(i);
						tmp->getDeviceName(&devName);
						delete tmp;
						CString dev_name(devName);
						CoTaskMemFree(devName);
						dev_name.MakeLower();
						if (dev_name.Find(search_str) >= 0)
						{
							devNr = i;
							break;
						}
					}
				}

                if(devNr<0||devNr>=endpoint->getCaptureEndpointCount()) {
                    argError(L"Invalid endpoint device index: ",argv[currentArg+1]);
                }
                captureEndpointVolume = endpoint->GetCaptureEndpointVolume(devNr);
            }
            captureEndpointVolume->getDeviceName(&devName);
            wprintf(L"Operating on capture endpoing: %s\n",devName);
            CoTaskMemFree(devName);
            if((usedargs = processCaptureArg(captureEndpointVolume,&argv[currentArg + 2],argc-currentArg-2)) == 0) {
                delete captureEndpointVolume;
                argError(L"Invalid argument at this point: ",argv[currentArg + 2]);
            }
            currentArg += usedargs + 2;
            delete captureEndpointVolume;
        } else {
            argError(L"Invalid argument at this point: ",argv[currentArg]);
        }
    }
    delete endpoint;

	return 0;
}

int processRenderArg(CRenderEndpointVolume *renderEndpointVolume,TCHAR **argPoint,int remainingArgCount)
{
    UINT usedargs=0;
    UINT pathId;
    CLevelVolumeControl *levelVolumeControl;
    int curArg;
    for(curArg=0;curArg<remainingArgCount;)
    {
        if(usedargs = processEndpointArg(renderEndpointVolume,&argPoint[curArg],remainingArgCount-curArg)){
            curArg += usedargs;
        } else if(lstrcmpi(argPoint[curArg],L"level")==0){
            if(remainingArgCount-curArg<3)
                argError(L"Too few arguments on: ",argPoint[curArg]);
            pathId = _wtoi(argPoint[curArg+1]);
            if(pathId<0||pathId>=renderEndpointVolume->getPathCount())
                argError(L"Invalid path index: ",argPoint[curArg + 1]);
            levelVolumeControl = renderEndpointVolume->GetLevelVolumeControl(pathId);
            if(levelVolumeControl==NULL)
                argError(L"Invalid level part, does not contain volume level: ",argPoint[curArg+1]);
            wprintf(L"\tOperating on level control in path: %u\n",pathId);
            curArg += processLevelArg(levelVolumeControl,&argPoint[curArg+2],remainingArgCount-curArg-2);
            curArg += 2;
            delete levelVolumeControl;
        } else if(lstrcmpi(argPoint[curArg],L"pathcount")==0){
            wprintf(L"\tOperation: display path count\n");
            wprintf(L"\t\tPaths: %u\n",renderEndpointVolume->getPathCount());
            curArg++;
        } else {
            break;
        }
    }
    return curArg;
}

int processCaptureArg(CCaptureEndpointVolume *captureEndpointVolume,TCHAR **argPoint,int remainingArgCount)
{
    UINT usedargs=0;
    int curArg;
    float level;
    for(curArg=0;curArg<remainingArgCount;)
    {
        if(usedargs = processEndpointArg(captureEndpointVolume,&argPoint[curArg],remainingArgCount-curArg)){
            curArg += usedargs;
        } else if(lstrcmpi(argPoint[curArg],L"enablemicboost")==0){
            wprintf(L"\tOperation: enable microphone boost\n");
            if(captureEndpointVolume->tryMicrophoneBoost(true))
                wprintf(L"\t\tSuccessfully enabled\n");
            else
                wprintf(L"\t\tUnsuccessful (or no boost found)\n");
            curArg++;
        } else if(lstrcmpi(argPoint[curArg],L"disablemicboost")==0){
            wprintf(L"\tOperation: disabl microphone boost\n");
            if(!captureEndpointVolume->tryMicrophoneBoost(false))
                wprintf(L"\t\tSuccessfully disabled (or no boost found)\n");
            else
                wprintf(L"\t\tUnsuccessful\n");
            curArg++;
        } else if(lstrcmpi(argPoint[curArg],L"setmicboost")==0){
            wprintf(L"\tOperation: set microphone boost (scalar)\n");
            if(remainingArgCount-curArg<2)
                argError(L"Too few arguments on: ",argPoint[curArg]);
            level = _wtof(argPoint[curArg+1]);
            if(captureEndpointVolume->tryMicrophoneBoostScalar(level))
                wprintf(L"\t\tSuccessfully set\n");
            else
                wprintf(L"\t\tNo boost found)\n");
            curArg+=2;
        } else if(lstrcmpi(argPoint[curArg],L"setmicboostdb")==0){
            wprintf(L"\tOperation: set microphone boost (dB)\n");
            if(remainingArgCount-curArg<2)
                argError(L"Too few arguments on: ",argPoint[curArg]);
            level = _wtof(argPoint[curArg+1]);
            if(captureEndpointVolume->tryMicrophoneBoostdB(level))
                wprintf(L"\t\tSuccessfully set\n");
            else
                wprintf(L"\t\tNo boost found)\n");
            curArg+=2;
        } else {
            break;
        }
    }
    return curArg;
}

int processEndpointArg(CEndpointVolume *endpointVolume,TCHAR **argPoint,int remainingArgCount)
{
    float level;
    bool ret=true;
    WCHAR *path;
    UINT usedargs=0;
	UINT channel;
    if(lstrcmpi(*argPoint,L"stepup")==0){
        wprintf(L"\tOperation: volume step up\n");
        endpointVolume->volumeStepUp();
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"stepdown")==0){
        wprintf(L"\tOperation: volume step down\n");
        endpointVolume->volumeStepDown();
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"volumedb")==0){
        if(remainingArgCount<2)
            argError(L"Too few arguments on: ",*argPoint);
        level = _wtof(argPoint[1]);
        wprintf(L"\tOperation: set volume to %fdB\n",level);
        endpointVolume->setVolumeLevel(level);
        usedargs=2;
    } else if(lstrcmpi(*argPoint,L"volumescalar")==0){
        if(remainingArgCount<2)
            argError(L"Too few arguments on: ",*argPoint);
        level = _wtof(argPoint[1]);
        wprintf(L"\tOperation: set volume to %f (scalar)\n",level);
        endpointVolume->setVolumeLevelScalar(level);
        usedargs=2;
    } else if(lstrcmpi(*argPoint,L"volumecdb")==0){
        if(remainingArgCount<3)
            argError(L"Too few arguments on: ",*argPoint);
		channel = _wtoi(argPoint[1]);
        level = _wtof(argPoint[2]);
        wprintf(L"\tOperation: set volume of channel %d to %fdB\n",channel,level);
        endpointVolume->setChannelVolumeLevel(channel,level);
        usedargs=3;
    } else if(lstrcmpi(*argPoint,L"volumecscalar")==0){
        if(remainingArgCount<3)
            argError(L"Too few arguments on: ",*argPoint);
		channel = _wtoi(argPoint[1]);
        level = _wtof(argPoint[2]);
        wprintf(L"\tOperation: set volume of channel %d to %f (scalar)\n",channel,level);
        endpointVolume->setChannelVolumeLevelScalar(channel,level);
        usedargs=3;
    } else if(lstrcmpi(*argPoint,L"mute")==0){
        wprintf(L"\tOperation: mute\n");
        endpointVolume->setMute(true);
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"unmute")==0){
        wprintf(L"\tOperation: unmute\n");
        endpointVolume->setMute(false);
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"togglemute")==0){
        wprintf(L"\tOperation: toggle mute\n");
        endpointVolume->toggleMute();
        usedargs=1;
    }  else if(lstrcmpi(*argPoint,L"setdefault")==0){
        wprintf(L"\tOperation: set as default device\n");
		endpointVolume->setDefault();
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"displayinfo")==0){
        wprintf(L"\tOperation: display information\n");
        wprintf(L"\t\tCurrent volume step: %u\n",endpointVolume->getCurrentStep());
        wprintf(L"\t\tNumber of steps: %u\n",endpointVolume->getStepCount());
        wprintf(L"\t\tCurrent volume level: %fdB\n",endpointVolume->getVolumeLevel());
        wprintf(L"\t\tMinimum dB %f:\n",endpointVolume->getMinDb());
        wprintf(L"\t\tMaximum dB %f:\n",endpointVolume->getMaxDb());
        wprintf(L"\t\tStep dB: %f\n",endpointVolume->getStepDb());
        wprintf(L"\t\tCurrent volume level (scalar): %fdB\n",endpointVolume->getVolumeLevelScalar());
        wprintf(L"\t\tMute status:");
        if(endpointVolume->getMute())
            wprintf(L" muted\n");
        else
            wprintf(L" unmuted\n");
        usedargs=1;
    } else if(lstrcmpi(*argPoint,L"traverseDevicePath")==0){
        wprintf(L"\tOperation: traverse device path\n");
        endpointVolume->traverseDevicePath(&path);
        wprintf(L"[---------Path start---------]\n");
        wprintf(L"%s\n",path);
        wprintf(L"[----------Path end----------]\n");
        CoTaskMemFree(path);
        usedargs=1;
    } 
    
    return usedargs;
}

int processLevelArg(CLevelVolumeControl *levelVolumeControl,TCHAR **argPoint,int remainingArgCount)
{
    float level;
    UINT usedargs=0;
    if(lstrcmpi(*argPoint,L"volumedb")==0){
        if(remainingArgCount<2)
            argError(L"Too few arguments on: ",*argPoint);
        level = _wtof(argPoint[1]);
        wprintf(L"\t\tOperation: set volume to %fdB\n",level);
        levelVolumeControl->setVolumeLevelDb(level);
        usedargs = 2;
    } else if(lstrcmpi(*argPoint,L"volumescalar")==0){
        if(remainingArgCount<2)
            argError(L"Too few arguments on: ",*argPoint);
        level = _wtof(argPoint[1]);
        wprintf(L"\t\tOperation: set volume to %f (scalar)\n",level);
        levelVolumeControl->setVolumeLevelScalar(level);
        usedargs = 2;
    } else if(lstrcmpi(*argPoint,L"mute")==0){
        wprintf(L"\t\tOperation: mute\n");
        levelVolumeControl->setMute(true);
        usedargs = 1;
    } else if(lstrcmpi(*argPoint,L"unmute")==0){
        wprintf(L"\t\tOperation: unmute\n");
        levelVolumeControl->setMute(false);
        usedargs = 1;
    } else if(lstrcmpi(*argPoint,L"togglemute")==0){
        wprintf(L"\t\tOperation: toggle mute\n");
        levelVolumeControl->toggleMute();
        usedargs = 1;
	} else if(lstrcmpi(*argPoint,L"volumecscalar")==0){
		if(remainingArgCount<3)
            argError(L"Too few arguments on: ",*argPoint);
		int channel = _wtoi(argPoint[1]);
		level = _wtof(argPoint[2]);
		wprintf(L"\t\tOperation: set volume of channel %d to %f (scalar)\n",channel,level);
		levelVolumeControl->setCurrentChannelScalar(channel,level);
		usedargs = 3;
    } else if(lstrcmpi(*argPoint,L"volumecdb")==0){
		if(remainingArgCount<3)
            argError(L"Too few arguments on: ",*argPoint);
		int channel = _wtoi(argPoint[1]);
		level = _wtof(argPoint[2]);
		wprintf(L"\t\tOperation: set volume of channel %d to %fdB\n",channel,level);
		levelVolumeControl->setCurrentChannelDb(channel,level);
		usedargs = 3;
    } else if(lstrcmpi(*argPoint,L"displayinfo")==0){
        wprintf(L"\t\tOperation: display information\n");
        wprintf(L"\t\t\tMinimum dB: %f\n",levelVolumeControl->getMinDb());
        wprintf(L"\t\t\tMaximum dB: %f\n",levelVolumeControl->getMaxDb());
        wprintf(L"\t\t\tStep dB: %f\n",levelVolumeControl->getStepDb());
        if(levelVolumeControl->mutePresent()) {
            wprintf(L"\t\t\tLevel control has mute control.\n");
            wprintf(L"\t\t\tMute status:");
            if(levelVolumeControl->getMute())
                wprintf(L" muted\n");
            else
                wprintf(L" unmuted\n");
        } else {
            wprintf(L"\t\t\tLevel control has no mute control.\n");
        }
        wprintf(L"\t\t\tNumber of Channels: %u\n",levelVolumeControl->getChannelCount());
        for(UINT i=0;i<levelVolumeControl->getChannelCount();i++)
        {
            wprintf(L"\t\t\tVolum of channel %u: %f\n",i,levelVolumeControl->getCurrentChannelDb(i));
        }
        usedargs = 1;
    }
    return usedargs;
}

void displayHelp(TCHAR *exeName)
{
    wprintf(L"General usage:\n");
    wprintf(L"%s capt|rend count|(def|INDEX|NAME OPERATION [OPERATION ...]) [capt|rend ...]:\n",exeName);
    wprintf(L"   capt|rend: operate on capture or render endpoint\n");
    wprintf(L"   count    : display corresponding endpoint count\n");
    wprintf(L"   def      : operate on default capture/render endpoint\n");
    wprintf(L"   INDEX    : device index, between 0 and endpoint count\n");
	wprintf(L"   NAME     : try to find device by substring matching (case insensitive) with NAME\n");
    wprintf(L"   OPERATION: one of the following:\n");
    wprintf(L"      stepup                 : master volume step up\n");
    wprintf(L"      stepdown               : master volume step down\n");
    wprintf(L"      volumedb FLOATVALUE    : set master volume level to FLOATVALUE dB\n");
    wprintf(L"      volumescalar FLOATVALUE: set master volume level to FLOATVALUE\n");
    wprintf(L"                             : (between 0.0 and 1.0)\n");    
    wprintf(L"      mute                   : mute endpoint\n");
    wprintf(L"      unmute                 : unmute endpoint\n");
    wprintf(L"      togglemute             : toggle mute status of endpoint\n");
    wprintf(L"      displayinfo            : display information on endpoint\n");
    wprintf(L"                             : (MIN/MAX dB values, current volume, etc.)\n");
    wprintf(L"      traversedevicepath     : display device graph of endpoint\n");
    wprintf(L"                             : (only the input multiplexer part)\n");
	wprintf(L"      setdefault             : set device as default endpoint\n");
    wprintf(L"\n   OPERATIONs specific to render endpoints:\n");
    wprintf(L"      pathcount              : display available device paths\n");
    wprintf(L"      level DEVPATH LEVELOP  : operate on individual volume level\n");
    wprintf(L"                             : DEVPATH is the path id, between 0 and path count\n");
    wprintf(L"         LEVELOP is one of the following:\n");
    wprintf(L"         volumedb FLOATVALUE    : set volume level to FLOATVALUE dB\n");
    wprintf(L"         volumescalar FLOATVALUE: set volume level to FLOATVALUE\n");
    wprintf(L"                                : (between 0.0 and 1.0)\n");    
    wprintf(L"         mute                   : mute volume level\n");
    wprintf(L"         unmute                 : unmute volume level\n");
    wprintf(L"         displayinfo            : display information on volume level\n");
    wprintf(L"                                : (MIN/MAX dB values, current volume, etc.)\n");
    wprintf(L"\n   OPERATIONs specific to capture endpoints:\n");
    wprintf(L"     for cards with toggleable microphone boosts:\n");
    wprintf(L"      enablemicboost            : enable microphone boost\n");
    wprintf(L"      disablemicboost           : disable microphone boost\n");
    wprintf(L"     for cards with variable microphone boosts:\n");
    wprintf(L"      setmicboost FLOATVALUE    : set microphone boost to FLOATVALUE dB\n");
    wprintf(L"      setmicboostdb FLOATVALUE  : set microphone boost to FLOATVALUE\n");
    wprintf(L"                                : (between 0.0 and 1.0)\n");
    wprintf(L"\n Multiple operations on endpoints can be combined, operations on volume levels");
    wprintf(L"\n can not (\"level DEVPATH\" is needed in front of each level operation).");
}
