/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * track.cc
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

#include "track.h"
#include <iostream>
#include <math.h>

#define PI 3.14159265f

Track::Track()
{
	up_down ();
}

void Track::semi_circle_jaggered()
{
	points = new float[(CFG::NUM_TRACK_PIECES + 1) * 2];

	int i = 0;
	while (i < (CFG::NUM_TRACK_PIECES + 1))
	{
		points[i] = sin((PI * i) / (1 * float(CFG::NUM_TRACK_PIECES))) * CFG::TRACK_DEVIATION * 10;
		points[i + 1] = -(cos((PI * i) / (1 * float(CFG::NUM_TRACK_PIECES))) * CFG::TRACK_DEVIATION - CFG::TRACK_DEVIATION) * 10;
		std::cout<<"x: "<<points[i]<<"\t\ty: "<<points[i + 1]<<std::endl;
		i += 2;
	}
	while (i < (CFG::NUM_TRACK_PIECES + 1) * 2)
	{
		points[i] = sin((PI * i) / (1 * float(CFG::NUM_TRACK_PIECES))) * CFG::TRACK_DEVIATION * 10;
		points[i + 1] = (cos((PI * i) / (1 * float(CFG::NUM_TRACK_PIECES))) * CFG::TRACK_DEVIATION + (3 * CFG::TRACK_DEVIATION)) * 10;
		std::cout<<"x: "<<points[i]<<"\t\ty: "<<points[i + 1]<<std::endl;
		i += 2;
	}
		
}

void Track::wiggle()
{
	points = new float[(CFG::NUM_TRACK_PIECES + 1) * 2];

	for (int i = 0; i < (CFG::NUM_TRACK_PIECES + 1) * 2; i += 2)
	{
		points[i] = i * CFG::TRACK_DEVIATION;
		points[i + 1] = (i * CFG::TRACK_DEVIATION * ((i % 4)) / CFG::NUM_TRACK_PIECES);
	}
}

void Track::up_down()
{
	points = new float[(CFG::NUM_TRACK_PIECES + 1) * 2];

	for (int i = 0; i < (CFG::NUM_TRACK_PIECES + 1) * 2; i += 2)
	{
		points[i] = i * CFG::TRACK_DEVIATION;
		points[i + 1] = (CFG::TRACK_DEVIATION / 1.7) * (i % 4);
	}
}

Track::~Track()
{
	delete[] points;
}

float* Track::get_track_pieces()
{
	return points;
}











