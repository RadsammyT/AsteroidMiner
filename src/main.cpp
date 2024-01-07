#include "game.h"
#include "rayImGui/imgui.h"
#include "rayImGui/rlImGui.h"
#include <cstdlib>
#include <ctime>
#include <raylib.h>

int main() {
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "Asteroid Miner");
	InitAudioDevice();
	srand(time(NULL));
	rotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT};
#if defined(PLATFORM_WEB)
	SetExitKey(NULL);
#endif
	SetTargetFPS(60);
#if DEBUG_MODE
	rlImGuiSetup(true);
	bool drawTestTex = true;
	bool drawImGuiDemo = false;
	bool drawplayerPosPoint = false;
	bool infAirBreak = false;
	bool infLaserCharge = false;
	Texture testTex = LoadTexture("resources/web/manBase.png");
	Rectangle source, dest = {};
	Vector2 origin = {};
	float rotation = 0;
#endif
	GameState state = {};

	state.gameState = STATE_PRETITLE;
	state.station.playerPosition = {0, 0};
	state.station.stationLevel = INTRO;
	state.station.stationState = WALK;
	state.station.dialogue = {
		.timeToNextChar = 0,
	};

	undertale(&state, 1'00'000);

	state.transition = {};
	state.station.anim.currentCycle = 3;
#ifdef PLATFORM_WEB
	state.textures.button_e = LoadTexture("resources/web/e_button.png");
	state.textures.button_r = LoadTexture("resources/web/r_button.png");
#else
	state.textures.button_e = LoadTexture("resources/web/e_button.png");
	state.textures.button_r = LoadTexture("resources/web/r_button.png");
#endif
	state.textures.raylibLogo = LoadTexture("resources/web/raylib.png");
	state.textures.basePlayer = LoadTexture("resources/web/manBase.png");
	state.textures.basePlayerFinale = LoadTexture("resources/web/manBase_finale.png");

	state.textures.testRoom = LoadTexture("resources/web/test_room_16.png");
	state.textures.protagRoom =
		LoadTexture("resources/web/protag_room_furnished.png");
	state.textures.mainHall = LoadTexture("resources/web/main_hall.png");
	state.textures.mainHallDay2 =
		LoadTexture("resources/web/main_hall_day_2.png");
	state.textures.mainHallFinale = LoadTexture("resources/web/main_hall_finale.png");
	state.textures.cafeteria = LoadTexture("resources/web/cafe_furnished.png");
	state.textures.managerRoom =
		LoadTexture("resources/web/manager_office_furnished.png");
	state.textures.shipBoarding =
		LoadTexture("resources/web/ship_boarding_furnished.png");
	state.textures.shipBoardingFinale = LoadTexture("resources/web/ship_boarding_finale.png");

	state.textures.shipUiLeft = LoadTexture("resources/web/ship_ui_left.png");
	state.textures.shipUiRight= LoadTexture("resources/web/ship_ui_right.png");

	Image tempArrow = LoadImage("resources/web/uiArrow.png");
	ImageColorInvert(&tempArrow);
	state.textures.shipUiArrow = LoadTextureFromImage(tempArrow);
	UnloadImage(tempArrow);
	state.textures.shipAirbreakGauge =
		LoadTexture("resources/web/uiAirbreak.png");
	state.textures.shipLaserGauge = LoadTexture("resources/web/uiLaser.png");

	state.textures.shipLaserImpact = LoadTexture("resources/web/laser_impact.png");
	state.textures.shipMonsterSprite =
		LoadTexture("resources/web/monster_space.png");
	state.textures.stationMonsterSprite = LoadTexture("resources/web/wtf_scary.png");

	Camera2D stationCam = {
		.offset = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2},
		.target = state.station.playerPosition,
		.rotation = 0.0f,
		.zoom = 5.0f,
	};

	Camera3D shipCam = {
		.position = {5, 0, 5},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 90.0f,
		.projection = CAMERA_PERSPECTIVE,
	};

	state.models.errorFallback = LoadModel("resources/web/missing.obj");

	state.models.size5Ast = LoadModel("resources/web/asteroid5.obj");
	state.models.size4Ast = LoadModel("resources/web/asteroid4.obj");
	state.models.size3Ast = LoadModel("resources/web/asteroid3.obj");
	state.models.size2Ast = LoadModel("resources/web/asteroid2.obj");
	state.models.size1Ast = LoadModel("resources/web/asteroid1.obj");

	state.models.size5AstBurrowed =
		LoadModel("resources/web/asteroid5_burrowed.obj");
	state.models.size4AstBurrowed =
		LoadModel("resources/web/asteroid4_burrowed.obj");
	state.models.size3AstBurrowed =
		LoadModel("resources/web/asteroid3_burrowed.obj");
	state.models.size2AstBurrowed =
		LoadModel("resources/web/asteroid2_burrowed.obj");
	state.models.size1AstBurrowed =
		LoadModel("resources/web/asteroid1_burrowed.obj");

	// note: if station model appears to be "inverted", as in faces only
	// showing from the inside, invert all the face normals in blender, then
	// re-export.
	state.models.station = LoadModel("resources/web/new_station.obj");

	state.sounds.spaceAmbience = LoadSound("resources/web/space-ambience.wav");
	state.sounds.coin = LoadSound("resources/web/coin.wav");
	state.sounds.phoneRing = LoadSound("resources/web/phone_low.wav");
	state.sounds.walkLeft = LoadSound("resources/web/walk1.wav");
	state.sounds.walkRight = LoadSound("resources/web/walk2.wav");
	state.sounds.rocket = LoadSound("resources/web/rocket.wav");
	state.sounds.explode = LoadSound("resources/web/asteroid_explosion.wav");
	state.sounds.mined = LoadSound("resources/web/asteroid_mined.wav");
	state.sounds.lasering = LoadSound("resources/web/laser_continuous.wav");
	state.sounds.doorOpen = LoadSound("resources/web/door_open.wav");
	state.sounds.doorClose = LoadSound("resources/web/door_close.wav");

	state.sounds.jumpscare = LoadSound("resources/web/jumpscare.wav");

	state.sounds.protagDialog.Load(
		LoadSound("resources/web/protag_dialog_sound.wav"));
	state.sounds.aiDialog.Load(LoadSound("resources/web/ai_dialog_sound.wav"));
	state.sounds.bossDialog.Load(
		LoadSound("resources/web/manager_dialog_sound.wav"));
	state.sounds.peterDialog.Load(
		LoadSound("resources/web/peter_dialog_sound.wav"));

	SetSoundVolume(state.sounds.lasering, 0.5);
	SetSoundVolume(state.sounds.rocket, 0.2);
	SetSoundVolume(state.sounds.coin, 0.5);
	SetSoundVolume(state.sounds.jumpscare, 0.3);
	state.sounds.protagDialog.SetVolume(DIALOG_VOLUME);
	state.sounds.aiDialog.SetVolume(DIALOG_VOLUME);
	state.sounds.bossDialog.SetVolume(DIALOG_VOLUME);
	state.sounds.peterDialog.SetVolume(DIALOG_VOLUME);
	while (!WindowShouldClose()) {
		BeginDrawing();
		if(state.gameState == GAME_STATE::STATE_PRETITLE) {
			DoPreTitle(&state);
		}
		if (state.gameState == GAME_STATE::STATE_TITLE) {
			DoTitle(&state);
		}
		if (state.gameState == GAME_STATE::STATE_STATION) {
			DoStation(&state, &stationCam, &shipCam);
		}
		if (state.gameState == GAME_STATE::STATE_SHIP) {
			DoShip(&state, &shipCam);
		}
		if(state.gameState == GAME_STATE::STATE_AFTERSCARE) {
			DoAfterscare(&state);
		}
#if DEBUG_MODE
		if (drawTestTex) {
			BeginMode2D(stationCam);
			DrawTexturePro(testTex, source, dest, origin, rotation, WHITE);
			EndMode2D();
		}
		if (drawplayerPosPoint) {
			BeginMode2D(stationCam);
			DrawCircleV(state.station.playerPosition, 1.5f, WHITE);
			EndMode2D();
		}
		if (infAirBreak) {
			state.ship.airBreakCharge = 1;
		}
		if (infLaserCharge) {
			state.ship.laserCharge = 1;
		}
		rlImGuiBegin();
		ImGui::Begin("Debug");
		ImGui::Text("FPS: %d", GetFPS());
		ImGui::Text("State: %u", state.gameState);
		ImGui::Checkbox("Show ImGui Demo Window", &drawImGuiDemo);
		if (ImGui::TreeNode("Station")) {
			ImGui::Text("station player pos: %.02f %.02f",
			            state.station.playerPosition.x,
			            state.station.playerPosition.y);
			ImGui::Text("station state: %d", state.station.stationState);
			ImGui::Text("station level: %d", state.station.stationLevel);
			ImGui::Checkbox("draw test tex", &drawTestTex);
			ImGui::Checkbox("draw playerPos point", &drawplayerPosPoint);
			if (drawTestTex) {
				ImGui::DragFloat4("Source", &source.x);
				ImGui::DragFloat4("Dest", &dest.x);
				ImGui::DragFloat2("Origin", &origin.x);
			}
			ImGui::Checkbox("draw test rec", &state.debug.drawTestRec);
			if (state.debug.drawTestRec) {
				ImGui::DragFloat4("testRec", &state.debug.testRec.x, 0.5);
			}
			if (ImGui::TreeNode("Story")) {
				ImGui::DragInt("Day/Story", &state.story.day);
				for (int i = 0; i < 32; i++) {
					if (ImGui::Button(TextFormat("%d/%d##BUTTON_%d", i,
					                             state.story.flags[i], i))) {
						state.story.flags[i] = !state.story.flags[i];
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Dialogue")) {
				ImGui::DragInt("Test Dialog Num", &state.debug.testDialogNum);
				if (ImGui::Button("call undertale(testDialogNum)")) {
					undertale(&state, state.debug.testDialogNum);
				}
				ImGui::Text("num: %d", state.station.dialogue.num);
				ImGui::Text("fullM: %s", state.station.dialogue.fullMessage);
				ImGui::Text("dispM: %s", state.station.dialogue.displayMessage);
				ImGui::Text("chars: %d", state.station.dialogue.chars);
				ImGui::Text("maxChars: %d", state.station.dialogue.maxChars);
				ImGui::Text("timeNextChar: %f",
				            state.station.dialogue.timeToNextChar);
				ImGui::Text("nextDialog: %d",
				            state.station.dialogue.nextDialog);
				ImGui::Text("ready: %d", state.station.dialogue.readyToAdvance);
				ImGui::Text("disengaged: %d",
				            state.station.dialogue.alreadyDisengaged);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Animation/Sounds")) {
				ImGui::Text("walkLeft is playing: %d",
				            IsSoundPlaying(state.sounds.walkLeft));
				ImGui::Text("walkRightis playing: %d",
				            IsSoundPlaying(state.sounds.walkRight));
				ImGui::Text("Current Walk Cycle: %d",
				            state.station.anim.currentCycle);
				ImGui::Text("Protag Sounds Playing: %d",
				            state.sounds.protagDialog.soundsPlaying());
				ImGui::Text("Boss Sounds Playing: %d",
				            state.sounds.bossDialog.soundsPlaying());
				ImGui::Text("Peter Sounds Playing: %d",
				            state.sounds.peterDialog.soundsPlaying());
				ImGui::Text("AI Sounds Playing: %d",
				            state.sounds.aiDialog.soundsPlaying());
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Ship")) {
			if(ImGui::TreeNode("mon")) {
				ImGui::Text("mon rot: %f", state.ship.monster.rot);
				ImGui::Text("mon pos: %.02f, %.02f", EXPAND_VEC2(state.ship.monster.pos));
				ImGui::Text("mon vel: %.02f %.02f", EXPAND_VEC2(state.ship.monster.posVel));
				ImGui::TreePop();
			}
			ImGui::Text("Ship Pos: %.04f %.04f", state.ship.shipPosition.x,
			            state.ship.shipPosition.y);
			ImGui::Text("Ship Vel: %.04f %.04f", state.ship.shipVelocity.x,
			            state.ship.shipVelocity.y);
			ImGui::Text("Ship Rot: (rad/real) %.02f \n\t\t  (deg) %.02f",
			            state.ship.shipRotation,
			            state.ship.shipRotation * RAD2DEG);
			ImGui::Text("Ship RotVel: %.05f", state.ship.shipRotVelo);
			ImGui::Text("Airbreak Charge: %.05f", state.ship.airBreakCharge);
			ImGui::Text("Laser Charge: %.02f", state.ship.laserCharge);
			ImGui::Text("Laser Collide: %f %f", state.debug.laserCollide.x,
			            state.debug.laserCollide.y);
			ImGui::Text("Particle Count: %zu", state.ship.particles.size());
			ImGui::Checkbox("Draw Hitboxes", &state.debug.drawHitboxes);
			ImGui::Checkbox("Inf Airbreak", &infAirBreak);
			ImGui::Checkbox("Inf Laser", &infLaserCharge);
			ImGui::Checkbox("2D mode", &state.debug.ship2dRep);
			if (ImGui::Button("Mine All Asteroids")) {
				state.ship.asteroids.clear();
			}
			ImGui::DragScalarN("Asteroid Hitboxes", ImGuiDataType_Float,
			                   state.debug.asteroidHitboxSizes, 5);
			ImGui::DragFloat("Circle Rad Multi", &state.debug.multiplier);
			ImGui::DragFloat("2d zoom", &state.debug.zoom2d);
			ImGui::DragFloat3("test cube pos", &state.debug.testCubePos.x);
			ImGui::DragFloat3("test cube size", &state.debug.testCubeSize.x);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Transition")) {
			ImGui::Text("active: %d", state.transition.active);
			ImGui::Text("atPeak: %d", state.transition.atPeak);
			ImGui::Text("transitionTime: %f", state.transition.transitionTime);
			ImGui::Text("maxTransitionTime: %f",
			            state.transition.maxTransitionTime);
			ImGui::Text("onPeak: %d", state.transition.onPeak);
			ImGui::Text("toLevel: %d", state.transition.toLevel);
			ImGui::TreePop();
		}
		ImGui::End();
		if (drawImGuiDemo) {
			ImGui::ShowDemoWindow();
		}
		rlImGuiEnd();
#endif
		EndDrawing();
	}

#if DEBUG_MODE
	rlImGuiShutdown();
	UnloadTexture(testTex);
#endif
	UnloadTexture(state.textures.button_e);
	CloseAudioDevice();
	CloseWindow();
}
