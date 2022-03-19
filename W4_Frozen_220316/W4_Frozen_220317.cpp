#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES	1

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <bangtal.h>
#include <string.h>
#include <string>

time_t time_st, time_ed;
int time_d;
string time_s;
const char* time_f;

SceneID room1;
ObjectID restart ,start, puzzle[16], initObjects[16], gameObjects[16];
int blank = 15;
bool game = false;
int mixing = 0;
const int mixCount = 200;
TimerID timerMixing, timercount;
const Second animationTime = 0.05f;

struct POS {
	int x, y;
};
const POS positions[16] = {
	{0, 450}, {150, 450}, {300, 450}, {450, 450},
	{0, 300}, {150, 300}, {300, 300}, { 450, 300},
	{0, 150}, {150, 150}, {300, 150}, {450, 150},
	{0, 00}, {150, 00}, {300, 00}, {450, 00},
};

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown) {
	//물체 생성하기
	ObjectID object = createObject(image);
	//위치보이기
	locateObject(object, scene, x, y);
	//보이기
	if (shown == true) {
		showObject(object);
	}
	return object;
}



void Gamesetting() {
	room1 = createScene("틀린 그림 찾기", "images/600x600.png");

	char image[20];
	for (int i = 0; i < 16; i++) {
		sprintf(image, "images/%d.png", i + 1);
		initObjects[i] = createObject(image, room1, positions[i].x,positions[i].y, true);
		gameObjects[i] = initObjects[i];
		locateObject(gameObjects[i], room1, positions[i].x, positions[i].y);
		showObject(gameObjects[i]);
	}
	//start = createObject("images/start.png", room1, 1100, 20, true);
	blank = 15;
	srand((unsigned int)time(NULL));
	game = false;
	timerMixing = createTimer();
	
}

//이거 무슨 뜻임?
int game_index(ObjectID oid) {
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] == oid) return i;
	}

	return -1;
}

bool game_move(int index)
{
	if (index < 0) return false;
	if (index > 15) return false;

	if (index == (blank - 4) || index == (blank + 4) ||
		(index == (blank - 1) && (blank % 4) != 0) ||
		(index == (blank + 1) && (blank % 4) != 3)) {

		locateObject(gameObjects[index], room1, positions[blank].x, positions[blank].y);
		locateObject(gameObjects[blank], room1, positions[index].x, positions[index].y);

		ObjectID o = gameObjects[blank];
		gameObjects[blank] = gameObjects[index];
		gameObjects[index] = o;

		blank = index;

		return true;
	}

	return false;
}

void game_random_move()
{
	bool possible = false;
	int next = -1;
	while (!possible) {
		switch (rand() % 4) {
		case 0:		// UP
			next = blank - 4;
			break;
		case 1:		// DOWN
			next = blank + 4;
			break;
		case 2:		// LEFT
			next = (blank % 4 != 0) ? (blank - 1) : -1;
			break;
		case 3:		// RIGHT
			next = (blank % 4 != 3) ? (blank + 1) : -1;
			break;
		}

		possible = game_move(next);
	}
}

void game_prepare()
{
	hideObject(gameObjects[blank]);

	mixing = mixCount;

	// start to mix
    setTimer(timerMixing, animationTime);
	startTimer(timerMixing);
}

bool game_check()
{
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] != initObjects[i]) return false;
	}

	return true;
}

void game_end()
{
	game = false;
	time_ed = time(NULL);
	int time_d = (time_ed - time_st);
	time_s = std::to_string(time_d);
	time_f = time_s.c_str();
	showMessage(time_f);

	hideObject(restart);
	showObject(gameObjects[blank]);
	showObject(start);
}



void timerCallback(TimerID timer)
{
	game_random_move();

	if (mixing > 0) {
		--mixing;

		setTimer(timerMixing, animationTime);
		startTimer(timerMixing);
	}

	else {
		showObject(restart);
		hideObject(start);
		game = true;
		time_st = time(NULL);
	}
}
/*
void gamerandom() {
	int i;
	for (int i = 0; i< 16; i++){
		int j = rand() % 16;

		locateObject(gameObjects[j], room1, positions[i].x, positions[i].y);

	}
}
*/
void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (game) {
		game_move(game_index(object));

		if (game_check()) {
			game_end();
		}
	}
	if (mixing == 0 && object == start) {
		game_prepare();
	}
	else if(object == restart) {
		game_prepare();

	}

}
int main() {

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);

	Gamesetting();
	start = createObject("images/start.png", room1, 300, 100, true);
	restart = createObject("images/restart.png", room1, 500, 600, false);
	startGame(room1);
}

