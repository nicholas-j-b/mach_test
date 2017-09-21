/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * environment.h
 * Copyright (C) 2017 Nicholas <>
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

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include <fstream>

#include "insect.h"
#include "track.h"

class Environment
{
public:
	 Environment();
	 virtual ~Environment();
	 void init_next_get();

	 bool update();
	 void determine_positions();
	 void determine_positions_simple();
	 void order_brains();
	 void save_brains();
	 void load_brains(int fileNum);
	 void evolve();
	 void pyramid_scheme();
	 float intersect_distance(int insectId, int sensorId);
	 void distance_to_track(int insectId);

	 //drawing
	 float** get_positions() {return insectPositions;}
	 float* get_track_pieces() {return track->get_track_pieces();}
	 float** get_sensor_positions() {return sensorPositions;}

	 float* get_leading_posXY() {return batch[order[0]].posXY;}
	 int* get_generationCount() {return static_cast<int*>(&generationCount);}

	 //maths
	 float get_random_float(float upper, float lower);

	 //debugging
	 void debug_printer();

protected:
	 int generationCount;
	 Insect* batch;
	 float** insectPositions;
	 float** sensorPositions;
	 Track* track;
	 //float* leadingInsectPos[2];

 	 int order[CFG::NUM_INSECTS_IN_TEST];
 	 float brainArray[CFG::NUM_INSECTS_IN_TEST * CFG::BRAIN_ARRAY_LENGTH];
	 int updates;

private:

};

#endif // _ENVIRONMENT_H_

