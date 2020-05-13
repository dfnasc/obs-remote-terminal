#include "obscommandhandler.h"

#include <QDebug>
#include <QThread>
#include <QMainWindow>
#include <QRect>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs.hpp>

#define ORT_OK    "OK"
#define ORT_ERROR "ERROR"

#define ORT_ERROR_INVALID_ARG "invalid arguments. Type \"help\" for usage"
#define ORT_ERROR_INVALID_CFG_NAME "invalid streaming service config. Type \"streaming info\" for more information about streaming config"
#define ORT_ERROR_INVALID_DEVICE_INDEX "invalid device index."
#define ORT_ERROR_UNKNOWN "unknown error"

OBSCommandHandler::OBSCommandHandler(QObject *parent)
    : QObject(parent) {

}

void OBSCommandHandler::handleCommand(Client *cli, QString cmd, QStringList args) {

   if (args.length() < 2) {
      if (args.length() > 0) sendUsage(cli);
      return;
   }

   args.removeFirst();

   if (cmd == "scene") {
      handleSceneCommand(cli, args);
   } else if (cmd == "streaming") {
      handleStreamingCommand(cli, args);
   } else if (cmd == "recording") {
      handleRecordingCommand(cli, args);
   } else if (cmd == "audio") {
      handleAudioCommand(cli, args);
   } else if (cmd == "window") {
      handleWindowCommand(cli, args);
   } else {
      sendUsage(cli);
   }
}

void OBSCommandHandler::handleSceneCommand(Client *cli, QStringList args) {

   QString cmd = args.at(0);

   if (cmd == "list") {
      listScenes(cli);
   } else if (cmd == "set") {

      if (args.length() == 2) {

         bool ok;
         size_t idx;

         idx = args.at(1).toInt(&ok);

         if (ok) {
            setScene(cli, idx);
         } else {
            emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
         }
      } else {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
      }

   } else if (cmd == "switch") {
      transition(cli);
   }
}

void OBSCommandHandler::handleStreamingCommand(Client *cli, QStringList args) {

   QString cmd = args.at(0);

   if (cmd == "start") {
      startStreaming(cli);
   } else if (cmd == "stop") {
      stopStreaming(cli);
   } else if (cmd == "status") {
      streamingStatus(cli);
   } else if (cmd == "info") {
      streamingInfo (cli);
   } else if (cmd == "config") {

      if (args.length() == 3) {
         configureStreaming(cli, args.at(1), args.at(2));
      } else {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
      }
   }
}

void OBSCommandHandler::handleRecordingCommand(Client *cli, QStringList args) {

   QString cmd = args.at(0);

   if (cmd == "start") {
      startRecord(cli);
   } else if (cmd == "stop") {
      stopRecord(cli);
   } else if (cmd == "status") {
      recordingStatus(cli);
   } else {
      sendUsage(cli);
   }
}

void OBSCommandHandler::handleAudioCommand(Client *cli, QStringList args) {

   QString cmd = args.at(0);

   if (cmd == "list") {
      listAudioDevices(cli);
   } else if (cmd == "mute") {

      bool ok;
      size_t deviceIndex;

      if (args.length() == 2) {

         deviceIndex = args.at(1).toInt(&ok);

         if (ok) {
            mute(cli, deviceIndex);
         } else {
            emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
         }

      } else {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
      }
      
   } else if (cmd == "unmute") {

      bool ok;
      size_t deviceIndex;

      if (args.length() == 2) {

         deviceIndex = args.at(1).toInt(&ok);

         if (ok) {
            unmute(cli, deviceIndex);
         } else {
            emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
         }

      } else {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
      }

   } else {
      sendUsage(cli);
   }

}

void OBSCommandHandler::handleWindowCommand(Client *cli, QStringList args) {

   QString cmd = args.at(0);

   if (cmd == "geometry") {

      bool ok;
      int geo[4];

      if (args.length() == 5) {

         for (int i = 0; i < 4; i++) {
            geo[i] = args.at(i+1).toInt(&ok);
            if (!ok) break;
         }

         if (ok) {
            setWindowGeometry(cli, geo[0], geo[1], geo[2], geo[3]);
         } else {
            emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
         }

      } else if (args.length() == 1) {
         windowGeometry(cli);
      } else {
         sendUsage(cli);
      }

   } else {
      sendUsage(cli);
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

   emit responseReady(cli, ORT_OK, response);
}

void OBSCommandHandler::setScene(Client *cli, size_t index) {

   struct obs_frontend_source_list list = {};

   obs_frontend_get_scenes(&list);

   if (index >= list.sources.num) {
      emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_ARG);
   } else {
      obs_source_t *cur = list.sources.array[index];
      obs_frontend_set_current_preview_scene(cur);
      emit responseReady(cli, ORT_OK, QString(obs_source_get_name(cur)));
   }

   obs_frontend_source_list_free(&list);
}

void OBSCommandHandler::transition(Client *cli) {

   QString response = "";
   QTextStream ts(&response);
   obs_source_t *s;

   s = obs_frontend_get_current_scene();

   if (s) {
      ts << "[" << QString(obs_source_get_name(s)) << "]";
      obs_source_release(s);
   }

   obs_frontend_preview_program_trigger_transition();

   QThread::msleep(100);

   ts << " <-> ";

   s = obs_frontend_get_current_scene();

   if (s) {
      ts << "[" << QString(obs_source_get_name(s)) << "]";
      obs_source_release(s);
   }

   emit responseReady(cli, ORT_OK, response);
}

void OBSCommandHandler::listAudioDevices(Client *cli) {

   QStringList devices;

   for (int i = 0; i < 5; i++) {

      obs_source_t *src = obs_get_output_source(i);

      if (!src)
         continue;

      if (obs_source_get_type(src) == OBS_SOURCE_TYPE_INPUT && obs_source_get_output_flags(src) & OBS_SOURCE_AUDIO) {
         QString desc;
         QTextStream ts(&desc);

         ts << QString::number(i) << ":" << QString(obs_source_get_name(src));

         if (obs_source_muted(src)) {
            ts << " (muted)";
         }

         devices.append(desc);
      }

      obs_source_release(src);
   }
   
   if (!devices.length()) {
      emit responseReady(cli, ORT_ERROR, ORT_ERROR_UNKNOWN);
   } else {
      QString response = "devices:\n\n" + devices.join("\n") + "\n";

      emit responseReady(cli, ORT_OK, response);
   }
}

void OBSCommandHandler::mute(Client *cli, size_t device_index) {

      obs_source_t *src = obs_get_output_source(device_index);

      if (!src) {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_DEVICE_INDEX);
         return;
      }

      const char *devname = obs_source_get_name(src);

      obs_source_set_muted(src, true);

      obs_source_release(src);

      emit responseReady(cli, ORT_OK, "muted (" + QString(devname) + ")\n");
}

void OBSCommandHandler::unmute(Client *cli, size_t device_index) {

      obs_source_t *src = obs_get_output_source(device_index);

      if (!src) {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_DEVICE_INDEX);
         return;
      }

      const char *devname = obs_source_get_name(src);

      obs_source_set_muted(src, false);

      obs_source_release(src);

      emit responseReady(cli, ORT_OK, "unmuted (" + QString(devname) + ")\n");
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

void OBSCommandHandler::streamingInfo(Client *cli) {

   obs_service_t *svc = obs_frontend_get_streaming_service();

   if (svc == nullptr) {
      emit responseReady(cli, "OK", "invalid streaming service");
      return;
   }

   obs_data_t *settings = obs_service_get_settings(svc);
   
   if (settings) {

      const char *json = obs_data_get_json(settings);

      if (json) {
         emit responseReady(cli, "OK", QString(json));
      } else {
         emit responseReady(cli, "ERROR", "could not get service info.");
      }
   }
}

void OBSCommandHandler::configureStreaming(Client *cli, const QString& attr, const QString& value) {

   qDebug() << "CONFIG " << attr << " = " << value;

   obs_service_t *svc = obs_frontend_get_streaming_service();

   if (svc == nullptr) {
      emit responseReady(cli, ORT_ERROR, ORT_ERROR_UNKNOWN);
      return;
   }

   obs_data_t *settings = obs_service_get_settings(svc);
   
   if (settings) {

      if (obs_data_get_string(settings, attr.toUtf8().data())) {

         QThread::msleep(200);
         obs_data_set_string(settings, attr.toUtf8().data(), value.toUtf8().data());
         obs_service_update(svc, settings);
         obs_frontend_save_streaming_service();

         emit responseReady(cli, ORT_OK, value);

      } else {
         emit responseReady(cli, ORT_ERROR, ORT_ERROR_INVALID_CFG_NAME);
      }

   } else {
      emit responseReady(cli, ORT_ERROR, ORT_ERROR_UNKNOWN);
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

void OBSCommandHandler::setWindowGeometry(Client *cli, int x, int y, int w, int h) {

   QMainWindow *wnd = (QMainWindow*)obs_frontend_get_main_window();

   if (wnd != nullptr) {
      wnd->showNormal();
      wnd->setGeometry(x, y, w, h);

      const QRect g = wnd->geometry();

      emit responseReady(cli, "OK", QString("%1 %2 %3 %4").arg(g.x()).arg(g.y()).arg(g.width()).arg(g.height()));
   } else {
      emit responseReady(cli, "ERROR", "obs_frontend_get_main_window() return null window");
   }

}

void OBSCommandHandler::windowGeometry(Client *cli) {

   QMainWindow *wnd = (QMainWindow*)obs_frontend_get_main_window();

   if (wnd != nullptr) {
      const QRect g = wnd->geometry();
      emit responseReady(cli, "OK", QString("%1 %2 %3 %4").arg(g.x()).arg(g.y()).arg(g.width()).arg(g.height()));
   } else {
      emit responseReady(cli, "ERROR", "obs_frontend_get_main_window() returned null window");
   }
}

void OBSCommandHandler::sendUsage(Client *cli) {

    QString usage = "\n";
    QTextStream ts(&usage);

    ts << ""
    << "\n scene <action> [<args>...]                             execute a scene action\n\n"
    << "     list                                                list available scenes\n"
    << "     set <scene index>                                   select scene by index.\n"
    << "     switch                                              switch to scene in preview screen\n"
    << "\n streaming <action> [<args>...]                         handle streaming\n\n"
    << "     start                                               start streaming service\n"
    << "     stop                                                stop streaming service\n"
    << "     status                                              print streaming service status <active|inactive>\n"
    << "     info                                                print streaming service type and settings\n"
    << "     config <attribute_name> <attribute_value>           update a streaming service configuration\n"
    << "\n recording <action> [<args>...]                         handle streaming\n\n"
    << "     start                                               start recording\n"
    << "     stop                                                stop recording\n"
    << "     status                                              print recording status <active|inactive>\n"
    << "\n audio <action> [<args>...]                             handle audio devices\n\n"
    << "     list                                                list audio devices\n"
    << "     mute <device index>                                 mute audio device by index\n"
    << "     unmute <device index>                               unmute audio device by index\n"
    << "\n window <action> [<args>...]                            handle obs main window\n\n"
    << "     geometry [<x> <y> <width> <height>]                 get/set obs main window geometry\n"
    << "\n";

    emit responseReady(cli, "COMMANDS", usage);
}
