//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef SIMPLETIMER_HPP
#define SIMPLETIMER_HPP

class SimpleTimer
{
private:
	int mStartTicks; // The time when the timer was started
	int mEndTicks; // The time when the timer has stopped

public:
	SimpleTimer();
	~SimpleTimer();

	int start();
	int getTicks();
};

#endif /* SIMPLETIMER_HPP */