#pragma once

#include "frustum/Frustum.h"
#include <raylib.h>
#include <vector>

#ifndef NO_DEBUG
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

#define HEIGHT 800
#define WIDTH 1400

#define SPEAKER_NARRATOR "<NARR>"

#define MAN_SPRITE_STAND_SOURCE                                                \
	{ 0, 0, 32, 32 }
#define MAN_SPRITE_MIDWALK_SOURCE                                              \
	{ 32, 0, 32, 32 }
#define MAN_SPRITE_WALK1_SOURCE                                                \
	{ 0, 32, 32, 32 }
#define MAN_SPRITE_WALK2_SOURCE                                                \
	{ 32, 32, 32, 32 }

#ifdef PLATFORM_WEB
#define KEY_E_SOURCE                                                           \
	{ 0, 0, 16, 16 }
#else
#define KEY_E_SOURCE                                                           \
	{ 0, 0, 16, 16 }
#endif

#define PFLAG state->story.flags
#define PDAY state->story.day

#define SSD state.station.dialogue
#define SSDP state->station.dialogue

#define SETMODEL(modelVar, model)                                              \
	modelVar = IsModelReady(*model) ? model : &state->models.errorFallback

#define PLAYERBOUNDS(MIN, MAX)                                                 \
	if (state->station.playerPosition.x <= MIN) {                              \
		state->station.playerPosition.x = MIN;                                 \
	}                                                                          \
	if (state->station.playerPosition.x >= MAX) {                              \
		state->station.playerPosition.x = MAX;                                 \
	}

#define DIALOG_TODO -1
#define DIALOGS(SPEAKER, MESSAGE, NEXTDIALOG, SOUND)                           \
	writeToCharArr(MESSAGE, state->station.dialogue.fullMessage);              \
	state->station.dialogue.maxChars = strlen(MESSAGE);                        \
	writeToCharArr(SPEAKER, state->station.dialogue.speaker);                  \
	state->station.dialogue.dialogueSound = SOUND;                             \
	state->station.dialogue.nextDialog = NEXTDIALOG;

#define DIALOG(SPEAKER, MESSAGE, NEXTDIALOG)                                   \
	DIALOGS(SPEAKER, MESSAGE, NEXTDIALOG, DIALOG_SOUND::PROTAG)

#define PSETPLAYERPOS(x_pos) state->station.playerPosition.x = x_pos

#define INTERACTABLE(X, Y, W, H, DIALOGNUM)                                    \
	if (CheckCollisionRecs(                                                    \
			{stationPlyPos.x - 23.0f, stationPlyPos.y - 32, 32, 3},            \
			{X, Y, W, H})) {                                                   \
		if (state->station.stationState != DIALOG)                             \
			DrawTexturePro(state->textures.button_e, KEY_E_SOURCE,             \
			               {X + (W / 2), Y - 16, 16, 16}, {8, 0}, 0, WHITE);   \
		if (DIALOGCHECK) {                                                     \
			undertale(state, DIALOGNUM);                                       \
			break;                                                             \
		} else {                                                               \
			break;                                                             \
		}                                                                      \
	}

#define ENDDIALOGUEP state->station.stationState = WALK;

#define RAND_FLOAT (float)(rand() % 100000) / 100000
#define RAND_FLOAT_BI (float)(((rand() % 200000) / 200000) - 1)

#define RAND_VEC3                                                              \
	{                                                                          \
		(float)(((float)(rand() % 200000) / 100000.0f) - 1),                   \
			(float)(((float)(rand() % 200000) / 100000.0f) - 1),               \
			(float)(((float)(rand() % 200000) / 100000.0f) - 1)                \
	}

#define EXPAND_VEC3(vec3) vec3.x, vec3.y, vec3.z
#define EXPAND_VEC2(vec2) vec2.x, vec2.y

#define ANIM_SPEED 10.0f / 60.0f
#define WALK_SPEED 50

#define DOOROPENP PlaySound(state->sounds.doorOpen)
#define DOORCLOSEP PlaySound(state->sounds.doorClose)

#define SOUNDALIAS_ARR_SIZE 8

#define DIALOG_VOLUME 0.5f

#define DRAWTEXTCENTER(str, x, y, size, color) DrawText(str, x-(MeasureText(str,size)/2), y, size, color)

const Rectangle WALKING_CYCLE_ARR[5] = {
	MAN_SPRITE_WALK1_SOURCE, MAN_SPRITE_MIDWALK_SOURCE, MAN_SPRITE_WALK2_SOURCE,
	MAN_SPRITE_MIDWALK_SOURCE, MAN_SPRITE_STAND_SOURCE};

extern Vector3 rotation;

enum GAME_STATE {
	STATE_PRETITLE,
	STATE_TITLE,
	STATE_STATION, // 2d
	STATE_SHIP,
	STATE_AFTERSCARE
};

enum GAMEOVER_CAUSE { OUTOFBOUNDS, EXCESSIVESPIN, OBJECTCOLLISION };

// mainly used for transitions between station and ship gameplay states.
enum SHIP_SUBSTATE { SST_DESCEND, SST_ACTION, SST_ASCEND, SST_FAILED };

enum STATION_SUBSTATE {
	WALK,
	DIALOG,
	SLEEPING, // squirrel stapler reference
};

enum STATION_LEVEL {
	TEST_LEVEL,
	TEST_LEVEL_TRANSITION,

	INTRO,           // room 00
	PROTAG_ROOM,     // room 01
	MAIN_HALLWAY,    // room 02, camera follows here.
	SHIP_BOARDING,   // room 03
	CAFETERIA,       // room 04
	MANAGERS_OFFICE, // room 05
};

enum ON_TRANSITION_PEAK {
	STATION_TO_SHIP,
	SHIP_TO_STATION,
	STATION_LEVEL_TO_LEVEL,
};

enum class DIALOG_SOUND { NONE, PROTAG, MANAGER, PETER, AI };

struct ShipAsteroid; // "prototyped" to avoid a compile error

struct SoundPack {
	Sound base;
	Sound alias[SOUNDALIAS_ARR_SIZE];
	void Load(Sound);
	void SetVolume(float);
	void Play();
	int soundsPlaying();
};

struct Particle {
	Vector3 pos;
	Vector3 vel;
	float size; // for billboards, size is squared
	float lifetime;
	float originalLifetime;

	Texture* tex;
};

struct GameState {
	GAME_STATE gameState;
	bool paused;
	struct {
		bool soundPlayed = false;
		float timeToStateChange = 3;
	} pretitle;
	struct { // station
		Vector2 playerPosition;
		STATION_SUBSTATE stationState;
		STATION_LEVEL stationLevel;
		struct {
			bool direction;   // 0 = left, 1 = right
			int currentCycle; // see WALKING_CYCLE_ARR
			float timeToNextCycle;
		} anim;
		struct {     // dialogue
			int num; // tied to what dialogue to use, 0 = no dialogue
			char speaker[64];
			char fullMessage[512];
			char displayMessage[512];
			int chars; // current characters of current dialogue to display. see
			           // undertale
			int maxChars; // maximum characters of current dialogue. also see
			              // undertale
			float timeToNextChar;
			bool nextDialog; // Is the next dialog possible for this one?
			bool readyToAdvance;
			bool alreadyDisengaged;
			DIALOG_SOUND dialogueSound;
		} dialogue;
		struct {
			bool active;
			float position;
		} monster;
		struct {
			std::vector<ShipAsteroid> asteroids;
			Camera3D cam;
			float maxTime;
			float time;
			float rotation;
		} sleep;
	} station;
	struct { // ship
		SHIP_SUBSTATE state;
		GAMEOVER_CAUSE deathCause;
		bool causeWritten;
		char deathCauseMsg[512];
		int originalAsteroids[3]; // 0 = normal, 1 = burrowed, 2 = b + m
		Vector2 shipPosition;
		Vector2 shipVelocity;
		float shipRotation;
		float shipRotVelo;
		float laserCharge; // also 1.00 to 0.00. about 5 seconds to use up, 10
		                   // seconds to recharge
		bool isLasering;
		float timeUntilNextDecal; // default 0.1
		int decalIndex;
		bool laserHit;
		Vector2 laserCollide;
		float airBreakCharge; // 1.00 to 0.00

		std::vector<ShipAsteroid> asteroids;
		std::vector<Particle> particles;
		int originalAsteroidsSize;

		Vector3 originalReturnPos;
		float ascensionLerp;
		int ascensionState = 0;
		struct {
			// monster should go to 0,0 (station) before going inactive
			// actually make it have transitions it would be very cool lul
			bool active;
			float rot;
			Vector2 pos;
			Vector2 posVel;

			bool reachedCenter;
			float yCoord;

			int animIndex;
			float animCycle;
		} monster;
	} ship;
	struct {
		float timeUntil;
		int state;
	} afterscare;
	struct {
		Texture raylibLogo;
		Texture button_e;
		Texture button_r;

		Texture basePlayer;
		Texture basePlayerFinale;

		Texture testRoom;
		Texture protagRoom;

		Texture mainHall;
		Texture mainHallDay2;
		Texture mainHallFinale;

		Texture cafeteria;
		Texture managerRoom;

		Texture shipBoarding;
		Texture shipBoardingFinale;

		Texture shipUiLeft;
		Texture shipUiRight;

		Texture shipUiArrow;
		Texture shipAirbreakGauge;
		Texture shipLaserGauge;

		Texture shipLaserImpact;
		Texture shipMonsterSprite;
		Texture stationMonsterSprite;
	} textures;
	struct {
		Model errorFallback; // Used when something went wrong when determining
		                     // a model

		Model size5AstBurrowed;
		Model size4AstBurrowed;
		Model size3AstBurrowed;
		Model size2AstBurrowed;
		Model size1AstBurrowed;

		Model size5Ast;
		Model size4Ast;
		Model size3Ast;
		Model size2Ast;
		Model size1Ast;

		Model station;
	} models;
	struct {
		Sound coin;

		Sound phoneRing;
		Sound doorOpen;
		Sound doorClose;
		Sound walkLeft;
		Sound walkRight;

		Sound explode;
		Sound mined;
		Sound lasering;
		Sound rocket;

		Sound spaceAmbience;
		Sound jumpscare;

		SoundPack protagDialog;
		SoundPack bossDialog;
		SoundPack aiDialog;
		SoundPack peterDialog;
	} sounds;
	struct {
		bool active;
		bool atPeak;
		float transitionTime;
		float maxTransitionTime;
		int onPeak;
		int toLevel;

		bool dialogAfter;
		int dialogNum;

		bool closeSoundAfter;
	} transition;
	struct {
		int day = 1;    // as in days 1-5
		bool flags[32]; // TODO: document flags
	} story;
#if DEBUG_MODE
	struct {
		float asteroidHitboxSizes[5];
		bool drawHitboxes = false;
		Vector2 laserPos;
		Vector2 laserEdge;
		Vector2 laserCollide;
		float multiplier = 1;
		float zoom2d = 1;
		Vector3 testCubePos;
		Vector3 testCubeSize;
		bool ship2dRep;

		bool drawTestRec = false;
		Rectangle testRec;

		int testDialogNum;
	} debug;
#endif
};

struct ShipAsteroid {
	Vector2 pos;
	Vector2 velocity;
	Vector4 cosAsteroidRotation;
	bool burrowed;
	bool monster;
	int size; // see GameState.models

	Model* model;
	float durability;
	bool lasered;
	// Must be called during drawing in 3d mode
	void DrawAst(GameState*, Camera3D*);
	float getSphereRad();
};
void DoPreTitle(GameState*);

void DoTitle(GameState*);

void DrawStation(GameState*);
void DoStation(GameState*, Camera2D*, Camera3D*);

void DoShip(GameState*, Camera3D*);

void DoAfterscare(GameState*);

void CheckInteract(GameState*);
void HandleState(GameState*, Camera3D*);
void undertale(GameState*, int);
void transitionToShip(GameState*, int, int, int, bool = false);
void transitionToStationLevel(GameState*, int, bool = false, int = -1,
                              bool = true);
void transitionToSleep(GameState*);
void onTransitionPeak(GameState*, Camera3D*);
void walkSound(GameState*);

void writeToCharArr(const char*, char*, int = -1);
void DrawPlayerTex(GameState*);

Vector3 Vector2to3XZ(Vector2);
