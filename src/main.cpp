//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//-------------------------
//It can be started with the "dev" argument to start the
//cheats and "test" follown by the episode and level to
//open directely on the level.
//	Exemple:
//	"./pekka-kana-2 dev test rooster\ island\ 2/level13.map"
//	Starts the level13.map on dev mode
//#########################
#include "engine/Piste.hpp"
#include "version.hpp"

#include "screens/screens.hpp"
#include "gfx/text.hpp"
#include "game/game.hpp"
#include "episode/episodeclass.hpp"
#include "episode/mapstore.hpp"
#include "save.hpp"
#include "gui.hpp"
#include "system.hpp"
#include "language.hpp"
#include "settings.hpp"

#include <cstring>
#include <algorithm>

#include <SDL.h>

void start_test(const char* arg) {
	
	if (arg == NULL) return;

	PFile::Path path(arg);

	episode_entry episode;
	episode.name = path.GetPath();
	episode.is_zip = false;
	Episode = new EpisodeClass("test", episode);

	Game = new GameClass(path.GetFileName());

	PLog::Write(PLog::DEBUG, "PK2", "Testing episode '%s' level '%s'", episode.name.c_str(), Game->map_file.c_str());

}

void quit() {

	Settings_Save();

	PSound::stop_music();
	GUI_Exit();

	if (Game)
		delete Game;
	
	if (Episode && !test_level) {
		Save_Record(10); //Save #10 is the backup
		delete Episode;
	}
	
	if(tekstit)
		delete tekstit;

	Piste::terminate();
	PLog::Write(PLog::DEBUG, "PK2", "Pekka Kana 2 terminated");
	PLog::Exit();

}

int main(int argc, char *argv[]) {

	char* test_path = NULL;
	bool path_set = false;

	// Read args
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "version") == 0) {
			printf(PK2_VERSION_STR "\n");
			exit(0);
		}
		if (strcmp(argv[i], "dev") == 0) {
			dev_mode = true;
			Piste::set_debug(true);
		}
		else if (strcmp(argv[i], "test") == 0) {
			if (argc <= i + 1) {
				printf("Please set a level to test\n");
				exit(1);
			}
			else {
				i++;
				test_path = argv[i];
				test_level = true;
				continue;
			}
		}
		else if (strcmp(argv[i], "path") == 0) {
			if (argc <= i + 1) {
				printf("Please set a path\n");
				exit(1);
			}
			else {
				i++;
				if (chdir(argv[i]) != 0) {
					printf("Invalid path\n");
					exit(1);
				}
				path_set = true;
				continue;
			}
		}
		else if (strcmp(argv[i], "fps") == 0) {
			show_fps = true;
			continue;
		}
		else if (strcmp(argv[i], "mobile") == 0) {
			PUtils::Force_Mobile();
		}
		else {
			printf("Invalid arg\n");
			exit(1);
		}

	}

	PLog::Init(PLog::ALL, PFile::Path(""));

	if(!path_set)
		PUtils::Setcwd();

	// Set data_path

	#ifndef __ANDROID__

	#ifdef PK2_PORTABLE

	data_path = "." PE_SEP "data" PE_SEP;
	PUtils::CreateDir(data_path);

	#else

	char* data_path_p = SDL_GetPrefPath(NULL, PK2_NAME);
	if (data_path_p == NULL) {

		PLog::Write(PLog::FATAL, "PK2", "Failed to init data path");
		quit();
		return 1;

	}

	data_path = data_path_p;
	SDL_free(data_path_p);

	#endif //PK2_PORTABLE

	// Read redirect file to change data directory
	PFile::Path redirect = PFile::Path(data_path, "redirect.txt");
	if (redirect.Find()) {

		PLog::Write(PLog::DEBUG, "PK2", "Found redirect");
		
		PFile::RW *redirect_rw = redirect.GetRW("r");

		char* buffer;
		int size = redirect_rw->to_buffer((void**) &buffer);
		redirect_rw->close();
		
		if (size > 0) {

			buffer[size - 1] = '\0';
			PLog::Write(PLog::DEBUG, "PK2", "Changing path to %s", buffer);

			data_path = buffer;
			SDL_free(buffer);
		
		}

	}

	#else //__ANDROID__

	//Must be free-ed at the end (but who cares?)
	External_Path = SDL_AndroidGetExternalStoragePath();
	Internal_Path = SDL_AndroidGetInternalStoragePath();

	if (!External_Path)
		PLog::Write(PLog::ERR, "PK2", "Couldn't find External Path");
	if (!Internal_Path)
		PLog::Write(PLog::ERR, "PK2", "Couldn't find Internal Path");

	PLog::Write(PLog::DEBUG, "PK2", "External %s", External_Path);
	PLog::Write(PLog::DEBUG, "PK2", "Internal %s", Internal_Path);

	// Choose between internal or external path on Android
	if (SDL_AndroidGetExternalStorageState() | SDL_ANDROID_EXTERNAL_STORAGE_WRITE) {

		PLog::Write(PLog::DEBUG, "PK2", "External access allowed");

		PFile::Path settings_f = PFile::Path("settings.ini");
		settings_f.SetPath(Internal_Path);
		if (!settings_f.Find()) {

			PLog::Write(PLog::DEBUG, "PK2", "Settings not found on internal");

			settings_f.SetPath(External_Path);
			if (settings_f.Find()) {

				PLog::Write(PLog::DEBUG, "PK2", "Settings found on external");
				external_dir = true;
			
			}
			else {

				PLog::Write(PLog::DEBUG, "PK2", "Settings not found on external");
				external_dir = false;

			}
		} else {

			PLog::Write(PLog::DEBUG, "PK2", "Settings found on internal");
			external_dir = false;

		}
	} else {

		PLog::Write(PLog::DEBUG, "PK2", "External access not allowed");
		external_dir = false;

	}

	if (external_dir)
		data_path = External_Path;
	else
		data_path = Internal_Path;

	data_path += PE_SEP;

	#endif //__ANDROID__

	//Now data_path is set

	PLog::Init(PLog::ALL, PFile::Path(data_path + "log.txt"));

	PLog::Write(PLog::DEBUG, "PK2", "Pekka Kana 2 started!");
	PLog::Write(PLog::DEBUG, "PK2", "Game version: %s", PK2_VERSION_STR);
	PLog::Write(PLog::DEBUG, "PK2", "Number: 0x%x", PK2_VERNUM);

	#ifdef COMMIT_HASH
	PLog::Write(PLog::DEBUG, "PK2", "Cammit hash: " COMMIT_HASH);
	#endif

	PLog::Write(PLog::DEBUG, "PK2", "Data path - %s", data_path.c_str());
	Prepare_DataPath();

	Settings_Open();

	// TODO - set screen_width

	Piste::init(screen_width, screen_height, PK2_NAME, "gfx" PE_SEP "icon.bmp", Settings.audio_buffer_size);
	if (!Piste::is_ready()) {

		PLog::Write(PLog::FATAL, "PK2", "Failed to init PisteEngine");
		quit();
		return 1;

	}

	Screen_First_Start();

	next_screen = SCREEN_INTRO;
	if (dev_mode)
		next_screen = SCREEN_MENU;
	if (test_level) {
		start_test(test_path);
		next_screen = SCREEN_GAME;
	}

	Piste::loop(Screen_Loop); //The game loop

	if(PK2_error) {

		PLog::Write(PLog::ERR, "PK2", PK2_error_msg);
		PUtils::Show_Error(PK2_error_msg);
		
	}

	quit();
	return 0;
}
