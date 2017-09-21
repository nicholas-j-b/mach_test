/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * insect.cc
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

#include "insect.h"
#include <iostream>
#include <cmath>


Insect::Insect(int seed):
dead(false),
speed(0),
direction(0),
posXY{0, 0},
decisionArray{0, 0, 0}
{
	srand(seed);
	for (int i = 0; i < CFG::BRAIN_ARRAY_LENGTH; i++)
	{
		brain[i] = get_random_float_minus_1_1();
	}
	//print_brain();
}

Insect::Insect(float* file):
dead(false),
speed(0),
direction(0),
posXY{0, 0},
decisionArray{0, 0, 0}
{
	for (int i = 0; i < CFG::BRAIN_ARRAY_LENGTH; i++)
	{
		brain[i] = file[i];
	}
}

Insect::~Insect()
{
}



void Insect::set_sensor(int sensorId)
{
	sensorPos[2 * sensorId] = posXY[0] + CFG::SENSOR_LENGTH * cos(direction - (M_PI / 2) + ((M_PI / (CFG::NUM_OF_SENSORS - 1)) * sensorId));
	sensorPos[2 * sensorId + 1] = posXY[1] + CFG::SENSOR_LENGTH * sin(direction - (M_PI / 2) + ((M_PI / (CFG::NUM_OF_SENSORS -1)) * sensorId));
}

float Insect::get_sensorPos(int sensorId, int x)
{
	return sensorPos[(sensorId * 2) + x];
}

void Insect::make_decision()
{
	//reinit decision slots
	for (int i = 0; i < 3; i++)
	{
		decisionArray[i] = 0;
	}
	for (int i = 0; i < CFG::NUM_OF_SENSORS; i++)
	{
		decisionArray[0] += brain[i] * sensorDistanceArray[i];
		decisionArray[1] += brain[i + CFG::NUM_OF_SENSORS] * sensorDistanceArray[i];
		decisionArray[2] += brain[i + (CFG::NUM_OF_SENSORS * 2)] * sensorDistanceArray[i];
	}
	direction += (decisionArray[0] - decisionArray[1]) / CFG::DIRECTION_WEAKENER;
	speed += decisionArray[2] * CFG::SPEED_ADJUST;
}

void Insect::calc_layers()
{
	float lay_1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float output[3] = {0.0f, 0.0f, 0.0f};

	//first layer
	for (int i = 0; i < CFG::NUM_OF_SENSORS; i++)
	{
		for (int j = 0; j < CFG::NUM_LAYER_1_NODES; j++)
		{
			lay_1[j] += /*input*/ sensorDistanceArray[i] * /*connection*/ brain[(i * CFG::NUM_LAYER_1_NODES) + j];
		}
	}
	for (int i = 0; i < CFG::NUM_LAYER_1_NODES; i++)
	{
		act_signed_root(lay_1[i]);
	}

	//second layer
	for (int i = 0; i < CFG::NUM_LAYER_1_NODES; i++)
	{
		for (int j = 0; j < CFG::NUM_OF_OUTPUTS; j++)
		{
			output[j] += /*input*/ lay_1[i] * /*connection*/ brain[CFG::NUM_OF_SENSORS * CFG::NUM_LAYER_1_NODES + (i * CFG::NUM_OF_OUTPUTS) + j];
		}
	}
	direction += (output[0] - output[1]) / CFG::DIRECTION_WEAKENER;
	speed += output[2] * CFG::SPEED_ADJUST;
}

void Insect::act_signed_root(float& x)
{
	if (x >= 0)
	{
		x = sqrt(x);
	}
	else
	{
		x = -sqrt(-x);
	}
}

void Insect::move()
{
	posXY[0] += speed * cos(direction);
	posXY[1] += speed * sin(direction);
}

void Insect::kill(int track, float dist)
{
	dead = true;
	restingTrack = track;
	restingDist = dist;
}
		

float Insect::get_random_float_0_1()
{
	float f = static_cast<float>(rand());
	f /= RAND_MAX;
	return f;
}

float Insect::get_random_float_minus_1_1()
{
	float f = static_cast<float>(rand());
	f *= 2;
	f /= RAND_MAX;
	f -= 1;
	return f;
}


void Insect::print_brain()
{
	for (int i = 0; i < CFG::BRAIN_ARRAY_LENGTH; i++)
	{
		std::cout<<brain[i]<<std::endl;
	}
}














