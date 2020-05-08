#include "obscommandhandler.h"

#include <QDebug>
#include <QThread>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs.hpp>

OBSCommandHandler::OBSCommandHandler(QObject *parent)
    : QObject(parent) {

}

void OBSCommandHandler::handleCommand(Client *cli, QString cmd, QStringList args) {

    if (cmd == "scenes") {
        listScenes(cli);
    } else if (cmd == "scene") {

        if (args.length() != 2) {
            emit responseReady(cli, "ERROR", "scene index must be informed.");
        } else {

            bool ok;
            int index = args.at(1).toInt(&ok);

            if (ok) {
               setScene(cli, index);
            } else {
               emit responseReady(cli, "ERROR", "invalid scene index.");
            }
        }

    } else if (cmd == "transition") {
       transition(cli);
    } else if (cmd == "start_streaming") {
        startStreaming(cli);
    } else if (cmd == "stop_streaming") {
        stopStreaming(cli);
    } else if (cmd == "streaming") {
        streamingStatus(cli);
    } else if (cmd == "start_recording") {
        startRecord(cli);
    } else if (cmd == "stop_recording") {
        stopRecord(cli);
    } else if (cmd == "recording") {
        recordingStatus(cli);
    } else if (cmd == "audio_devices") {
       listAudioDevices(cli);
    } else if (cmd == "mute") {

        if (args.length() != 2) {
            emit responseReady(cli, "ERROR", "audio device index must be informed.");
        } else {

            bool ok;
            int index = args.at(1).toInt(&ok);

            if (ok) {
               mute(cli, index);
            } else {
               emit responseReady(cli, "ERROR", "invalid device index.");
            }
        }

    } else if (cmd == "unmute") {

        if (args.length() != 2) {
            emit responseReady(cli, "ERROR", "audio device index must be informed.");
        } else {

            bool ok;
            int index = args.at(1).toInt(&ok);

            if (ok) {
               unmute(cli, index);
            } else {
               emit responseReady(cli, "ERROR", "invalid device index.");
            }
        }

    } else if (cmd == "help") {
        sendUsage(cli);
    } else {
        emit responseReady(cli, "ERROR", "invalid command. type \"help\" for usage");
    }
}

void OBSCommandHandler::listScenes(Client *cli) {

   QString response;
   QStringList list;

   char **scenes = obs_frontend_get_scene_names();
   char *p;
   int count = 0;

   if (scenes != NULL) {
      while ((p = scenes[count]) != NULL) {
         list.append(QString::number(count) + ":" + QString(p));
         count++;
      }
   } 

   bfree(scenes);

   if (list.length() > 0) {
      response = "scenes:\n\n" + list.join("\n");
   } else {
      response = "no scene available";
   }

   emit responseReady(cli, "OK", response);
}

void OBSCommandHandler::setScene(Client *cli, size_t index) {

   struct obs_frontend_source_list list = {};

   obs_frontend_get_scenes(&list);

   if (index >= list.sources.num) {
      emit responseReady(cli, "ERROR", "invalid scene index.");
   } else {
      obs_frontend_set_current_preview_scene(list.sources.array[index]);
      emit responseReady(cli, "OK", "done");
   }

   obs_frontend_source_list_free(&list);
}

void OBSCommandHandler::transition(Client *cli) {
   obs_frontend_preview_program_trigger_transition();

   emit responseReady(cli, "OK", "done");
}

void OBSCommandHandler::listAudioDevices(Client *cli) {

   QStringList devices;

   for (int i = 0; i < 5; i++) {

      obs_source_t *src = obs_get_output_source(i);

      if (!src)
         continue;

      if (obs_source_get_type(src) == OBS_SOURCE_TYPE_INPUT && obs_source_get_output_flags(src) & OBS_SOURCE_AUDIO) {
         devices.append(QString::number(i) + ":" + QString(obs_source_get_name(src)));
      }

      obs_source_release(src);
   }
   
   if (!devices.length()) {
      emit responseReady(cli, "ERROR", "no output source found.");
   } else {
      QString response = "devices:\n\n" + devices.join("\n") + "\n";

      emit responseReady(cli, "OK", response);
   }
}

void OBSCommandHandler::mute(Client *cli, size_t device_index) {

      obs_source_t *src = obs_get_output_source(device_index);

      if (!src) {
         emit responseReady(cli, "ERROR", "invalid device index. Use \"audio_devices\" to get device list.");
         return;
      }

      const char *devname = obs_source_get_name(src);

      obs_source_set_muted(src, true);

      obs_source_release(src);

      emit responseReady(cli, "OK", "muted (" + QString(devname) + ")\n");
}

void OBSCommandHandler::unmute(Client *cli, size_t device_index) {

      obs_source_t *src = obs_get_output_source(device_index);

      if (!src) {
         emit responseReady(cli, "ERROR", "invalid device index. Use \"audio_devices\" to get device list.");
         return;
      }

      const char *devname = obs_source_get_name(src);

      obs_source_set_muted(src, false);

      obs_source_release(src);

      emit responseReady(cli, "OK", "unmuted (" + QString(devname) + ")\n");
}

void OBSCommandHandler::startStreaming(Client *cli) {

   bool success = false;
   int tries = 5;

   if (obs_frontend_streaming_active()) {
      emit responseReady(cli, "ERROR", "already streaming.");
      return;
   }

   obs_frontend_streaming_start();

   while (tries-- > 0) {
      if (obs_frontend_streaming_active()) {
         success = true;
         break;
      }

      QThread::msleep(1000);
   }

   if (success) {
      emit responseReady(cli, "OK", "streaming");
   } else {
      emit responseReady(cli, "ERROR", "not started.");
   }
}

void OBSCommandHandler::stopStreaming(Client *cli) {

   int tries = 5;
   bool success = false;

   if (!obs_frontend_streaming_active()) {
      emit responseReady(cli, "ERROR", "already stopped.");
      return;
   }

   obs_frontend_streaming_stop();

   while (tries-- > 0) {
      if (obs_frontend_streaming_active()) {
         success = true;
         break;
      }

      QThread::msleep(1000);
   }

   if (success) {
      emit responseReady(cli, "OK", "not streaming");
   } else {
      emit responseReady(cli, "ERROR", "still streaming :/");
   }

}

void OBSCommandHandler::streamingStatus(Client *cli) {

   if (obs_frontend_streaming_active()) {
      emit responseReady(cli, "OK", "active");
   } else {
      emit responseReady(cli, "OK", "inactive");
   }

}

void OBSCommandHandler::startRecord(Client *cli) {

   bool success = false;
   int tries = 5;

   if (obs_frontend_recording_active()) {
      emit responseReady(cli, "ERROR", "already recording.");
      return;
   }

   obs_frontend_recording_start();

   while (tries-- > 0) {
      if (obs_frontend_recording_active()) {
         success = true;
         break;
      }

      QThread::msleep(1000);
   }

   if (success) {
      emit responseReady(cli, "OK", "recording...");
   } else {
      emit responseReady(cli, "ERROR", "not started.");
   }
}

void OBSCommandHandler::recordingStatus(Client *cli) {

   if (obs_frontend_recording_active()) {
      emit responseReady(cli, "OK", "active");
   } else {
      emit responseReady(cli, "OK", "inactive");
   }

}

void OBSCommandHandler::stopRecord(Client *cli) {

   int tries = 5;
   bool success = false;

   if (!obs_frontend_recording_active()) {
      emit responseReady(cli, "ERROR", "already stopped.");
      return;
   }

   obs_frontend_recording_stop();

   while (tries-- > 0) {
      if (obs_frontend_recording_active()) {
         success = true;
         break;
      }

      QThread::msleep(1000);
   }

   if (success) {
      emit responseReady(cli, "OK", "recording was stopped.");
   } else {
      emit responseReady(cli, "ERROR", "still recording :/");
   }
}

void OBSCommandHandler::sendUsage(Client *cli) {

    QString usage = "\n\n";

    usage += "scenes                 list profile scenes\n";
    usage += "scene <index>          select scene by index\n";
    usage += "transition             switch to selected scene\n";
    usage += "audio_devices          list available audio devices\n";
    usage += "mute <device index>    mute audio device\n";
    usage += "unmute <device_index>  unmute audio audio\n";
    usage += "start_streaming        start streaming\n";
    usage += "stop_streaming         stop streaming\n";
    usage += "streaming              streaming status <on|off>\n";
    usage += "start_recording        start recording\n";
    usage += "stop_recording         stop recording\n";
    usage += "recording              recording status <on|off>\n";

    emit responseReady(cli, "COMMANDS", usage);
}
