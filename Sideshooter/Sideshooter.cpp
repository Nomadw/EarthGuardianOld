#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_audio.h>
#include <allegro5\allegro_acodec.h>
#include "objects.h"
 
//GLOBALS==============================
const int WIDTH = 1024;
const int HEIGHT = 768;
const int NUM_BULLETS = 999;
const int NUM_COMETS = 30;
const int NUM_EXPLOSIONS = 100;
enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
enum GAMESTATE{STATE_TITLE, STATE_GAME, STATE_OPTIONS}; //Game state numbers
bool gamestate[3] = {false, false, false};
int sound = 1;

ALLEGRO_SAMPLE *gameovermusic = NULL;
ALLEGRO_SAMPLE *titlescreenmusic = NULL;
ALLEGRO_SAMPLE *optionscreenmusic = NULL;
ALLEGRO_SAMPLE *explosionsound = NULL;
ALLEGRO_SAMPLE *playerexplode = NULL;
 
//prototypes
void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image);
void ResetShipAnimation(SpaceShip &ship, int position);
void DrawShip(SpaceShip &ship);
void MoveShipUp(SpaceShip &ship);
void MoveShipDown(SpaceShip &ship);
void MoveShipLeft(SpaceShip &ship);
void MoveShipRight(SpaceShip &ship);
 
void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip &ship, int yOffset);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int bsize, Comet comets[], int csize, SpaceShip &ship, Explosion explosions[]);
 
void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *image);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size);
void CollideComet(Comet comets[], int csize, SpaceShip &ship, Explosion explosions[]);

void InitExplosion(Explosion explosions[], int size, ALLEGRO_BITMAP *image);
void CreateExplosion(Explosion explosions[], int size, int xpx, int xpy);
void DrawExplosion(Explosion explosions[], int size);
 
int main(void)
{
        //primitive variable
        bool done = false;
        bool redraw = true;
        const int FPS = 60;
        bool isGameOver = false;

		//game initialisation variables
		bool gameinit = false;
		int titleinit = 0;
		int optioninit = 0;
		int gameoverinit = 0;

		int gamemusic = 1;
		

		int titleMenuOption = 1;
		int OptionMenuOption = 1;
 
        //object variables
        SpaceShip ship;
        Bullet bullets[NUM_BULLETS];
        Comet comets[NUM_COMETS];
		Explosion explosions[NUM_EXPLOSIONS];
 
        //bullet auto-fire delay.
        int bulletDelay = 0;
        int BULLET_DELAY_MAX = 8;
 
        //Allegro variables
        ALLEGRO_DISPLAY *display = NULL;

        ALLEGRO_EVENT_QUEUE *event_queue = NULL;

        ALLEGRO_TIMER *timer = NULL;

        ALLEGRO_FONT *font32 = NULL;
		ALLEGRO_FONT *font18 = NULL;

		ALLEGRO_BITMAP *titleBackground;
        ALLEGRO_BITMAP *shipImage;
		ALLEGRO_BITMAP *gameBackground;
		ALLEGRO_BITMAP *asteroid;
		ALLEGRO_BITMAP *explosion;
		ALLEGRO_BITMAP *optionsBackground;

		ALLEGRO_SAMPLE *levelmusic = NULL;
		ALLEGRO_SAMPLE *levelmusic2 = NULL;
		ALLEGRO_SAMPLE *levelmusic3 = NULL;
		ALLEGRO_SAMPLE_ID music1;
		ALLEGRO_SAMPLE_ID music2;
		ALLEGRO_SAMPLE *gunsound = NULL;
 
        //Initialization Functions
        if(!al_init())                                                                          //initialize Allegro
                return -1;
 
        display = al_create_display(WIDTH, HEIGHT);                     //create our display object
 
        if(!display)                                                                            //test display object
                return -1;
 
        al_init_primitives_addon();
        al_install_keyboard();
        al_init_font_addon();
        al_init_ttf_addon();
 
        al_init_image_addon();

		al_install_audio();
		al_init_acodec_addon();

		al_reserve_samples(99); //number of sound samples to reserve
 
        event_queue = al_create_event_queue();
        timer = al_create_timer(1.0 / FPS);
 
        shipImage = al_load_bitmap("Spaceship_sprites_by_arboris.png");
        al_convert_mask_to_alpha(shipImage, al_map_rgb(255,0,255));

		titleBackground = al_load_bitmap("free___sky_background_image_1_by_rin_shiba-d4t8y992.png");

		optionsBackground = al_load_bitmap("metal_background_scratches_surface_18408_1920x1200.png");

		gameBackground = al_load_bitmap("Space_Nuclear_war_view_from_the_Earth_s_orbit_018170_.png");

		//asteroid = al_load_bitmap("asteroid-1-96.png");
		asteroid = al_load_bitmap("asteroid-1-962.png");

		explosion = al_load_bitmap("Explosion1spritesheet.png");
 
        srand(time(NULL));
        //InitShip(ship, shipImage);
        //InitBullet(bullets, NUM_BULLETS);
        //InitComet(comets, NUM_COMETS, asteroid);
		//InitExplosion(explosions, NUM_EXPLOSIONS, explosion);
       
        font32 = al_load_font("arial.ttf", 32, 0);
		font18 = al_load_font("arial.ttf", 18, 0);
 
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
        al_register_event_source(event_queue, al_get_display_event_source(display));

		//load in game music
		gameovermusic = al_load_sample("gameover.ogg");
		levelmusic = al_load_sample("stage1111.ogg");
		levelmusic2 = al_load_sample("music2.ogg");
		levelmusic3 = al_load_sample("music3.ogg");
		titlescreenmusic = al_load_sample("titlemusic.ogg");
		optionscreenmusic = al_load_sample("optionsmusic.ogg");


		gunsound = al_load_sample("gun.ogg");

		explosionsound = al_load_sample("explosion.ogg");
		playerexplode = al_load_sample("playerexplosion.ogg");

		gamestate[STATE_TITLE] = 1;
        al_start_timer(timer);
        while(!done)
        {
				
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);
				if(gamestate[STATE_TITLE] == 1) //TITLE SCREEN==================================
				{
					gameinit = false;
					gameoverinit = 0;
					isGameOver = false;
					optioninit = 0;

					if(titleinit == 0)
					{
						keys[SPACE] = false;
						titleinit = 1;
						titleMenuOption = 1;
						al_play_sample(titlescreenmusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &music2);
					}


					al_draw_bitmap(titleBackground, 0, 0, 0);
					if(titleMenuOption == 1)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Start Game");
					else al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Start Game");

					if(titleMenuOption == 2)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "Game Options");
					else al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "Game Options");

					if(titleMenuOption == 3)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "Exit Game");
					else al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "Exit Game");

				if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
					{
					 switch(ev.keyboard.keycode)
						{
						 case ALLEGRO_KEY_ESCAPE:
							done = true;
							break;
						case ALLEGRO_KEY_UP:
 
                /* CODE COPY-PASTED TO HERE */
							if(titleMenuOption == 1)
							 {                                                    
								titleMenuOption = 1;
								al_rest(0.1);
									}
									else if(titleMenuOption == 2)
									{
											titleMenuOption = 1;
											al_rest(0.1);
									}
									else if(titleMenuOption == 3)
									{
											titleMenuOption = 2;
											al_rest(0.1);
									}
									/* END COPY PASTE */
 
									keys[UP] = true;
									break;
							case ALLEGRO_KEY_DOWN:
 
									/*  CODE COPY AND PASTED TO HERE */
									if(titleMenuOption == 1)
									{                                                    
											titleMenuOption = 2;
											al_rest(0.1);
									}
									else if(titleMenuOption == 2)
									{
											titleMenuOption = 3;
											al_rest(0.1);
									}
									else if(titleMenuOption == 3)
									{
											titleMenuOption = 3;
											al_rest(0.1);
									}
									/* END COPY PASTE*/
 
									keys[DOWN] = true;
									break;
							case ALLEGRO_KEY_LEFT:
									keys[LEFT] = true;
									break;
							case ALLEGRO_KEY_RIGHT:
									keys[RIGHT] = true;
									break;
							case ALLEGRO_KEY_SPACE:
									keys[SPACE] = true;
									break;
							}
					}
					if(ev.type == ALLEGRO_EVENT_KEY_UP)
					{
							switch(ev.keyboard.keycode)
							{
							case ALLEGRO_KEY_UP:
									keys[UP] = false;
									break;
							case ALLEGRO_KEY_DOWN:
									keys[DOWN] = false;
									break;
							case ALLEGRO_KEY_LEFT:
									keys[LEFT] = false;
									break;
							case ALLEGRO_KEY_RIGHT:
									keys[RIGHT] = false;
									break;
							case ALLEGRO_KEY_SPACE:
									keys[SPACE] = false;
									break;
							}
					}
                                       
 
					if(keys[SPACE])
					{
							if(titleMenuOption == 1)
							{
									gamestate[STATE_TITLE] = 0;
									gamestate[STATE_GAME] = 1;
									gamestate[STATE_OPTIONS] = 0;
									al_stop_sample(&music2);
									al_rest(1);
							}
							else if(titleMenuOption == 2)
							{
									gamestate[STATE_TITLE] = 0;
									gamestate[STATE_GAME] = 0;
									gamestate[STATE_OPTIONS] = 1;
									al_stop_sample(&music2);
									al_rest(1);
							}
							else if(titleMenuOption == 3)
							{
									done = true;
							}
					}

				if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                        done = true;
                }
 
					al_flip_display();
                    al_clear_to_color(al_map_rgb(0,0,0));
				}
				

				if(gamestate[STATE_OPTIONS] == 1) //OPTIONS SCREEN==================================
				{
					gameinit = false;
					gameoverinit = 0;
					isGameOver = false;
					titleinit = 0;

					if(optioninit == 0)
					{
						keys[SPACE] = false;
						optioninit = 1;
						OptionMenuOption = 1;
						al_play_sample(optionscreenmusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &music2);
					}


					al_draw_bitmap(optionsBackground, 0, 0, 0);

					al_draw_textf(font32, al_map_rgb(0,255,0), WIDTH / 2, (HEIGHT / 2) - 340, ALLEGRO_ALIGN_CENTER, "Options");

					if(OptionMenuOption == 1) {
						if(gamemusic == 1)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "< Game Music: Rising Blue Lightning >");
						if(gamemusic == 2)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "< Game Music: Cyber Cave >");
						if(gamemusic == 3)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "< Game Music: Steel of Destiny >");
						if(gamemusic == 4)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "< Game Music: None >");
					} else {
						if(gamemusic == 1)
					al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Game Music: Rising Blue Lightning");
						if(gamemusic == 2)
					al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Game Music: Cyber Cave");
						if(gamemusic == 3)
					al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Game Music: Steel of Destiny");
						if(gamemusic == 4)
					al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 70, ALLEGRO_ALIGN_CENTER, "Game Music: None");
					}

					if(OptionMenuOption == 2) {
						if(sound == 1)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "< Sound effects: On >");
						if(sound == 0)
						al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "< Sound effects: Off >");
					} else {
						if(sound == 1)
							al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "Sound effects: On");
						if(sound == 0)
							al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) - 30, ALLEGRO_ALIGN_CENTER, "Sound effects: Off");
					}

					if(OptionMenuOption == 3)
					al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH / 2, (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "Return to Title");
					else al_draw_textf(font32, al_map_rgb(0,0,255), WIDTH / 2, (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "Return to Title");

						if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
						{
								switch(ev.keyboard.keycode)
								{
								case ALLEGRO_KEY_ESCAPE:
										done = true;
										break;
								case ALLEGRO_KEY_UP:
 
										
										if(OptionMenuOption == 1)
										{                                                    
												OptionMenuOption = 1;
												al_rest(0.1);
										}
										else if(OptionMenuOption == 2)
										{
												OptionMenuOption = 1;
												al_rest(0.1);
										}
										else if(OptionMenuOption == 3)
										{
												OptionMenuOption = 2;
												al_rest(0.1);
										}
										
 
										keys[UP] = true;
										break;
								case ALLEGRO_KEY_DOWN:
 
										
										if(OptionMenuOption == 1)
										{                                                    
												OptionMenuOption = 2;
												al_rest(0.1);
										}
										else if(OptionMenuOption == 2)
										{
												OptionMenuOption = 3;
												al_rest(0.1);
										}
										else if(OptionMenuOption == 3)
										{
												OptionMenuOption = 3;
												al_rest(0.1);
										}
										
 
										keys[DOWN] = true;
										break;
								case ALLEGRO_KEY_LEFT:

										if(OptionMenuOption == 1)
										{
											gamemusic--;
											if(gamemusic < 1)
												gamemusic = 4;
										}

										if(OptionMenuOption == 2)
										{
											if(sound == 1)
												sound = 0;
											else sound = 1;
										}

										keys[LEFT] = true;
										break;
								case ALLEGRO_KEY_RIGHT:

										if(OptionMenuOption == 1)
										{
											gamemusic++;
											if(gamemusic > 4)
												gamemusic = 1;
										}

										if(OptionMenuOption == 2)
										{
											if(sound == 1)
												sound = 0;
											else sound = 1;
										}
										keys[RIGHT] = true;
										break;
								case ALLEGRO_KEY_SPACE:
										keys[SPACE] = true;
										break;
								}
						}
						if(ev.type == ALLEGRO_EVENT_KEY_UP)
						{
								switch(ev.keyboard.keycode)
								{
								case ALLEGRO_KEY_UP:
										keys[UP] = false;
										break;
								case ALLEGRO_KEY_DOWN:
										keys[DOWN] = false;
										break;
								case ALLEGRO_KEY_LEFT:
										keys[LEFT] = false;
										break;
								case ALLEGRO_KEY_RIGHT:
										keys[RIGHT] = false;
										break;
								case ALLEGRO_KEY_SPACE:
										keys[SPACE] = false;
										break;
								}
						}

					if(keys[SPACE]) {
						if(OptionMenuOption == 3) {
						gamestate[STATE_TITLE] = 1;
						gamestate[STATE_GAME] = 0;
						gamestate[STATE_OPTIONS] = 0;
						al_stop_sample(&music2);
						al_rest(1);
						}
					}

				if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                        done = true;
                }
 
					al_flip_display();
                    al_clear_to_color(al_map_rgb(0,0,0));
				} 
				if(gamestate[STATE_GAME] == 1)         //MAIN GAME=============================
				{
						if(gameinit == false) {
							al_stop_sample(&music2);
							BULLET_DELAY_MAX = 8;
							gameinit = true;
							keys[SPACE] = false;
							titleinit = false;
							InitShip(ship, shipImage);
							InitBullet(bullets, NUM_BULLETS);
							InitComet(comets, NUM_COMETS, asteroid);
							InitExplosion(explosions, NUM_EXPLOSIONS, explosion);
							if(gamemusic == 1)
							al_play_sample(levelmusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &music1); //MUSIC
							if(gamemusic == 2)
							al_play_sample(levelmusic2, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &music1);
							if(gamemusic == 3)
							al_play_sample(levelmusic3, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &music1);
						}

                if(ev.type == ALLEGRO_EVENT_TIMER)
                {
                        redraw = true;
                        if(keys[UP])
                                MoveShipUp(ship);
                        else if(keys[DOWN])
                                MoveShipDown(ship);
                        else
                                ResetShipAnimation(ship, 1);
 
                        if(keys[LEFT])
                                MoveShipLeft(ship);
                        else if(keys[RIGHT])
                                MoveShipRight(ship);
                        else
                                ResetShipAnimation(ship, 2);
 
 
                        if(bulletDelay > 0)
                                --bulletDelay;
                       
                        if((keys[SPACE]) && (!bulletDelay))
                        {
								if(ship.score < 75)
								{
								 bulletDelay = BULLET_DELAY_MAX;
									FireBullet(bullets, NUM_BULLETS, ship, 10);
									FireBullet(bullets, NUM_BULLETS, ship, -10);
								}
								if(ship.score >=75)
								{
									bulletDelay = BULLET_DELAY_MAX;
									FireBullet(bullets, NUM_BULLETS, ship, 10);
									FireBullet(bullets, NUM_BULLETS, ship, 0);
									FireBullet(bullets, NUM_BULLETS, ship, -10);
								}

								if(sound == 1)
								al_play_sample(gunsound, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,NULL);
                        }
 
                        if(!isGameOver)
                        {
                                UpdateBullet(bullets, NUM_BULLETS);
                                StartComet(comets, NUM_COMETS);
                                UpdateComet(comets, NUM_COMETS);
                                CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, ship, explosions);
                                CollideComet(comets, NUM_COMETS, ship, explosions);
 
                                if(ship.lives <= 0)
                                        isGameOver = true;

								
                        }
                }
                else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                        done = true;
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
                {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:
                                done = true;
                                break;
                        case ALLEGRO_KEY_UP:
                                keys[UP] = true;
                                break;
                        case ALLEGRO_KEY_DOWN:
                                keys[DOWN] = true;
                                break;
                        case ALLEGRO_KEY_LEFT:
                                keys[LEFT] = true;
                                break;
                        case ALLEGRO_KEY_RIGHT:
                                keys[RIGHT] = true;
                                break;
                        case ALLEGRO_KEY_SPACE:
                                keys[SPACE] = true;
                                break;
                        }
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_UP)
                {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:
                                done = true;
                                break;
                        case ALLEGRO_KEY_UP:
                                keys[UP] = false;
                                break;
                        case ALLEGRO_KEY_DOWN:
                                keys[DOWN] = false;
                                break;
                        case ALLEGRO_KEY_LEFT:
                                keys[LEFT] = false;
                                break;
                        case ALLEGRO_KEY_RIGHT:
                                keys[RIGHT] = false;
                                break;
                        case ALLEGRO_KEY_SPACE:
                                keys[SPACE] = false;
                                break;
                        }
                }
 
                if(redraw && al_is_event_queue_empty(event_queue))
                {
                        redraw = false;
 
                        if(!isGameOver)
                        {
							gameoverinit = 0;
								if(ship.score >= 50)
									BULLET_DELAY_MAX = 4;
								if(keys[UP] == false && keys[DOWN] == false)
								{
									ship.y += 1;
									ship.x += 1;
									if(ship.y > HEIGHT)
										ship.y = HEIGHT;
									if(ship.x > WIDTH)
										ship.x = WIDTH;
								}
								al_draw_bitmap(gameBackground, 0, 0, 0);
                                DrawShip(ship);
                                DrawBullet(bullets, NUM_BULLETS);
                                DrawComet(comets, NUM_COMETS);
								DrawExplosion(explosions, NUM_EXPLOSIONS);

								if(ship.lives >= 3)
                                al_draw_textf(font32, al_map_rgb(0,255,0), 5, 5, 0, "Lives: %i", ship.lives);

								if(ship.lives == 2)
								al_draw_textf(font32, al_map_rgb(255,186,0), 5, 5, 0, "Lives: %i", ship.lives);

								if(ship.lives == 1)
								al_draw_textf(font32, al_map_rgb(255,0,0), 5, 5, 0, "Lives: %i", ship.lives);

                                al_draw_textf(font32, al_map_rgb(255,0,0), HEIGHT - 5, 5, 0,"Score: %i", ship.score);

								if(ship.score < 50)
									al_draw_textf(font18, al_map_rgb(255,0,0), HEIGHT - 5, 35, 0,"Next upgrade at 50 points");
								
								if(ship.score >= 50 && ship.score < 75)
									al_draw_textf(font18, al_map_rgb(255,0,0), HEIGHT - 5, 35, 0,"Next upgrade at 75 points");

								if(ship.score >= 75)
									al_draw_textf(font18, al_map_rgb(255,0,0), HEIGHT - 5, 35, 0,"Free life at every 100 points");
								
                        }
                        else        //GAME OVER SCREEN==========================
                        {
							if(gameoverinit == 0) {
								al_play_sample(gameovermusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &music2);
								gameoverinit = 1;
								keys[SPACE] = false;
							}

							   if(gamemusic == 1 || gamemusic == 2 || gamemusic == 3)
								al_stop_sample(&music1);
								//al_play_sample(gameovermusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &music2);
                                al_draw_textf(font32, al_map_rgb(255,0,0), WIDTH /  2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Game over. Final score, %i.", ship.score);
								al_draw_textf(font18, al_map_rgb(255,0,0), WIDTH /  2, (HEIGHT / 2) + 70, ALLEGRO_ALIGN_CENTER, "Hit fire to return to menu");

								if(keys[SPACE] == true)
								{
									al_rest(1);
									al_stop_sample(&music2);
									gamestate[STATE_TITLE] = 1;
									gamestate[STATE_GAME] = 0;
									gamestate[STATE_OPTIONS] = 0;
								}
                        }
                       
 
               
                        al_flip_display();
                        al_clear_to_color(al_map_rgb(0,0,0));
                }
				}
				
        }
 
       al_destroy_event_queue(event_queue);
       al_destroy_timer(timer);
       al_destroy_display(display);                                            //destroy our display object

        al_destroy_bitmap(shipImage);
		al_destroy_bitmap(gameBackground);
		al_destroy_bitmap(titleBackground);
		al_destroy_bitmap(optionsBackground);
		al_destroy_bitmap(asteroid);
		al_destroy_bitmap(explosion);

       al_destroy_font(font32);
		al_destroy_font(font18);

		al_destroy_sample(levelmusic);
		al_destroy_sample(levelmusic2);
		al_destroy_sample(levelmusic3);
		al_destroy_sample(gameovermusic);
		al_destroy_sample(optionscreenmusic);
		al_destroy_sample(titlescreenmusic);
		al_destroy_sample(gunsound);
		al_destroy_sample(explosionsound);
		al_destroy_sample(playerexplode);
 
        return 0;
}
 
void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image)
{
        ship.x = 20;
        ship.y = HEIGHT / 2;
        ship.ID = PLAYER;
        ship.lives = 3;
        ship.speed = 8;
        ship.boundx = 10;
        ship.boundy = 12;
        ship.score = 0;
 
        ship.maxFrame = 3;
        ship.curFrame = 0;
        ship.frameCount = 0;
        ship.frameDelay = 50;
        ship.frameWidth = 46;
        ship.frameHeight = 41;
        ship.animationColumns = 3;
        ship.animationDirection = 1;
 
        ship.animationRow = 1;
 
        ship.image = image;
 
}
 
void ResetShipAnimation(SpaceShip &ship, int position)
{
        if(position == 1)
                ship.animationRow = 1;
        else
                ship.curFrame = 0;
}
 
void DrawShip(SpaceShip &ship)
{
        int fx = (ship.curFrame % ship.animationColumns) * ship.frameWidth;
        int fy = ship.animationRow * ship.frameHeight;
 
        al_draw_bitmap_region(ship.image, fx, fy, ship.frameWidth, ship.frameHeight, ship.x - ship.frameWidth / 2,
                                                        ship.y - ship.frameHeight / 2, 0);
}
void MoveShipUp(SpaceShip &ship)
{
		ship.speed = 6;
        ship.animationRow = 0;
        ship.y -= ship.speed;
        if(ship.y < 0)
                ship.y = 0;
}
void MoveShipDown(SpaceShip &ship)
{
		ship.speed = 8;
        ship.animationRow = 2;
        ship.y += ship.speed;
        if(ship.y > HEIGHT)
                ship.y = HEIGHT;
}
void MoveShipLeft(SpaceShip &ship)
{
		ship.speed = 6;
        ship.curFrame = 2;
        ship.x -= ship.speed;
        if(ship.x < 0)
                ship.x = 0;
}
void MoveShipRight(SpaceShip &ship)
{
		ship.speed = 8;
        ship.curFrame = 1;
        ship.x += ship.speed;
        if(ship.x > WIDTH)
                ship.x = WIDTH;
}
 
void InitBullet(Bullet bullet[], int size)
{
        for(int i = 0; i < size; i++)
        {
                bullet[i].ID = BULLET;
                bullet[i].speed = 20;
                bullet[i].live = false;
        }
}
void DrawBullet(Bullet bullet[], int size)
{
        for( int i = 0; i < size; i++)
        {
                if(bullet[i].live)
                        al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(0, 255, 255));
        }
}
void FireBullet(Bullet bullet[], int size, SpaceShip &ship, int yOffset)
{
        for( int i = 0; i < size; i++)
        {
                if(!bullet[i].live)
                {
                        bullet[i].x = ship.x + 17;
                        bullet[i].y = ship.y + yOffset;
                        bullet[i].live = true;
                        break;
                }
        }
}
void UpdateBullet(Bullet bullet[], int size)
{
        for(int i = 0; i < size; i++)
        {
                if(bullet[i].live)
                {
                        bullet[i].x += bullet[i].speed;
                        if(bullet[i].x > WIDTH)
                                bullet[i].live = false;
                }
        }
}
 
void CollideBullet(Bullet bullet[], int bsize, Comet comets[], int csize, SpaceShip &ship, Explosion explosions[])
{
        for (int i = 0; i < bsize; i++)
        {
                if(bullet[i].live)
                {
                        for(int j = 0; j < csize; j++)
                        {
								
                                if(comets[j].live)
                                {
                                        if(bullet[i].x > (comets[j].x - comets[j].boundx) &&
                                                bullet[i].x < (comets[j].x + comets[j].boundx) &&
                                                bullet[i].y > (comets[j].y - comets[j].boundy) &&
                                                bullet[i].y < (comets[j].y + comets[j].boundy))
                                        {
												
                                                bullet[i].live = false;
                                                comets[j].health--;
												if(comets[j].health <= 0)
												{
													if(sound == 1)
													al_play_sample(explosionsound, 0.75, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,NULL);
													CreateExplosion(explosions, NUM_EXPLOSIONS, comets[j].x, comets[j].y);
													comets[j].live = false;
													ship.score++;
													if (int (ship.score / 100.0) == float(ship.score / 100.0))
														ship.lives++;
												}
                                             
                                        }
								}
							
                        }
                }
        }
}
 
void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *asteroid)
{
        for(int i = 0; i < size; i++)
        {
                comets[i].ID = ENEMY;
                comets[i].live = false;
                comets[i].speed = rand() % 6 + 1;
                comets[i].boundx = 48;
                comets[i].boundy = 48;
				//comets[i].health = 20;

				comets[i].maxFrame = 0;
				//comets[i].curFrame = 0;
				comets[i].curFrame = rand() % 21;
				comets[i].frameCount = 0;
				comets[i].frameDelay = rand() % 80 + 20;
				comets[i].frameWidth = 96;
				comets[i].frameHeight = 96;
				comets[i].animationColumns = 20;
				comets[i].animationDirection = 0;

				//comets[i].animationRow = 0;
				comets[i].animationRow = rand() % 6;

				comets[i].image = asteroid;
        }
}
void DrawComet(Comet comets[], int size)
{
        for(int i = 0; i < size; i++)
        {
                if(comets[i].live)
                {
                        //al_draw_filled_circle(comets[i].x, comets[i].y, 20, al_map_rgb(255, 255, 0));
						int fx = (comets[i].curFrame % comets[i].animationColumns) * comets[i].frameWidth;
						int fy = comets[i].animationRow * comets[i].frameHeight;
						//int fy = comets[i].frameHeight;
 
						al_draw_bitmap_region(comets[i].image, fx, fy, comets[i].frameWidth, comets[i].frameHeight, comets[i].x - comets[i].frameWidth / 2,
																								comets[i].y - comets[i].frameHeight / 2, 0);
						comets[i].curFrame++;
						if(comets[i].curFrame >= 20)
						{
							comets[i].curFrame = 0;
							comets[i].animationRow++;
						}
						
						if(comets[i].animationRow >= 7)
							comets[i].animationRow = 0;


                }
        }
}
void StartComet(Comet comets[], int size)
{
        for(int i = 0; i < size; i++)
        {
                if(!comets[i].live)
                {
                        if(rand() % 500 == 0)
                        {
                                comets[i].live = true;
                                comets[i].x = WIDTH;
                                comets[i].y = 30 + rand() % (HEIGHT - 60);
								comets[i].health = rand() % 10 + 5;
 
                                break;
                        }
                }
        }
}
void UpdateComet(Comet comets[], int size)
{
        for(int i = 0; i < size; i++)
        {
                if(comets[i].live)
                {
                        comets[i].x -= comets[i].speed;
 
                        if(comets[i].x < 0)
                                comets[i].live = false;
                }
        }
}
 
void CollideComet(Comet comets[], int csize, SpaceShip &ship, Explosion explosions[])
{
        for(int i = 0; i < csize; i++)
        {
                if(comets[i].live)
                {
                        if(comets[i].x - comets[i].boundx < ship.x + ship.boundx &&
                                comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
                                comets[i].y - comets[i].boundy < ship.y + ship.boundy &&
                                comets[i].y + comets[i].boundy > ship.y - ship.boundy)
                        {
								if(sound == 1)
								al_play_sample(playerexplode, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,NULL);
								CreateExplosion(explosions, NUM_EXPLOSIONS, ship.x, ship.y);
                                ship.lives--;
                                comets[i].live = false;
                        }
                        else if(comets[i].x <= 0)
                        {
							comets[i].live = false;  
                        }
                }
        }
}

void InitExplosion(Explosion explosions[], int size, ALLEGRO_BITMAP *explosion)
{
	        for(int i = 0; i < size; i++)
        {
                explosions[i].live = false;

				explosions[i].maxFrame = 0;
				explosions[i].curFrame = 0;
				explosions[i].frameCount = 0;
				explosions[i].frameDelay = rand() % 80 + 20;
				explosions[i].frameWidth = 256;
				explosions[i].frameHeight = 256;
				explosions[i].animationColumns = 7;
				explosions[i].animationDirection = 0;

				explosions[i].animationRow = 0;

				explosions[i].image = explosion;
        }
}
void CreateExplosion(Explosion explosions[], int size, int xpx, int xpy)
{
		 for(int i = 0; i < size; i++)
        {
                if(!explosions[i].live)
                {
					explosions[i].x = xpx;
					explosions[i].y = xpy;
					explosions[i].live = true;

					break;
				}
			
		 }
}

void DrawExplosion(Explosion explosions[], int size)
{
        for(int i = 0; i < size; i++)
        {
                if(explosions[i].live)
                {
                      
						int fx = (explosions[i].curFrame % explosions[i].animationColumns) * explosions[i].frameWidth;
						int fy = explosions[i].animationRow * explosions[i].frameHeight;
						
 
						al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth, explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2,
																								explosions[i].y - explosions[i].frameHeight / 2, 0);
						explosions[i].curFrame++;
						if(explosions[i].curFrame >= 7)
						{
							explosions[i].curFrame = 0;
							explosions[i].animationRow++;
						}
						
						if(explosions[i].animationRow >= 5) {
							explosions[i].animationRow = 0;
							explosions[i].live = false;
						}
              }
        }
}