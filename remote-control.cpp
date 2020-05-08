#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <sys/stat.h>
#include <obs-frontend-api.h>

#include <QMainWindow>
#include <QAction>

#include "remotecontroldialog.h"
#include "obscommandhandler.h"

#define blog(log_level, format, ...)                    \
	blog(log_level, "[remote-control] " format, ##__VA_ARGS__)

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)

OBSCommandHandler *handler;
RemoteControlDialog *rcUi;

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("remote-control", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "Listen for remote commands: transition, start, stop";
}

void show_scenes() {

   char **scenes = obs_frontend_get_scene_names();
   char *p;
   int count = 0;

   info("Loading remote control service...");

   if (scenes != NULL) {

      info("Scenes: ");

      while ((p = scenes[count++]) != NULL) {
         info("\t[%d] %s", count, p);
      }

   } else {
      info("No scenes available");
   }
}

void rc_create_menu() {

	QAction *action = (QAction *)obs_frontend_add_tools_menu_qaction(
		obs_module_text("Remote Control Server"));

	QMainWindow *window = (QMainWindow *)obs_frontend_get_main_window();

	obs_frontend_push_ui_translation(obs_module_get_string);
   rcUi = new RemoteControlDialog(window, handler);
	obs_frontend_pop_ui_translation();

	action->connect(action, &QAction::triggered, []{ rcUi->show(); });
}

bool obs_module_load(void)
{
   handler = new OBSCommandHandler();

   rc_create_menu();

	return true;
}
