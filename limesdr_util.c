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

#include "limesdr_util.h"
#include <stdio.h>
#include <string.h>

int limesdr_set_channel( const unsigned int freq,
			 const double bandwidth_calibrating,
			 const double gain,
			 const unsigned int channel,
			 const char* antenna,
			 const int is_tx,
			 lms_device_t* device )

{
	int nb_antenna = LMS_GetAntennaList(device, is_tx, channel, NULL);
	lms_name_t list[ nb_antenna ];
	LMS_GetAntennaList( device, is_tx, channel, list );
	int antenna_found = 0;
	int i;
	for ( i = 0; i < nb_antenna; i++ ) {
		if ( strcmp( list[i], antenna ) == 0 ) {
			antenna_found = 1;
			if ( LMS_SetAntenna( device, is_tx, channel, i ) < 0 ) {
				fprintf(stderr, "LMS_SetAntenna() : %s\n", LMS_GetLastErrorMessage());
				return -1;
			}
		}
	}
	if ( antenna_found == 0 ) {
		fprintf( stderr, "ERROR: unable to found antenna : %s\n", antenna );
		return -1;
	}
	if ( LMS_Calibrate( device, is_tx, channel, bandwidth_calibrating, 0 ) < 0 ) {
		fprintf(stderr, "LMS_Calibrate() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_SetLOFrequency( device, is_tx, channel, freq ) < 0 ) {
		fprintf(stderr, "LMS_SetLOFrequency() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( gain >= 0 ) {
		if ( LMS_SetNormalizedGain( device, is_tx, channel, gain ) < 0 ) {
			fprintf(stderr, "LMS_SetNormalizedGain() : %s\n", LMS_GetLastErrorMessage());
			return -1;
		}
	}
	return 0;
}

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
	       )
{
	int device_count = LMS_GetDeviceList(NULL);
	if(device_count < 0){
		fprintf(stderr, "LMS_GetDeviceList() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	lms_info_str_t device_list[ device_count ];
	if ( LMS_GetDeviceList(device_list) < 0 ) {
		fprintf(stderr, "LMS_GetDeviceList() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_Open(device, device_list[ device_i ], NULL) < 0 ) {
		fprintf(stderr, "LMS_Open() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_Reset(*device) < 0 ) {
		fprintf(stderr, "LMS_Reset() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_Init(*device) < 0 ) {
		fprintf(stderr, "LMS_Init() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	int is_not_tx = (is_tx == LMS_CH_TX) ? LMS_CH_RX : LMS_CH_TX;
	if ( LMS_EnableChannel(*device, is_not_tx, channel,   false) < 0 ) {
		fprintf(stderr, "LMS_EnableChannel() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_EnableChannel(*device, is_tx,   channel,   true) < 0 ) {
		fprintf(stderr, "LMS_EnableChannel() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_SetSampleRate( *device, sample_rate, 0 ) < 0 ) {
		fprintf(stderr, "LMS_SetSampleRate() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	if ( LMS_GetSampleRate( *device, is_tx, channel, host_sample_rate, NULL ) < 0 ) {
		fprintf(stderr, "LMS_GetSampleRate() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}

	if ( limesdr_set_channel( freq, bandwidth_calibrating, gain, channel, antenna, is_tx, *device ) < 0 ) {
		return -1;
	}
	return 0;
}
