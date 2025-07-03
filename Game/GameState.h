#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

//=================================================================
//Possible GameStates
//=================================================================

enum GameState {
	GS_NULL = 0,
	GS_ATTRACT,
	GS_PLAY_MAIN_CAM,
	GS_PLAY_TPS_CAM,
	GS_LEVEL_WON,
	GS_WAITING_TO_CONTINUE,
	GS_PAUSE,
	GS_GAME_OVER,
	GS_COUNT,
	GS_MAIN_MENU,
};

#endif
