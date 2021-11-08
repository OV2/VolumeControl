#Volume Control

Volume Control is a command-line application to traverse the device paths and control the various volume-levels (master and individual levels) of capture and render devices.
Additionaly, it can be used to turn the microphone boost on and of (this has only been tested on Audigy 2 cards, but has some internal logic to work on other devices). 

It's been a long time since I developed the tool for my own personal use, so I can't vouch for any code quality.
Although developed when Vista was released, it still works the same on Windows 10.

```
General usage:
VolumeControl.exe capt|rend count|(def|INDEX OPERATION [OPERATION ...]) [capt|rend ...]:
   capt|rend: operate on capture or render endpoint
   count    : display corresponding endpoint count
   def      : operate on default capture/render endpoint
   INDEX    : device index, between 0 and endpoint count
   OPERATION: one of the following:
      stepup                 : master volume step up
      stepdown               : master volume step down
      volumedb FLOATVALUE    : set master volume level to FLOATVALUE dB
      volumescalar FLOATVALUE: set master volume level to FLOATVALUE
                             : (between 0.0 and 1.0)
      mute                   : mute endpoint
      unmute                 : unmute endpoint
      togglemute             : toggle mute status of endpoint
      displayinfo            : display information on endpoint
                             : (MIN/MAX dB values, current volume, etc.)
      traversedevicepath     : display device graph of endpoint
                             : (only the input multiplexer part)
      setdefault             : set device as default endpoint

   OPERATIONs specific to render endpoints:
      pathcount              : display available device paths
      level DEVPATH LEVELOP  : operate on individual volume level
                             : DEVPATH is the path id, between 0 and path count
         LEVELOP is one of the following:
         volumedb FLOATVALUE    : set volume level to FLOATVALUE dB
         volumescalar FLOATVALUE: set volume level to FLOATVALUE
                                : (between 0.0 and 1.0)
         mute                   : mute volume level
         unmute                 : unmute volume level
         displayinfo            : display information on volume level
                                : (MIN/MAX dB values, current volume, etc.)

   OPERATIONs specific to capture endpoints:
     for cards with toggleable microphone boosts:
      enablemicboost            : enable microphone boost
      disablemicboost           : disable microphone boost
     for cards with variable microphone boosts:
      setmicboost FLOATVALUE    : set microphone boost to FLOATVALUE dB
      setmicboostdb FLOATVALUE  : set microphone boost to FLOATVALUE
                                : (between 0.0 and 1.0)

 Multiple operations on endpoints can be combined, operations on volume levels
 can not ("level DEVPATH" is needed in front of each level operation).
```