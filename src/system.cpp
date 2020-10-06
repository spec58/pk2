//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//#########################
#include "system.hpp"

#include "engine/PLog.hpp"
#include "engine/PUtils.hpp"
#include "engine/PDraw.hpp"
#include "settings.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
//C++17
#include <filesystem>

namespace fs = std::filesystem;

int screen_width  = 800;
int screen_height = 480;

//Android const paths
char* External_Path, Internal_Path;

std::string data_path;
bool external_dir = false;

int game_assets = -1;
int game_assets2 = -1;
int bg_screen = -1;

int key_delay = 0;

double cos_table[360];
double sin_table[360];
int degree = 0;
int degree_temp = 0;

bool test_level = false;
bool dev_mode = false;

bool doublespeed = false;

bool show_fps = false;

bool PK2_error = false;
const char* PK2_error_msg = nullptr;

int PK2_Error(const char* msg) {
	
	PK2_error = true;
	PK2_error_msg = msg;

	return 0;
}

void Calculate_SinCos(){

	for ( int i = 0; i < 360; i++ ) {
	
		cos_table[i] = cos(M_PI*2*i/180) * 33;
		sin_table[i] = sin(M_PI*2*i/180) * 33;
	
	}

}

void Draw_Cursor(int x, int y){

	PDraw::image_cutclip(game_assets,x,y,621,461,640,480);
	
}

void Prepare_DataPath() {

	PUtils::CreateDir(data_path);
	PUtils::CreateDir(data_path + "scores" PE_SEP);
	PUtils::CreateDir(data_path + "mapstore" PE_SEP);

}

void Move_DataPath(std::string new_path) {

	PLog::Write(PLog::DEBUG, "PK2", "Moving data from %s to %s", data_path.c_str(), new_path.c_str());

	for(fs::path p : fs::directory_iterator(data_path)) {
		std::string dst = new_path/p.filename();
		PLog::Write(PLog::DEBUG, "PK2", "	rename file %s -> %s", p.c_str(), dst.c_str());
        fs::rename(p, dst);
    }

	data_path = new_path;

}

//TODO - Receive Episode, organize this
bool FindAsset(PFile::Path* path, const char* default_dir) {

	if (!path->Find()) {

		PLog::Write(PLog::WARN, "PK2", "Can't find %s", path->c_str());

		path->SetPath(default_dir);
		PLog::Write(PLog::INFO, "PK2", "Trying %s", path->c_str());
		
		if (!path->Find()) {
			
			PLog::Write(PLog::WARN, "PK2", "Can't find %s", path->c_str());

			if (path->Is_Zip()) {

				PLog::Write(PLog::INFO, "PK2", "Trying outsize zip");

				std::string filename = path->GetFileName();

				*path = PFile::Path(default_dir);
				path->SetFile(filename);
				if (!path->Find()) {

					PLog::Write(PLog::ERR, "PK2", "Can't find %s", path->c_str());
					return false;

				}

			}
		
		}
		
	}

	return true;

}