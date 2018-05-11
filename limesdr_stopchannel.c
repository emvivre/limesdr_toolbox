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

#include <lime/LimeSuite.h>
#include <stdio.h>

int main()
{
	int device_count = LMS_GetDeviceList(NULL);
	if(device_count < 0){
		fprintf(stderr, "LMS_GetDeviceList() : %s\n", LMS_GetLastErrorMessage());
		return -1;
	}
	lms_info_str_t device_list[ device_count ];
	if ( LMS_GetDeviceList(device_list) < 0 ) {
		fprintf(stderr, "LMS_GetDeviceList() : %s\n", LMS_GetLastErrorMessage());
		return 1;
	}
	int i;
	for ( i = 0; i < device_count; i++ ) {
		lms_device_t* device = NULL;
		if ( LMS_Open(&device, device_list[ i ], NULL) < 0 ) {
			return 1;
		}
		LMS_Reset(device);
		LMS_Init(device);
		int nb_channel = LMS_GetNumChannels( device, LMS_CH_RX );
		int c;
		for( c = 0; c < nb_channel; c++ ) {
			LMS_EnableChannel(device, LMS_CH_RX, c, false);
		}
		nb_channel = LMS_GetNumChannels( device, LMS_CH_TX );
		for( c = 0; c < nb_channel; c++ ) {
			LMS_EnableChannel(device, LMS_CH_TX, c, false);
		}
		LMS_Close(device);
	}
	return 0;
}
