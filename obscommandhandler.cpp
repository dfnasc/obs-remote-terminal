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
    } else if (cmd == "mute_desk") {
        muteDesktop(cli);
    } else if (cmd == "unmute_desk") {
        unmuteDesktop(cli);
    } else if (cmd == "mute_mic") {
        muteMicrophone(cli);
    } else if (cmd == "unmute_mic") {
        unmuteMicrophone(cli);
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
      response = list.join("\n");
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

void OBSCommandHandler::muteDesktop(Client *cli) {
    emit responseReady(cli, "ERROR", "not implemented");
}

void OBSCommandHandler::unmuteDesktop(Client *cli) {
    emit responseReady(cli, "ERROR", "not implemented");
}

void OBSCommandHandler::muteMicrophone(Client *cli) {
    emit responseReady(cli, "ERROR", "not implemented");
}

void OBSCommandHandler::unmuteMicrophone(Client *cli) {
    emit responseReady(cli, "ERROR", "not implemented");
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

    usage += "scenes               list profile scenes\n";
    usage += "scene <index>        select scene by index\n";
    usage += "transition           switch to selected scene\n";
    usage += "mute_desk            mute desktop audio output\n";
    usage += "unmute_desk          unmute desktop audio output\n";
    usage += "mute_mic             mute microphone output\n";
    usage += "unmute_mic           unmute microphone output\n";
    usage += "start_streaming      start streaming\n";
    usage += "stop_streaming       stop streaming\n";
    usage += "streaming            streaming status <on|off>\n";
    usage += "start_recording      start recording\n";
    usage += "stop_recording       stop recording\n";
    usage += "recording            recording status <on|off>\n";

    emit responseReady(cli, "COMMANDS", usage);
}
