/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * environment.cc
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

#include <new>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string.h>


#include "environment.h"


Environment::Environment()
: updates(0)
{
	srand(time(NULL));
	insectPositions = new float*[CFG::NUM_INSECTS_IN_TEST];
	sensorPositions = new float*[CFG::NUM_INSECTS_IN_TEST];
	batch = static_cast<Insect*>(::operator new(sizeof(Insect) * CFG::NUM_INSECTS_IN_TEST));
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		new(&batch[i]) Insect(int(rand()));
		insectPositions[i] = batch[i].get_pos_refs();
		sensorPositions[i] = batch[i].get_pos_sensors();
	}
	track = new Track;

	
}

Environment::~Environment()
{
	delete[] insectPositions;
	delete[] sensorPositions;
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		batch[i].~Insect();
	}
	::operator delete(batch);
	delete track;
}

void Environment::init_next_get()
{
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		batch[i].~Insect();
		new(&batch[i]) Insect(&brainArray[i*CFG::BRAIN_ARRAY_LENGTH]);
		insectPositions[i] = batch[i].get_pos_refs();
		sensorPositions[i] = batch[i].get_pos_sensors();
	}
}

bool Environment::update()
{
	if (updates > (CFG::TIMER * CFG::FPS))
	{
		updates = 0;
		return false;
	}
	updates++;
	float temp;
	int numDeadInsects = 0;
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		//check if dead
		if (batch[i].dead)
		{
			numDeadInsects++;
			continue;
		}
		//take sensor readings
		for (int j = 0; j < CFG::NUM_OF_SENSORS; j++)
		{
			batch[i].set_sensor(j);
			temp = intersect_distance(i, j);
			batch[i].set_sensorDistanceArray(j, temp);
		}

		//make a decision - set velocity, direction etc
		batch[i].make_decision();
		
		//act - move insect
		batch[i].move();

		//determine distance to nearest path and kill appropriately
		distance_to_track(i);
	}
	if (numDeadInsects >= CFG::DEATH_QUOTA)
	{
		return false;
	}
	return true;
}

void Environment::distance_to_track(int insectId)
{
	int nearestTrackPiece;
	//insect pos as point
	const float p0x = batch[insectId].posXY[0];
	const float p0y = batch[insectId].posXY[1];
	float distNearestTrack = CFG::SENSOR_LENGTH * 3;
	for (int j = 0; j < CFG::NUM_TRACK_PIECES; j++)
	{
		//two line segment ends as points
		const float p1x = track->points[j * 2];
		const float p1y = track->points[(j * 2) + 1];
		const float p2x = track->points[(j + 1) * 2];
		const float p2y = track->points[((j + 1)* 2) + 1];

		const float trackLength2 = pow(p1x - p2x, 2) + pow (p1y - p2y, 2);
		const float trackPointDot = ((p1x - p2x) * (p0x - p2x)) + ((p1y - p2y) * (p0y - p2y));
		const float t = std::max(0.0f, std::min(1.0f, trackPointDot / trackLength2));
		const float slidingProjX = p2x + (t * (p1x - p2x));
		const float slidingProjY = p2y + (t * (p1y - p2y));
		
		float n_distNearestTrack = sqrt(pow(p0x - slidingProjX, 2) + pow(p0y - slidingProjY, 2));
		if (n_distNearestTrack < distNearestTrack)
		{
			nearestTrackPiece = j;
			distNearestTrack = n_distNearestTrack;
		}
	}

	//kill lost insects
	if (distNearestTrack >= CFG::SENSOR_LENGTH * 2)
	{
		//final resting place
		float restingDist = sqrt(pow(track->points[(nearestTrackPiece) * 2] - p0x, 2) + pow(track->points[((nearestTrackPiece)* 2) + 1] - p0y, 2));
		batch[insectId].kill(nearestTrackPiece, restingDist);
	}
}
	

void Environment::determine_positions()
{
	int tempOrder = 0;
	float restingDists[CFG::NUM_INSECTS_IN_TEST];
	for(int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		restingDists[i] = batch[i].get_restingDist() + (CFG::TRACK_LENGTH_LIMIT * batch[i].get_restingTrack());
	}
	float currentBiggest = 0;
	float previousBiggest = CFG::TRACK_LENGTH_LIMIT * CFG::NUM_TRACK_PIECES * 2;
	for(int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		for(int j = 0; j < CFG::NUM_INSECTS_IN_TEST; j++)
		{
			if((previousBiggest > restingDists[j]) && (restingDists[j] > currentBiggest))
			{
				currentBiggest = restingDists[j];
				tempOrder = j;
			}
		}
		order[i] = tempOrder;
		previousBiggest = currentBiggest;
		currentBiggest = 0;
	}
}

void Environment::determine_positions_simple()
{
	int tempOrder = 0;
	float restingDists[CFG::NUM_INSECTS_IN_TEST];
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		restingDists[i] = batch[i].posXY[0];
	}
	float currentBiggest = 0;
	float previousBiggest = CFG::TRACK_LENGTH_LIMIT * CFG::NUM_TRACK_PIECES * 2;
	for(int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		for(int j = 0; j < CFG::NUM_INSECTS_IN_TEST; j++)
		{
			if((previousBiggest > restingDists[j]) && (restingDists[j] > currentBiggest))
			{
				currentBiggest = restingDists[j];
				tempOrder = j;
			}
		}
		order[i] = tempOrder;
		previousBiggest = currentBiggest;
		currentBiggest = 0;
	}
}

void Environment::order_brains()
{
	for (int i = 0; i < CFG::NUM_BREEDING_INSECTS; i++)
	{
		for (int j = 0; j < CFG::BRAIN_ARRAY_LENGTH; j++)
		{
			brainArray[(i * CFG::BRAIN_ARRAY_LENGTH) + j] = batch[order[i]].brain[j];
		}
	}
}

void Environment::save_brains()
{
	int magicNumber = CFG::MAGIC_NUMBER;
	int headerInfos[] = {CFG::NUM_INSECTS_IN_TEST, CFG::NUM_OF_SENSORS, CFG::NUM_OF_OUTPUTS};
	
	char* memHeader00 = reinterpret_cast<char*>(&magicNumber);
	char* memHeader01 = reinterpret_cast<char*>(headerInfos);
	char* memBrains = reinterpret_cast<char*>(brainArray);
	
	std::streampos header00Size = CFG::MAGIC_BYTE_SIZE;
	std::streampos header01Size = CFG::HEADER_INFO_LENGTH * sizeof(int);
	std::streampos brainSize = CFG::NUM_INSECTS_IN_TEST * sizeof(float) * CFG::BRAIN_ARRAY_LENGTH;
	
	std::ofstream log00("saves/log00.bin", std::ios::out | std::ios::binary);
	if(log00.is_open())
	{
		log00.write(memHeader00, header00Size);
		log00.write(memHeader01, header01Size);
		log00.write(memBrains, brainSize);
	}
	else
	{
		std::cout<<"log not open for writing"<<std::endl;
	}
	log00.close();
	std::cout<<"save function finished"<<std::endl;
}

//load into brainArray
void Environment::load_brains(int fileNum)
{
	int magicNumber;
	int headerInfos[] = {CFG::NUM_INSECTS_IN_TEST, CFG::NUM_OF_SENSORS, CFG::NUM_OF_OUTPUTS};

	std::streampos header00Size = CFG::MAGIC_BYTE_SIZE;
	std::streampos header01Size = CFG::HEADER_INFO_LENGTH * sizeof(int);
	std::streampos brainSize = CFG::NUM_INSECTS_IN_TEST * sizeof(float) * CFG::BRAIN_ARRAY_LENGTH;

	char memHeader01[int(header01Size)];
	
	std::ifstream log("saves/log00.bin", std::ios::in | std::ios::binary);
	if(log.is_open())
	{
		log.read((char*)&magicNumber, header00Size);
		if (magicNumber == CFG::MAGIC_NUMBER)
		{
			log.read(memHeader01, header01Size);
			int n = memcmp(memHeader01, headerInfos, size_t(header01Size));
			if(n == 0)
			{
				log.read((char*)&brainArray, brainSize);
			}
			else
			{
				std::cout<<n<<std::endl;
				std::cout<<"file incompatible yet correct type"<<std::endl;
				throw;
			}
		}
		else
		{
			std::cout<<"incorrect file type"<<std::endl;
			throw;
		}
	}
	else
	{
		std::cout<<"log not open for loading"<<std::endl;
	}

	log.close();
	std::cout<<"load function finished"<<std::endl;

	
}

//populate brainArray
void Environment::evolve()
{
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		for (int j = 0; j < CFG::BRAIN_ARRAY_LENGTH; j++)
		{
			int prob = rand();
			float quotient = 1;
			if((prob % CFG::SMALL_DEVIATION_PROB) == 0)
			{
				quotient = get_random_float(CFG::SMALL_DEVIATION_U, CFG::SMALL_DEVIATION_L);
			}
			else 
			{
				prob = rand();
			}
			if((prob % CFG::LARGE_DEVIATION_PROB) == 0)
			{
				quotient = get_random_float(CFG::LARGE_DEVIATION_U, CFG::LARGE_DEVIATION_L);
			}
			else
			{
				prob = rand();
			}
			if((prob % CFG::SIGN_REVERSAL_PROB) == 0)
			{
				quotient = -1.0f;
			}
			//std::cout<<"voher: "<<brainArray[(i * CFG::BRAIN_ARRAY_LENGTH) + j]<<std::endl;
			int source = ((i * CFG::BRAIN_ARRAY_LENGTH) + j) % (CFG::NUM_BREEDING_INSECTS * CFG::BRAIN_ARRAY_LENGTH);
			// ^^ maybe check -- dubious understanding of this maths ^^
			brainArray[(i * CFG::BRAIN_ARRAY_LENGTH) + j] = brainArray[source] * quotient;
			//std::cout<<"nachher: "<<brainArray[(i * CFG::BRAIN_ARRAY_LENGTH) + j]<<std::endl;
			
		}
	}
}

void Environment::pyramid_scheme()
{
	int source;
	int ticker = 1;
	int counter = 1;
	for (int i = 0; i < CFG::NUM_INSECTS_IN_TEST; i++)
	{
		for (int j = 0; j < CFG::BRAIN_ARRAY_LENGTH; j++)
		{
			int prob = rand();
			float quotient = 1;
			if((prob % CFG::SMALL_DEVIATION_PROB) == 0)
			{
				quotient = get_random_float(CFG::SMALL_DEVIATION_U, CFG::SMALL_DEVIATION_L);
			}
			else 
			{
				prob = rand();
			}
			if((prob % CFG::LARGE_DEVIATION_PROB) == 0)
			{
				quotient = get_random_float(CFG::LARGE_DEVIATION_U, CFG::LARGE_DEVIATION_L);
			}
			else
			{
				prob = rand();
			}
			if((prob % CFG::SIGN_REVERSAL_PROB) == 0)
			{
				quotient = -1.0f;
			}
			source = ((i * CFG::BRAIN_ARRAY_LENGTH) + j) % (counter * CFG::BRAIN_ARRAY_LENGTH);
			brainArray[(i * CFG::BRAIN_ARRAY_LENGTH) + j] = brainArray[source] * quotient;
		}
		ticker++;
		if (ticker % counter == 0)
		{
			counter++;
			ticker = 0;
		}
	}
}


//returns a float between 0 and 1
//inverse of distance
/*float Environment::intersect_distance(int insectId, int sensorId)
{
	float distances[3] = {0, 0, 0};
	int distanceCount = 0;
	
	for(int i = 0; i < CFG::NUM_TRACK_PIECES; i++)
	{
		//check boundingbox intersect
		float a = batch[insectId].posXY[0];
		float b = batch[insectId].posXY[1];
		float c = batch[insectId].get_sensorPos(sensorId, 0);
		float d = batch[insectId].get_sensorPos(sensorId, 1);
		float w = track->points[i * 2];
		float x = track->points[(i * 2) + 1];
		float y = track->points[(i + 1) * 2];
		float z = track->points[((i + 1)* 2) + 1];

		if(		((a <= w <= c)
		   ||   (a >= w >= c)
		   ||   (a <= y <= c)
		   ||   (a >= y >= c)	)

		   && (	(b <= x <= d)
		   ||   (b >= x >= d)
		   ||   (b <= z <= d)
		   ||   (b >= z >= d))  )
		{
			//check line intersect
			float m1 = (c - a) / (d - b);
			float m2 = (y - w) / (z - x);
			float beta1 = b - (m1 * a);
			float beta2 = x - (m2 * w);
			float xVal = (beta2 - beta1) / (m1 - m2);
			float yVal = (m1 * xVal) + beta1;
			if((	(a <= xVal <= b)
			    ||  (a >= xVal >= b))
			   &&(  (w <= xVal <= x)
			    ||  (w >= xVal >= x))
			   &&(  (c <= yVal <= d)
			    ||  (c >= yVal >= d))
			   &&(  (y <= yVal <= z)
			    ||  (y >= yVal >= z)))
			{
				distances[distanceCount] = std::sqrt(std::pow(xVal - a, 2) + std::pow(yVal - b, 2));
				distanceCount++;
			}
		}
	}
	float smallest = CFG::SENSOR_LENGTH;
	for(int i = 0; i < distanceCount; i++)
	{
		if(distances[i] < smallest)
			smallest = distances[i];
	}
	return 1 - (smallest / CFG::SENSOR_LENGTH);
}*/



float Environment::intersect_distance(int insectId, int sensorId)
{
	float distances[CFG::NUM_TRACK_PIECES];
	for (int i = 0; i < CFG::NUM_TRACK_PIECES; i++)
	{
		distances[i] = 0;
	}
	int distanceCount = 0;

	//check boundingbox intersect
	float a = batch[insectId].posXY[0];
	float b = batch[insectId].posXY[1];
	float c = batch[insectId].get_sensorPos(sensorId, 0);
	float d = batch[insectId].get_sensorPos(sensorId, 1);
	float w;
	float x;
	float y;
	float z;
	float m1;
	float m2;
	float beta1;
	float beta2;
	float xVal;
	float yVal;

	
	for(int i = 0; i < (CFG::NUM_TRACK_PIECES); i++)
	{
		//check boundingbox intersect
		w = track->points[i * 2];
		x = track->points[(i * 2) + 1];
		y = track->points[(i + 1) * 2];
		z = track->points[((i + 1)* 2) + 1];

		if(		((a <= w <= c)
		   ||   (a >= w >= c)
		   ||   (a <= y <= c)
		   ||   (a >= y >= c)	)

		   && (	(b <= x <= d)
		   ||   (b >= x >= d)
		   ||   (b <= z <= d)
		   ||   (b >= z >= d))  )
		{
			//check line intersect
			m1 = (c - a) / (d - b);
			m2 = (y - w) / (z - x);
			beta1 = b - (m1 * a);
			beta2 = x - (m2 * w);
			xVal = (beta2 - beta1) / (m1 - m2);
			yVal = (m1 * xVal) + beta1;
			if((	(a <= xVal <= b)
			    ||  (a >= xVal >= b))
			   &&(  (w <= xVal <= x)
			    ||  (w >= xVal >= x))
			   &&(  (c <= yVal <= d)
			    ||  (c >= yVal >= d))
			   &&(  (y <= yVal <= z)
			    ||  (y >= yVal >= z)))
			{
				distances[distanceCount] = std::sqrt(std::pow(xVal - a, 2) + std::pow(yVal - b, 2));
				distanceCount++;	
			}
		}
	}
	float smallest = CFG::SENSOR_LENGTH;
	for(int i = 0; i < distanceCount; i++)
	{
		if(distances[i] < smallest)
			smallest = distances[i];
	}
	return 1 - (smallest / CFG::SENSOR_LENGTH);
}











/*float Environment::intersect_distance(int insectId, int sensorId)
{
	float distances[3] = {0, 0, 0};
	int distanceCount = 0;

	float e = new float[14];

	//check boundingbox intersect
	float a = batch[insectId].posXY[0];
	float b = batch[insectId].posXY[1];
	float c = batch[insectId].get_sensorPos(sensorId, 0);
	float d = batch[insectId].get_sensorPos(sensorId, 1);
	float w;
	float x;
	float y;
	float z;
	float m1;
	float m2;
	float beta1;
	float beta2;
	float xVal;
	float yVal;
	
	for(int i = 0; i < CFG::NUM_TRACK_PIECES; i++)
	{
		//check boundingbox intersect
		w = track->points[i * 2];
		x = track->points[(i * 2) + 1];
		y = track->points[(i + 1) * 2];
		z = track->points[((i + 1)* 2) + 1];
		
		if(		((a <= w <= c)
		   ||   (a >= w >= c)
		   ||   (a <= y <= c)
		   ||   (a >= y >= c)	))
		{
		   if((	(b <= x <= d)
		   ||   (b >= x >= d)
		   ||   (b <= z <= d)
		   ||   (b >= z >= d))  )
			{
				//check line intersect
				m1 = (c - a) / (d - b);
				m2 = (y - w) / (z - x);
				beta1 = b - (m1 * a);
				beta2 = x - (m2 * w);
				xVal = (beta2 - beta1) / (m1 - m2);
				yVal = (m1 * xVal) + beta1;
				if((	(a <= xVal <= b)
					||  (a >= xVal >= b)))
			   {
				   if((  (w <= xVal <= x)
					||  (w >= xVal >= x)))
					{
					   if((  (c <= yVal <= d)
						||  (c >= yVal >= d)))
					   {
						   if((  (y <= yVal <= z)
							||  (y >= yVal >= z)))
							{
								distances[distanceCount] = std::sqrt(std::pow(xVal - a, 2) + std::pow(yVal - b, 2));
								distanceCount++;	
							}
					   }
				   }
				}
		   }
		}
	}
	float smallest = CFG::SENSOR_LENGTH;
	for(int i = 0; i < distanceCount; i++)
	{
		if(distances[i] < smallest)
			smallest = distances[i];
	}
	delete[] e;
	return 1 - (smallest / CFG::SENSOR_LENGTH);
}*/









/*float Environment::intersect_distance(int insectId, int sensorId)
{
	float distances[CFG::NUM_TRACK_PIECES];
	for (int i = 0; i < CFG::NUM_TRACK_PIECES; i++)
	{
		distances[i] = 0;
	}
	int distanceCount = 0;

	float* e = new float[14];

	//check boundingbox intersect
	e[0] = batch[insectId].posXY[0];
	e[1] = batch[insectId].posXY[1];
	e[2] = batch[insectId].get_sensorPos(sensorId, 0);
	e[3] = batch[insectId].get_sensorPos(sensorId, 1);
	
	for(int i = 0; i < (CFG::NUM_TRACK_PIECES) * 2; i += 2)
	{
		std::cout<<i<<std::endl;
		//check boundingbox intersect
		e[4] = track->points[i];
		e[5] = track->points[(i) + 1];
		e[6] = track->points[(i + 3)];
		e[7] = track->points[((i + 4))];

		std::cout<<"after trackpoints"<<std::endl;
		
		if(		((e[0] <= e[4] <= e[2])
		   ||   (e[0] >= e[4] >= e[2])
		   ||   (e[0] <= e[6] <= e[2])
		   ||   (e[0] >= e[6] >= e[2])	))
		{
		   if((	(e[1] <= e[5] <= e[3])
		   ||   (e[1] >= e[5] >= e[3])
		   ||   (e[1] <= e[7] <= e[3])
		   ||   (e[1] >= e[7] >= e[3]))  )
			{
				//check line intersect
				e[8] = (e[2] - e[0]) / (e[3] - e[1]);
				e[9] = (e[6] - e[4]) / (e[7] - e[5]);
				e[10] = e[1] - (e[8] * e[0]);
				e[11] = e[5] - (e[9] * e[4]);
				e[12] = (e[11] - e[10]) / (e[8] - e[9]);
				e[13] = (e[8] * e[12]) + e[10];
				if((	(e[0] <= e[12] <= e[1])
					||  (e[0] >= e[12] >= e[1])))
			   {
				   if((  (e[4] <= e[12] <= e[5])
					||  (e[4] >= e[12] >= e[5])))
					{
					   if((  (e[2] <= e[13] <= e[3])
						||  (e[2] >= e[13] >= e[3])))
					   {
						   if((  (e[6] <= e[13] <= e[7])
							||  (e[6] >= e[13] >= e[7])))
							{
								distances[distanceCount] = std::sqrt(std::pow(e[12] - e[0], 2) + std::pow(e[13] - e[1], 2));
								distanceCount++;	
							}
					   }
				   }
				}
		   }
		}
	}
	float smallest = CFG::SENSOR_LENGTH;
	for(int i = 0; i < distanceCount; i++)
	{
		if(distances[i] < smallest)
			smallest = distances[i];
	}
	delete[] e;
	return 1 - (smallest / CFG::SENSOR_LENGTH);
}*/











float Environment::get_random_float(float upper, float lower)
{
	float dist = upper - lower;
	float f = static_cast<float>(rand());
	f /= RAND_MAX;
	f *= dist;
	f += lower;
	return f;
}


void Environment::debug_printer()
{
	batch[8].print_brain();
}













