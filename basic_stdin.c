#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <asndlib.h>
#include <mp3player.h>
#include "music_mp3.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void init_audio() {
    ASND_Init();
    MP3Player_Init();
    
    MP3Player_PlayBuffer(music_mp3, music_mp3_size, NULL);
}

bool quitapp = false;

void keyPress_cb(char sym) {
    if (sym > 31) putchar(sym);
    if (sym == 13) putchar('\n');

    if (sym == 0x1b) quitapp = true;
}

int main(int argc, char **argv) {
    // Initialise the video system
    VIDEO_Init();

    // This function initialises the attached controllers
    WPAD_Init();

    // Audio Subsystem
    init_audio();
    
    // Obtain the preferred video mode from the system
    // This will correspond to the settings in the Wii menu
    rmode = VIDEO_GetPreferredMode(NULL);

    // Allocate memory for the display in the uncached region
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Initialise the console, required for printf
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    // Set up the video registers with the chosen mode
    VIDEO_Configure(rmode);

    // Tell the video hardware where our display memory is
    VIDEO_SetNextFramebuffer(xfb);

    // Make the display visible
    VIDEO_SetBlack(false);

    // Flush the video register changes to the hardware
    VIDEO_Flush();

    // Wait for Video setup to complete
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // The console understands VT terminal escape codes
    // This positions the cursor on row 2, column 0
    // we can use variables for this with format codes too
    // e.g. printf("\x1b[%d;%dH", row, column);
    printf("\x1b[33;1m"); // Yellow text color
    printf(
        " .-----------------------TypeMii by August Wolf-----------------------.\n"
        " .--------------------------------------------------------------------.\n"
        " | [Esc] [F1][F2][F3][F4][F5][F6][F7][F8][F9][F0][F10][F11][F12] o o o|\n"
        " | [`][1][2][3][4][5][6][7][8][9][0][-][=][_<_] [I][H][U] [N][/][*][-]|\n"
        " | [|-][Q][W][E][R][T][Y][U][I][O][P][{][}] | | [D][E][D] [7][8][9]|+||\n"
        " | [CAP][A][S][D][F][G][H][J][K][L][;]['][#]|_|           [4][5][6]|_||\n"
        " | [^][\\][Z][X][C][V][B][N][M][,][.][/] [__^__]    [^]    [1][2][3]| ||\n"
        " | [c]   [a][________________________][a]   [c] [<][V][>] [ 0  ][.]|_||\n"
        " `--------------------------------------------------------------------'\n"
        " `-----------------Press ESC on your keyboard to exit-----------------'\n"
    );

if (KEYBOARD_Init(keyPress_cb) == 0) {
    printf("\n");
}

	do {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		int key = getchar();

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) quitapp=true;

		// Wait for the next frame
		VIDEO_WaitVSync();
	} while(!quitapp);

	return 0;
}