/*
  ===========================================================================

  Copyright (C) 2018 Emvivre

  This file is part of LIMESDR_TOOLBOX.

  LIMESDR_TOOLBOX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LIMESDR_TOOLBOX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LIMESDR_TOOLBOX.  If not, see <http://www.gnu.org/licenses/>.

  ===========================================================================
*/

#ifndef _LIMESDR_UTIL_H_
#define _LIMESDR_UTIL_H_

#include <lime/LimeSuite.h>

int limesdr_init( const double sample_rate,
		  const unsigned int freq,
		  const double bandwidth_calibrating,
		  const double gain,
		  const unsigned int device_i,
		  const unsigned int channel,
		  const char* antenna,
		  const int is_tx,
		  lms_device_t** device,
		  double* host_sample_rate
		  );

int limesdr_set_channel( const unsigned int freq,
			 const double bandwidth_calibrating,
			 const double gain,
			 const unsigned int channel,
			 const char* antenna,
			 const int is_tx,
			 lms_device_t* device );

#endif /* !_LIMESDR_UTIL_H_ */
