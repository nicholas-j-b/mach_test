#ifndef _CFG_H_
#define _CFG_H_

//minimum track piece length ~100?


namespace CFG
{
	//draw characteristics
	const float NODE_DRAW_SIZE = 30;
	const float SENSOR_DRAW_SIZE = 13;
	const int TRACK_DRAW_WIDTH = 260;
	const float CAMERA_ADJUST = 250;
	const float ZOOM = .3;
	const int FPS = 60;
	
	//insect characteristics
	const int NUM_OF_SENSORS = 5;
	const int NUM_OF_OUTPUTS = 3;
	const int BRAIN_ARRAY_LENGTH = NUM_OF_OUTPUTS * NUM_OF_SENSORS;
	const float SENSOR_LENGTH = 50;
	const float DIRECTION_WEAKENER = 100;
	const float SPEED_ADJUST = .3;

	//environment characteristics
	const int NUM_INSECTS_IN_TEST = 150;
	const int NUM_TRACK_PIECES = 21;
	const int TRACK_LENGTH_LIMIT = 10000;
	const int TRACK_DEVIATION = 250;

	//evolution characteristics
	const int NUM_BREEDING_INSECTS = NUM_INSECTS_IN_TEST / 4;
	const float SMALL_DEVIATION_L = .999;
	const float SMALL_DEVIATION_U = 1.001;
	const float LARGE_DEVIATION_L = .99;
	const float LARGE_DEVIATION_U = 1.1;
	const int SMALL_DEVIATION_PROB = 2;
	const int LARGE_DEVIATION_PROB = 20; //after small dev
	const int SIGN_REVERSAL_PROB = 2000; //after the other two
	const int DEATH_QUOTA = static_cast<int>(static_cast<float>(CFG::NUM_INSECTS_IN_TEST) * 1);
	const int TIMER = 30;

	//save data
	const int MAGIC_NUMBER = 1852400491;
	const int MAGIC_BYTE_SIZE = 4;
	const int HEADER_INFO_LENGTH = 3;

	//main
	const int STARTING_POINT = 250;
	const bool INSTANT_START = true;
}

#endif //_CFG_H_