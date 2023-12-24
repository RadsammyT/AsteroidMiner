#include <cstdlib>
#include <ctime>
#include <raylib.h>
#include "rayImGui/imgui.h"
#include "rayImGui/rlImGui.h"
#include "game.h"

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
	Rectangle source, dest= {};
	Vector2 origin = {};
	float rotation = 0;
#endif
	GameState state = {};

	state.gameState = STATE_TITLE;
	state.station.playerPosition = {0,0};
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
	state.textures.basePlayer = LoadTexture("resources/web/manBase.png");

	state.textures.testRoom = LoadTexture("resources/web/test_room_16.png");
	state.textures.protagRoom = LoadTexture("resources/web/protag_room_furnished.png");
	state.textures.mainHall = LoadTexture("resources/web/main_hall.png");
	state.textures.cafeteria = LoadTexture("resources/web/cafe_furnished.png");
	state.textures.shipBoarding = LoadTexture("resources/web/ship_boarding_furnished.png");

	state.textures.shipUiArrow = LoadTexture("resources/web/uiArrow.png");
	state.textures.shipAirbreakGauge = LoadTexture("resources/web/uiAirbreak.png");
	state.textures.shipLaserGauge = LoadTexture("resources/web/uiLaser.png");

	Camera2D stationCam = {
		.offset = {(float)GetScreenWidth()/2, (float)GetScreenHeight()/2},
		.target = state.station.playerPosition,
		.rotation = 0.0f,
		.zoom = 5.0f,
	};

	Camera3D shipCam = {
		.position = {5,0,5},
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

	state.models.size5AstBurrowed = LoadModel("resources/web/asteroid5_burrowed.obj");
	state.models.size4AstBurrowed = LoadModel("resources/web/asteroid4_burrowed.obj");
	state.models.size3AstBurrowed = LoadModel("resources/web/asteroid3_burrowed.obj");
	state.models.size2AstBurrowed = LoadModel("resources/web/asteroid2_burrowed.obj");
	state.models.size1AstBurrowed = LoadModel("resources/web/asteroid1_burrowed.obj");

	state.models.station = LoadModel("resources/web/new_station.obj");

	state.sounds.phoneRing = LoadSound("resources/web/phone_low.wav");

	while(!WindowShouldClose()) {
		BeginDrawing();
			if(state.gameState == GAME_STATE::STATE_TITLE) {
				DoTitle(&state);
			}
			if(state.gameState == GAME_STATE::STATE_STATION) {
				DoStation(&state, &stationCam, &shipCam);
			}
			if(state.gameState == GAME_STATE::STATE_SHIP) {
				DoShip(&state, &shipCam);
			}
#if DEBUG_MODE
			if(drawTestTex) {
				BeginMode2D(stationCam);
					DrawTexturePro(testTex,  source, dest, origin, rotation, WHITE);
				EndMode2D();
			}
			if(drawplayerPosPoint) {
				BeginMode2D(stationCam);
					DrawCircleV(state.station.playerPosition, 1.5f, WHITE);
				EndMode2D();
			}
			if(infAirBreak) {
				state.ship.airBreakCharge = 1;
			}
			if(infLaserCharge) {
				state.ship.laserCharge = 1;
			}
			rlImGuiBegin();
				ImGui::Begin("Debug");
					ImGui::Text("FPS: %d", GetFPS());
					ImGui::Text("State: %u", state.gameState);
					ImGui::Checkbox("Show ImGui Demo Window", &drawImGuiDemo);
					if(ImGui::TreeNode("Station")) {
						ImGui::Text("station player pos: %.02f %.02f", 
								state.station.playerPosition.x, state.station.playerPosition.y);
						ImGui::Text("station state: %d", state.station.stationState);
						ImGui::Text("station level: %d", state.station.stationLevel);
						ImGui::Checkbox("draw test tex",&drawTestTex);
						ImGui::Checkbox("draw playerPos point", &drawplayerPosPoint);
						if(drawTestTex) {
							ImGui::DragFloat4("Source", &source.x);
							ImGui::DragFloat4("Dest", &dest.x);
							ImGui::DragFloat2("Origin", &origin.x);
						}
						ImGui::Checkbox("draw test rec", &state.debug.drawTestRec);
						if(state.debug.drawTestRec) {
							ImGui::DragFloat4("testRec", &state.debug.testRec.x, 0.5);
						}
						if(ImGui::TreeNode("Story")) {
							ImGui::DragInt("Day/Story", &state.story.day);
							for(int i = 0; i < 32; i++) {
								if(ImGui::Button(TextFormat("%d/%d##BUTTON_%d", i, state.story.flags[i], i))) {
									state.story.flags[i] = !state.story.flags[i];
								}
							}
							ImGui::TreePop();
						}
						if(ImGui::TreeNode("Dialogue")) {
							ImGui::DragInt("Test Dialog Num", &state.debug.testDialogNum);
							if(ImGui::Button("call undertale(testDialogNum)")) {
								undertale(&state, state.debug.testDialogNum);
							}
							ImGui::Text("num: %d", state.station.dialogue.num);
							ImGui::Text("fullM: %s", state.station.dialogue.fullMessage);
							ImGui::Text("dispM: %s", state.station.dialogue.displayMessage);
							ImGui::Text("chars: %d", state.station.dialogue.chars);
							ImGui::Text("maxChars: %d", state.station.dialogue.maxChars);
							ImGui::Text("timeNextChar: %f", state.station.dialogue.timeToNextChar);
							ImGui::Text("nextDialog: %d", state.station.dialogue.nextDialog);
							ImGui::Text("ready: %d", state.station.dialogue.readyToAdvance);
							ImGui::Text("disengaged: %d", state.station.dialogue.alreadyDisengaged);
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					if(ImGui::TreeNode("Ship")) {
						ImGui::Text("Ship Pos: %.04f %.04f", 
								state.ship.shipPosition.x,
								state.ship.shipPosition.y);
						ImGui::Text("Ship Vel: %.04f %.04f", 
								state.ship.shipVelocity.x,
								state.ship.shipVelocity.y);
						ImGui::Text("Ship Rot: (rad/real) %.02f \n\t\t  (deg) %.02f",
								state.ship.shipRotation,
								state.ship.shipRotation * RAD2DEG);
						ImGui::Text("Ship RotVel: %.05f", state.ship.shipRotVelo);
						ImGui::Text("Airbreak Charge: %.05f", state.ship.airBreakCharge);
						ImGui::Text("Laser Charge: %.02f", state.ship.laserCharge);
						ImGui::Text("Laser Collide: %f %f",
								state.debug.laserCollide.x,
								state.debug.laserCollide.y);
						ImGui::Checkbox("Draw Hitboxes", &state.debug.drawHitboxes);
						ImGui::Checkbox("Inf Airbreak", &infAirBreak);
						ImGui::Checkbox("Inf Laser", &infLaserCharge);
						ImGui::Checkbox("2D mode", &state.debug.ship2dRep);
						if(ImGui::Button("Mine All Asteroids")) {
							state.ship.asteroids.clear();
						}
						ImGui::DragScalarN("Asteroid Hitboxes",
								ImGuiDataType_Float, state.debug.asteroidHitboxSizes, 5);
						ImGui::DragFloat("Circle Rad Multi", &state.debug.multiplier);
						ImGui::DragFloat("2d zoom", &state.debug.zoom2d);
						ImGui::DragFloat3("test cube pos", &state.debug.testCubePos.x);
						ImGui::DragFloat3("test cube size", &state.debug.testCubeSize.x);
						ImGui::TreePop();
					}
					if(ImGui::TreeNode("Transition")) {
						ImGui::Text("active: %b", state.transition.active);
						ImGui::Text("atPeak: %b", state.transition.atPeak);
						ImGui::Text("transitionTime: %f", state.transition.transitionTime);
						ImGui::Text("maxTransitionTime: %f", state.transition.maxTransitionTime);
						ImGui::Text("onPeak: %d", state.transition.onPeak);
						ImGui::Text("toLevel: %d", state.transition.toLevel);
						ImGui::TreePop();
					}
				ImGui::End();
				if(drawImGuiDemo) {
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
