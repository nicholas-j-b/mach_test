/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * track.h
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

#ifndef _TRACK_H_
#define _TRACK_H_

#include "cfg.h"

class Track
{
public:
	 Track();
	 virtual ~Track();

	 void semi_circle_jaggered();
	 void wiggle();
	 void up_down();

	 float* get_track_pieces();


	 float* points;


private:

};

#endif // _TRACK_H_

