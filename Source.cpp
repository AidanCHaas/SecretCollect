#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <time.h>
#include <string>
using namespace std;

//Constants
const double FPS = 60;
const int SCREEN_HEIGHT = 1500;
const int SCREEN_WIDTH = 1500;
const int SQUARE_SIZE = 40;
const double INSIDE_SIZE = 1327.25;
const double VEL_START = 4;
const int TOP_ALLIGN = 132;
const double RIGHT_ALLIGN = SCREEN_WIDTH - 42.5;
const double LEFT_ALLIGN = 42.5;
const double BOTTOM_ALLIGN = SCREEN_HEIGHT - 42.5;

const int LEVEL1x = 150;
const int LEVEL1y = INSIDE_SIZE / 2 + 100;
const int LEVEL2x = 62;
const int LEVEL2y = 130;
const int LEVEL3x = SCREEN_WIDTH/2-20;
const int LEVEL3y = 180;

enum keys {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER
};

class SpeedBump {
private:
	double xpos1;
	double ypos1;
	double xpos2;
	double ypos2;
	bool collected = false;
public:
	void setPos(double x, double y);
	void draw();
	bool collection(double x1, double y1);
	void reset();
};

class rainbow {
private:
	double xpos1;
	double ypos1;
	double xpos2;
	double ypos2;
	bool plus = true;
public:
	void setPos(double x, double y, double x2, double y2);
	void draw(int r, int g, int b);
	bool collision(double x1, double y1);
	void drawHole();
	void movex(int vel, double left, double right);
	void movey(int vel, double up, double down);
};

class secret {
private:
	double xpos1;
	double ypos1;
	double xpos2;
	double ypos2;
public:
	void setPos(double x, double y);
	void draw();
	bool collection(double x1, double y1);
};

class ExtraLife {
private:
	double xpos1;
	double ypos1;
	double xpos2;
	double ypos2;
	bool collected = false;
public:
	void setPos(double x, double y);
	void draw();
	bool collection(double x1, double y1);
	void reset();
};

int main() {
	//Setup Stuff
	srand(time(NULL));
	al_init();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1);

	//ALLEGRO & Setup Variables
	ALLEGRO_DISPLAY *screen = NULL;
	ALLEGRO_EVENT_QUEUE *events = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = al_load_font("pong.ttf", 50, NULL);
	bool key[5] = { false, false, false, false, false };
	bool redraw = true;
	bool doexit = false;

	//Other Variables
	double playerx1=LEVEL1x;
	double playery1= LEVEL1y;
	double playerx2=playerx1+SQUARE_SIZE;
	double playery2=playery1+SQUARE_SIZE;
	double velocity = VEL_START;
	int level = 0;
	int mans = 3;
	int clock = 60;
	int clockperiod = 60;
	int chill = 300;
	int r;
	int g;
	int b;

	//Class Setup
	//Level 1
	SpeedBump s0;
	s0.setPos(200, 250);
	SpeedBump s1;
	s1.setPos(500, LEVEL1y);
	SpeedBump s2;
	s2.setPos(700, LEVEL1y);
	SpeedBump s3;
	s3.setPos(900, LEVEL1y);
	SpeedBump s4;
	s4.setPos(1100, LEVEL1y);
	rainbow r1;
	r1.setPos(42.5, TOP_ALLIGN, SCREEN_WIDTH-42.5, INSIDE_SIZE/2);
	rainbow r2;
	r2.setPos(42.5, INSIDE_SIZE/2 + 230, SCREEN_WIDTH - 42.5, SCREEN_HEIGHT - 42.5);
	secret w0;
	w0.setPos(1230, 500);
	secret w1;
	w1.setPos(SCREEN_WIDTH-150, INSIDE_SIZE/2+100);
	//Level 2
	SpeedBump s5;
	s5.setPos(60, SCREEN_HEIGHT-102.5);
	SpeedBump s6;
	s6.setPos(SCREEN_WIDTH - 102.5, SCREEN_HEIGHT - 102.5);
	SpeedBump s7;
	s7.setPos(SCREEN_WIDTH - 102.5, 150);
	rainbow r3;
	r3.setPos(120, 200, 1370, 1380);
	rainbow r4;
	r4.setPos(120, TOP_ALLIGN, 160, 200);
	secret w2;
	w2.setPos(180, LEVEL2y+15);
	//Level 3
	ExtraLife e1;
	e1.setPos(100, 200);
	SpeedBump s8;
	s8.setPos(LEVEL3x, LEVEL3y + 250);
	SpeedBump s9;
	s9.setPos(LEVEL3x, SCREEN_HEIGHT-460);
	rainbow r5;
	r5.setPos(250, 500, 1250, 1000);
	rainbow r6;
	r6.setPos(SCREEN_WIDTH / 2 - 90, TOP_ALLIGN, SCREEN_WIDTH / 2 - 50, 400);
	rainbow r7;
	r7.setPos(SCREEN_WIDTH / 2 + 50, TOP_ALLIGN, SCREEN_WIDTH / 2 + 90, 400);
	rainbow r8;
	r8.setPos(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT - 400, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 42.5);
	rainbow r9;
	r9.setPos(SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT - 400, SCREEN_WIDTH / 2 + 90, SCREEN_HEIGHT - 42.5);
	secret w3;
	w3.setPos(LEVEL3x, SCREEN_HEIGHT - 130);

	//Setup 2: Electric Boogalo
	timer = al_create_timer(1.0 / FPS);
	screen = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	events = al_create_event_queue();

	al_register_event_source(events, al_get_display_event_source(screen));
	al_register_event_source(events, al_get_timer_event_source(timer));
	al_register_event_source(events, al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();

	al_start_timer(timer);
	
	while (!doexit) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(events, &ev);
		
		//Physics
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (level > 0) {
				if (key[KEY_LEFT] && playerx1 >= 42.5) {
					playerx1 -= velocity;
				}
				if (key[KEY_RIGHT] && playerx2 <= SCREEN_WIDTH - 42.5) {
					playerx1 += velocity;
				}
				if (key[KEY_UP] && playery1 >= 132.5) {
					playery1 -= velocity;
				}
				if (key[KEY_DOWN] && playery2 <= SCREEN_HEIGHT - 42.5) {
					playery1 += velocity;
				}
			}
			else {
				if (key[KEY_ENTER]) {
					velocity = VEL_START;
					//THERE SHOULD BE A RESET FOR ALL SPEEDBUMPS AND EXTRA LIVES
					s1.reset();
					s2.reset();
					s3.reset();
					s4.reset();
					s5.reset();
					s6.reset();
					s7.reset();
					s8.reset();
					s9.reset();
					e1.reset();
					level = 1;
					mans = 3;
					clockperiod = 120;
					clock = 60;
				}
			}

			playerx2 = playerx1 + SQUARE_SIZE;
			playery2 = playery1 + SQUARE_SIZE;

			//Timer
			if (level > 0) {
				clockperiod--;
				if (clockperiod == 0) {
					clock -= 5;
					cout << clock << endl;
					clockperiod = 120;
				}
			}

			//Levels
			if (level == 1) {
				if (s1.collection(playerx1, playery1) || s2.collection(playerx1, playery1) || s3.collection(playerx1, playery1) || s4.collection(playerx1, playery1)) {
					velocity++;
				}
				if (r1.collision(playerx1, playery1) || r2.collision(playerx1, playery1)) {
					playerx1 = LEVEL1x;
					playery1 = LEVEL1y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
				}
				if (w1.collection(playerx1, playery1)) {
					level = 2;
					clock = 60;
					clockperiod = 60;
					playerx1 = LEVEL2x;
					playery1 = LEVEL2y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
					velocity = VEL_START;

				}
				if (clock == 0) {
					cout << "ded";
					mans--;
					clock = 60;
					playerx1 = LEVEL1x;
					playery1 = LEVEL1y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
					if (mans < 1) {
						level = 0;
					}
				}
			}
			if (level == 2) {
				if (s5.collection(playerx1, playery1) || s6.collection(playerx1, playery1) || s7.collection(playerx1, playery1)) {
					velocity++;
				}
				if (r3.collision(playerx1, playery1) || r4.collision(playerx1, playery1)) {
					playerx1 = LEVEL2x;
					playery1 = LEVEL2y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
				}
				if (clock == 0) {
					cout << "ded";
					mans--;
					clock = 60;
					playerx1 = LEVEL2x;
					playery1 = LEVEL2y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
					if (mans < 1) {
						level = 0;

					}
				}
				if (w2.collection(playerx1, playery1)) {
					level = 3;
					clock = 60;
					clockperiod = 60;
					playerx1 = LEVEL3x;
					playery1 = LEVEL3y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
					velocity = VEL_START;
				}
			}
			if (level == 3) {
				if (clock == 0) {
					cout << "ded";
					mans--;
					clock = 60;
					playerx1 = LEVEL3x;
					playery1 = LEVEL3y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
					if (mans < 1) {
						level = 0;
					}
				}
				if (e1.collection(playerx1, playery1)) {
					mans++;
				}
				if (w3.collection(playerx1, playery1)) {
					level = 4;
				}
				if (s8.collection(playerx1, playery1) || s9.collection(playerx1, playery1)) {
					velocity++;
				}
				if (r5.collision(playerx1, playery1) || r6.collision(playerx1, playery1) || r7.collision(playerx1, playery1) || r8.collision(playerx1, playery1) || r9.collision(playerx1, playery1)) {
					playerx1 = LEVEL3x;
					playery1 = LEVEL3y;
					playerx2 = playerx1 + SQUARE_SIZE;
					playery2 = playery1 + SQUARE_SIZE;
				}
			}
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		//Keyboard IO
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = true;
				break;
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = true;
				break;
			case ALLEGRO_KEY_ENTER:
				key[KEY_ENTER] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = false;
				break;
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;
			case ALLEGRO_KEY_ENTER:
				key[KEY_ENTER] = false;
				break;
			}
		}

		//Rendering
		redraw = false;
		al_clear_to_color(al_map_rgb(0, 0, 0));
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;

		if (level == 0 ) {
			al_draw_text(font, al_map_rgb(255, 255, 255), 150, 400, 0, "PLAY WITH THE ARROW KEYS");
			al_draw_text(font, al_map_rgb(255, 255, 255), 250, 600, 0, "PRESS ENTER TO START");
		}
		if (level > 0 && level < 4) {
			al_draw_filled_rectangle(playerx1, playery1, playerx2, playery2, al_map_rgb(255, 0, 0));
			al_draw_rectangle(30, 120, SCREEN_WIDTH - 30, SCREEN_HEIGHT - 30, al_map_rgb(0, 200, 0), 25);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 30, 0, "LEVEL %d", level);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 1050, 30, 0, "MANS: %d", mans);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 675, 30, 0, "%d", clock);

		}
		//Classes
		if (level == 1) {
			s1.draw();
			s2.draw();
			s3.draw();
			s4.draw();
			r1.draw(r, g, b);
			r1.drawHole();
			r2.draw(r, g, b);
			r2.drawHole();
			w1.draw();
			al_draw_text(font, al_map_rgb(255, 255, 255), 220, 500, 0, "COLLECT THE SECRET");
			al_draw_text(font, al_map_rgb(255, 255, 255), 350, 250, 0, "MAKES YOU GO FASTER");
			w0.draw();
			s0.draw();
		}
		if (level == 2) {
			s5.draw();
			s6.draw();
			s7.draw();
			r3.draw(r, g, b);
			r4.draw(r, g, b);
			r3.drawHole();
			w2.draw();
		}
		if (level == 3) {
			e1.draw();
			r5.draw(r, g, b);
			r5.drawHole();
			r6.draw(r, g, b);
			r7.draw(r, g, b);
			r8.draw(r, g, b);
			r9.draw(r, g, b);
			s8.draw();
			s9.draw();
			al_draw_text(font, al_map_rgb(255, 255, 255), 450, 620, 0, "CAN YOU FIND");
			al_draw_text(font, al_map_rgb(255, 255, 255), 500, 720, 0, "THE HIDDEN");
			al_draw_text(font, al_map_rgb(255, 255, 255), 450, 820, 0, "EXTRA LIVES?");
			w3.draw();
		}
		if (level == 4) {
			al_draw_text(font, al_map_rgb(255, 255, 255), 550, 620, 0, "YOU WIN!");
			chill--;
			if (chill == 0) {
				doexit = true;
			}
		}
		//Fleep
		al_flip_display();
	}
	//Ending Program
	al_destroy_timer(timer);
	al_destroy_display(screen);
	al_destroy_event_queue(events);

	return 0;
}

void SpeedBump::setPos(double x, double y) {
	xpos1 = x;
	ypos1 = y;
	xpos2 = x + SQUARE_SIZE;
	ypos2 = y + SQUARE_SIZE;
}

void SpeedBump::draw() {
	if (!collected) {
		al_draw_filled_rectangle(xpos1, ypos1, xpos2, ypos2, al_map_rgb(0, 0, 255));
	}
}

bool SpeedBump::collection(double x1, double y1) {
	if (((x1 >= xpos1 && x1 <= xpos2)||(x1+SQUARE_SIZE >= xpos1 && x1+SQUARE_SIZE <= xpos2)) && ((y1 >= ypos1 && y1 <= ypos2)||(y1+SQUARE_SIZE >= ypos1 && y1+SQUARE_SIZE <= ypos2)) && !collected) {
		collected = true;
		return true;
	}
	else {
		return false;
	}
}

void SpeedBump::reset() {
	collected = false;
}

void rainbow::setPos(double x, double y, double x2, double y2) {
	xpos1 = x;
	ypos1 = y;
	xpos2 = x2;
	ypos2 = y2;
}

void rainbow::draw(int r, int g, int b) {
	al_draw_filled_rectangle(xpos1, ypos1, xpos2, ypos2, al_map_rgb(r, g, b));
}

bool rainbow::collision(double x1, double y1) {
	if (((x1 >= xpos1 && x1 <= xpos2) || (x1 + SQUARE_SIZE >= xpos1 && x1 + SQUARE_SIZE <= xpos2)) && ((y1 >= ypos1 && y1 <= ypos2) || (y1 + SQUARE_SIZE >= ypos1 && y1 + SQUARE_SIZE <= ypos2))) {
		cout << "bruh moment" << endl;
		return true;
	}
	else {
		return false;
	}
}

void rainbow::drawHole() {
	al_draw_filled_rectangle(xpos1 + 75, ypos1 + SQUARE_SIZE, xpos2 - 75, ypos2 - SQUARE_SIZE, al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(xpos1 + SQUARE_SIZE, ypos1 + 75, xpos2 - SQUARE_SIZE, ypos2 - 75, al_map_rgb(0, 0, 0));
}

void rainbow::movex(int vel, double left, double right) {
	if (xpos1 <= left || xpos2 >= right) {
		plus = !plus;
	}
	if (plus) {
		xpos1 += vel;
		xpos2 += vel;
	}
	else {
		xpos1 -= vel;
		xpos2 -= vel;
	}
}

void rainbow::movey(int vel, double up, double down) {
	if (ypos1 <= up || ypos2 >= down) {
		plus = !plus;
	}
	if (plus) {
		ypos1 += vel;
		ypos2 += vel;
	}
	else {
		ypos1 -= vel;
		ypos2 -= vel;
	}
}

void secret::setPos(double x, double y) {
	xpos1 = x;
	ypos1 = y;
	xpos2 = x + SQUARE_SIZE;
	ypos2 = y + SQUARE_SIZE;
}

void secret::draw() {
	al_draw_filled_rectangle(xpos1, ypos1, xpos2, ypos2, al_map_rgb(255, 255, 0));
}

bool secret::collection(double x1, double y1) {
	if (((x1 >= xpos1 && x1 <= xpos2) || (x1 + SQUARE_SIZE >= xpos1 && x1 + SQUARE_SIZE <= xpos2)) && ((y1 >= ypos1 && y1 <= ypos2) || (y1 + SQUARE_SIZE >= ypos1 && y1 + SQUARE_SIZE <= ypos2))) {
		cout << "gamer moment" << endl;
		return true;
	}
	else {
		return false;
	}
}

void ExtraLife::setPos(double x, double y) {
	xpos1 = x;
	ypos1 = y;
	xpos2 = x + SQUARE_SIZE;
	ypos2 = y + SQUARE_SIZE;
}

void ExtraLife::draw() {
	if (!collected) {
		al_draw_filled_rectangle(xpos1, ypos1, xpos2, ypos2, al_map_rgb(10, 10, 10));
	}
}

bool ExtraLife::collection(double x1, double y1) {
	if (((x1 >= xpos1 && x1 <= xpos2) || (x1 + SQUARE_SIZE >= xpos1 && x1 + SQUARE_SIZE <= xpos2)) && ((y1 >= ypos1 && y1 <= ypos2) || (y1 + SQUARE_SIZE >= ypos1 && y1 + SQUARE_SIZE <= ypos2)) && !collected) {
		collected = true;
		return true;
	}
	else {
		return false;
	}
}

void ExtraLife::reset() {
	collected = false;
}