# OBS Remote Terminal

OBS RT is a plugin for OBS Stúdio that allows user to control basic functions through any terminal using telnet.

## Commands

 **scene** &lt;action&gt; [&lt;args&gt;...]                             execute a scene action

     *list*                                                list available scenes
     *set* &lt;scene index&gt;                                   select scene by index.
     *switch*                                              switch to scene in preview screen

 **streaming** &lt;action&gt; [&lt;args&gt;...]                         handle streaming

     *start*                                               start streaming service
     *stop*                                                stop streaming service
     *status*                                              print streaming service status &lt;active|inactive&gt;
     *info*                                                print streaming service type and settings
     *config* &lt;attribute_name&gt; &lt;attribute_value&gt;           update a streaming service configuration

 **recording** &lt;action&gt; [&lt;args&gt;...]                         handle streaming

     *start*                                               start recording
     *stop*                                                stop recording
     *status*                                              print recording status &lt;active|inactive&gt;

 **audio** &lt;action&gt; [&lt;args&gt;...]                             handle audio devices

     *list*                                                list audio devices
     *mute* &lt;device index&gt;                                 mute audio device by index
     *unmute* &lt;device index&gt;                               unmute audio device by index

 **window** &lt;action&gt; [&lt;args&gt;...]                            handle obs main window

     *geometry* [&lt;x&gt; &lt;y&gt; &lt;width&gt; &lt;height&gt;]                 get/set obs main window geometry
