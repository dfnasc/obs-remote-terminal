# OBS Remote Terminal

OBS RT is a plugin for OBS Stúdio that allows user to control basic functions through any terminal using telnet.

## Supported Commands

 * **scenes**                                                              - list available scenes in current profile
 * **scene** &lt;index&gt;                                                 - select scene by index
 * **transition**                                                          - trigger transition button
 * **audio_devices**                                                       - list audio devices
 * **mute** &lt;device index&gt;                                            - mute audio device
 * **unmute** &lt;device index&gt;                                          - unmute audio device
 * **start_streaming**                                                     - start streaming
 * **stop_streaming**                                                      - stop streaming
 * **streaming**                                                           - streaming status <on|off>
 * **start_recording**                                                     - start recording
 * **stop_recording**                                                      - stop recording
 * **recording**                                                           - recording status <on|off>
 * **window_geometry** [&lt;x&gt; &lt;y&gt; &lt;width&gt; &lt;height&gt;]  - get/set obs main window geometry
