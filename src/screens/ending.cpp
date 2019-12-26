
#include "screens.hpp"

#include "gfx/text.hpp"
#include "gui.hpp"
#include "game.hpp"
#include "sfx.hpp"
#include "language.hpp"

#include "PisteDraw.hpp"
#include "PisteInput.hpp"
#include "PisteSound.hpp"

#include "types.hpp"

DWORD loppulaskuri = 0;
bool siirry_lopusta_menuun = false;

int Draw_EndGame_Image(int x, int y, int tyyppi, int plus, int rapytys){
	int frm = 0;
	int yk = 0;

	if (tyyppi == 1){ // Pekka
		frm = 1;
		if ((degree/10)%10==rapytys) frm = 0;
		yk = (int)sin_table[(degree%360)];
		PDraw::image_cutclip(kuva_tausta,x+3,y+56, 4, 63, 29, 69);
		if (yk < 0){
			y+=yk;
			frm = 2;
		}
		PDraw::image_cutclip(kuva_tausta,x,y, 1+frm*35, 1, 32+frm*35, 59);
	}

	if (tyyppi == 2){ // kana (katse oikealle)
		frm = 0;
		if ((degree/10)%10==rapytys) frm = 1;
		yk = (int)cos_table[((degree+plus)%360)];
		PDraw::image_cutclip(kuva_tausta,x+3,y+36, 4, 63, 29, 69);
		if (yk < 0) {
			y+=yk;
			frm = 2;
		}
		PDraw::image_cutclip(kuva_tausta,x,y, 106+frm*37, 1, 139+frm*37, 38);
	}

	if (tyyppi == 3){ // kana (katse vasemmalle)
		frm = 0;
		if ((degree/10)%10==rapytys) frm = 1;
		yk = (int)cos_table[((degree+plus)%360)];
		PDraw::image_cutclip(kuva_tausta,x+3,y+36, 4, 63, 29, 69);
		if (yk < 0) {
			y+=yk;
			frm = 2;
		}
		PDraw::image_cutclip(kuva_tausta,x,y, 106+frm*37, 41, 139+frm*37, 77);
	}

	if (tyyppi == 4){ // pikkukana (katse oikealle)
		frm = 0;
		if ((degree/10)%10==rapytys) frm = 1;
		yk = (int)sin_table[(((degree*2)+plus)%360)];
		PDraw::image_cutclip(kuva_tausta,x+3,y+27, 4, 63, 29, 69);
		if (yk < 0) {
			y+=yk;
			frm = 2;
		}
		PDraw::image_cutclip(kuva_tausta,x,y, 217+frm*29, 1, 243+frm*29, 29);
	}

	if (tyyppi == 5){ // pikkukana (katse vasemmalle)
		frm = 0;
		if ((degree/10)%10==rapytys) frm = 1;
		yk = (int)sin_table[(((degree*2)+plus)%360)];
		PDraw::image_cutclip(kuva_tausta,x,y+27, 4, 63, 29, 69);
		if (yk < 0) {
			y+=yk;
			frm = 2;
		}
		PDraw::image_cutclip(kuva_tausta,x,y, 217+frm*29, 33, 243+frm*29, 61);
	}

	return 0;
}
int Draw_EndGame(){

	DWORD onnittelut_alku	= 300;
	DWORD onnittelut_loppu	= onnittelut_alku + 1000;
	DWORD the_end_alku		= onnittelut_loppu + 80;
	DWORD the_end_loppu		= the_end_alku + 3000;

	PDraw::screen_fill(0);
	PDraw::image_cutclip(kuva_tausta,320-233/2,240-233/2, 6, 229, 239, 462);

	Draw_EndGame_Image(345, 244, 3, 30, 2);
	Draw_EndGame_Image(276, 230, 2, 50, 3);
	Draw_EndGame_Image(217, 254, 4, 0, 4);

	Draw_EndGame_Image(305, 240, 1, 0, 1);

	Draw_EndGame_Image(270, 284, 2, 20, 1);
	Draw_EndGame_Image(360, 284, 5, 60, 2);

	if (loppulaskuri > onnittelut_alku) {
		CreditsText_Draw(tekstit->Hae_Teksti(PK_txt.end_congratulations), fontti2, 220, 380, onnittelut_alku, onnittelut_loppu, loppulaskuri);
		CreditsText_Draw(tekstit->Hae_Teksti(PK_txt.end_chickens_saved), fontti1, 220, 402, onnittelut_alku+30, onnittelut_loppu+30, loppulaskuri);
	}
	if (loppulaskuri > the_end_alku) {
		CreditsText_Draw(tekstit->Hae_Teksti(PK_txt.end_the_end), fontti2, 280, 190, the_end_alku, the_end_loppu, loppulaskuri);
	}

	return 0;
}

int Screen_Ending_Init() {
	
	GUI_Change(UI_TOUCH_TO_START);
	
	PDraw::set_xoffset(Settings.isWide? 80 : 0);

	PDraw::image_load(kuva_tausta, "gfx/ending.bmp", true);

	if (PSound::start_music("music/intro.xm") != 0)
		PK2_Error("Can't load intro.xm");

	PSound::set_musicvolume(Settings.music_max_volume);

	loppulaskuri = 0;
	siirry_lopusta_menuun = false;
	//episode_started = false;

	PDraw::fade_in(PDraw::FADE_FAST);
}

int Screen_Ending(){

	Draw_EndGame();

	degree = 1 + degree % 360;

	loppulaskuri++;
	//introlaskuri = loppulaskuri; // introtekstej� varten

	if (siirry_lopusta_menuun && !PDraw::is_fading())
	{
		next_screen = SCREEN_MENU;
		//menu_nyt = MENU_MAIN;
		//episode_started = false;
	}

	if (key_delay > 0)
		key_delay--;

	if (key_delay == 0) {
		if (PisteInput_Keydown(PI_RETURN) || PisteInput_Keydown(PI_SPACE)) {
			siirry_lopusta_menuun = true;
			PSound::set_musicvolume(0);
			PDraw::fade_out(PDraw::FADE_SLOW);
		}
	}

	return 0;
}