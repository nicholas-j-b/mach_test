/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2017 Nicholas
 * 
 * mach_test is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mach_test is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "environment.h"


int main()
{
	int screen_width = 1400;
	int screen_height = 900;
	float FPS = static_cast<float>(CFG::FPS);

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
	if(!al_init())
		return -1;
	display = al_create_display(screen_width, screen_height);
	if(!display)
		return -1;
	al_set_window_position(display, 10, 10);

	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();

	timer = al_create_timer(1.0/FPS);
	eventQueue = al_create_event_queue();
	ALLEGRO_FONT* font20 = al_load_font("ub_font.ttf", 20, 0);

	al_register_event_source(eventQueue, al_get_timer_event_source(timer));
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());

	//environment conditions
	Environment *environment = new Environment;
	float** e_positions = environment->get_positions();
	float* e_trackPieces = environment->get_track_pieces();
	float** e_sensorPositions = environment->get_sensor_positions();
	bool pauseTimer = false;
	float* e_leadingPosXY = NULL;
	//int* e_generationCount = environment->get_generationCount();
	int generationCount = !CFG::INSTANT_START * CFG::STARTING_POINT;

	if(!CFG::INSTANT_START)
	{
		for (int i = 0; i < CFG::STARTING_POINT; i++)
		{
			if(i % (CFG::STARTING_POINT / 100) == 0)
			{
				std::cout<<((i * 100) / CFG::STARTING_POINT) + 1<<"%"<<std::endl;
			}
			while(true)
			{
				if(!environment->update())
				{
					environment->determine_positions_simple ();
					environment->order_brains();
					//environment->evolve();
					environment->pyramid_scheme();
					environment->init_next_get();
					break;
				}
			}
		}
	}

	al_start_timer(timer);
	while(true)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(eventQueue, &ev);
		if((ev.type == ALLEGRO_EVENT_TIMER) && (pauseTimer == false))
		{

			if(environment->update())
			{				
				//get leading insect
				environment->determine_positions_simple();
				e_leadingPosXY = environment->get_leading_posXY();
				
				//draw track
				for(int j = 0; j < CFG::NUM_TRACK_PIECES; j++)
				{
					al_draw_line((((e_trackPieces[j * 2] - e_leadingPosXY[0]) * CFG::ZOOM) + (screen_width / 2)), 
								(((e_trackPieces[(j * 2) + 1] - e_leadingPosXY[1]) * CFG::ZOOM) + (screen_height / 2)),
								(((e_trackPieces[(j + 1) * 2] - e_leadingPosXY[0]) * CFG::ZOOM) + (screen_width / 2)), 
						        (((e_trackPieces[((j + 1) * 2) + 1] - e_leadingPosXY[1]) * CFG::ZOOM) + (screen_height / 2)), al_map_rgb(200, 150, 0), CFG::TRACK_DRAW_WIDTH * CFG::ZOOM);
					al_draw_filled_circle((((e_trackPieces[j * 2] - e_leadingPosXY[0]) * CFG::ZOOM) + (screen_width / 2)), 
								(((e_trackPieces[(j * 2) + 1] - e_leadingPosXY[1]) * CFG::ZOOM) + (screen_height / 2)),(CFG::TRACK_DRAW_WIDTH / 2) * CFG::ZOOM,al_map_rgb(200, 150, 0));
				}

				//draw insects
				for(int j = 0; j < CFG::NUM_INSECTS_IN_TEST; j++)
				{
					al_draw_filled_circle((((*e_positions[j] - e_leadingPosXY[0]) * CFG::ZOOM) + (screen_width / 2)),
										(((*(e_positions[j] + 1) - e_leadingPosXY[1]) * CFG::ZOOM) + (screen_height / 2)), 
						                 CFG::NODE_DRAW_SIZE * CFG::ZOOM, al_map_rgb(100,30,20));
					for(int k = 0; k < CFG::NUM_OF_SENSORS; k++)
					{
						al_draw_circle((((*(e_sensorPositions[j] + (2 * k)) - e_leadingPosXY[0]) * CFG::ZOOM) + (screen_width / 2)), 
							           (((*(e_sensorPositions[j] + (2 * k) + 1) - e_leadingPosXY[1]) * CFG::ZOOM) + (screen_height / 2)),
							           CFG::SENSOR_DRAW_SIZE * CFG::ZOOM, al_map_rgb(255,255,255), 3);
					} 
				}
				//draw current gen
				al_draw_textf(font20, al_map_rgb(255,255,255), screen_width/1.2, screen_height/1.2, ALLEGRO_ALIGN_CENTRE, "Generation: %i", generationCount);

				//flip buffer and clear
				al_flip_display();
				al_clear_to_color(al_map_rgb(50,100,30));
			}
			else
			{
				pauseTimer = true;
				generationCount++;
				environment->determine_positions_simple();
				environment->order_brains();
				//environment->evolve();
				environment->pyramid_scheme();
				environment->init_next_get();
				pauseTimer = false;


			}

		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
			{
				pauseTimer = true;
				generationCount++;
				environment->determine_positions_simple();
				environment->order_brains();
				//environment->evolve();
				environment->pyramid_scheme();
				environment->init_next_get();
				pauseTimer = false;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				break;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_S)
			{
				std::cout<<"s pressed"<<std::endl;
				pauseTimer = true;
				environment->save_brains();
				pauseTimer = false;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_L)
			{
				std::cout<<"l pressend"<<std::endl;
				pauseTimer = true;
				environment->load_brains(0);
				environment->init_next_get();
				pauseTimer = false;
			}
		}
	}

	//environment.debug_printer();

	delete environment;

	al_uninstall_keyboard();
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(eventQueue);
	
	return 0;
}








#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}
/*
int main()
{
   using std::cout;
   using std::endl;

   double vm, rss;
   process_mem_usage(vm, rss);
   cout << "VM: " << vm << "; RSS: " << rss << endl;
}*/