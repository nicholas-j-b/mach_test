/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * insect.h
 * Copyright (C) 2017 Nicholas <nicholas@nicholas-HP-Pavilion-15-Notebook-PC>
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

#ifndef _INSECT_H_
#define _INSECT_H_

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "cfg.h"

class Insect
{
public:
	 Insect(int seed); //init random
	 Insect(float* file); //init from file
	 //Insect(std::string brain_file, bool child) //init child from parent file
	 virtual ~Insect();

	 //init drawing
	 float* get_pos_refs() {return posXY;}
	 float* get_pos_sensors() {return sensorPos;}

	 void set_sensorDistanceArray(int j, float val) {sensorDistanceArray[j] = val;}


	 //maths
	 float get_random_float_0_1();
	 float get_random_float_minus_1_1();

	 //misc
	 void set_sensor(int sensorId);
	 float get_sensorPos(int sensorId, int x);
	 void make_decision();
	 void move();
	 void kill(int track, float dist);
	 int get_restingTrack() { return restingTrack;}
	 int get_restingDist() {return restingDist;}


	 //debug_printing
	 void print_brain();

 	 float posXY[2];
	 bool dead;

	 float brain[CFG::BRAIN_ARRAY_LENGTH];

protected:

	 float sensorPos[CFG::NUM_OF_SENSORS * 2];
	 float direction; //zero is right facing, pi is left facting
	 float speed;
	 float decisionArray[3];

	 float sensorDistanceArray[CFG::NUM_OF_SENSORS];

	 int restingTrack;
	 float restingDist;

private:

};

#endif // _INSECT_H_

