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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "limesdr_util.h"

int main(int argc, char** argv)
{
	if ( argc < 2 ) {
		printf("Usage: %s <OPTIONS>\n", argv[0]);
		printf("  -f <INPUT_FREQUENCY>\n"
		       "  -F <OUTPUT_FREQUENCY>\n"
		       "  -b <BANDWIDTH_CALIBRATING> (default: 8e6)\n"
		       "  -s <SAMPLE_RATE> (default: 2e6)\n"
		       "  -g <INPUT_GAIN_NORMALIZED> (default: unused)\n"
		       "  -G <OUTPUT_GAIN_NORMALIZED> (default: 1)\n"
                       "  -l <BUFFER_SIZE> (default: 1024*1024)\n"
		       "  -d <DEVICE_INDEX> (default: 0)\n"
		       "  -c <INPUT_CHANNEL_INDEX> (default: 0)\n"
		       "  -C <OUTPUT_CHANNEL_INDEX> (default: 0)\n"
		       "  -a <INPUT_ANTENNA> (LNAL | LNAH | LNAW) (default: LNAW)\n"
		       "  -A <OUTPUT_ANTENNA> (BAND1 | BAND2) (default: BAND1)\n");
		return 1;
	}
	int i;
	unsigned int freq_input = 0;
	unsigned int freq_output = 0;
	double bandwidth_calibrating = 8e6;
	double sample_rate = 2e6;
	double gain_input = -1;
	double gain_output = 1;
	unsigned int buffer_size = 1024*1024;
	unsigned int device_i = 0;
	unsigned int channel_input = 0;
	unsigned int channel_output = 0;
	char* antenna_input = "LNAW";
	char* antenna_output = "BAND1";
        for ( i = 1; i < argc-1; i += 2 ) {
		if      (strcmp(argv[i], "-f") == 0) { freq_input = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-F") == 0) { freq_output = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-b") == 0) { bandwidth_calibrating = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-s") == 0) { sample_rate = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-g") == 0) { gain_input = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-G") == 0) { gain_output = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-l") == 0) { buffer_size = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-d") == 0) { device_i = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-c") == 0) { channel_input = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-C") == 0) { channel_output = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-a") == 0) { antenna_input = argv[i+1]; }
		else if (strcmp(argv[i], "-A") == 0) { antenna_output = argv[i+1]; }
	}
	if ( freq_input == 0 ) {
		fprintf( stderr, "ERROR: invalid input frequency : %d\n", freq_input );
		return 1;
	}
	if ( freq_output == 0 ) {
		fprintf( stderr, "ERROR: invalid output frequency : %d\n", freq_output );
		return 1;
	}
	struct s16iq_sample_s {
	        short i;
		short q;
	} __attribute__((packed));
	struct s16iq_sample_s *buff = (struct s16iq_sample_s*)malloc(sizeof(struct s16iq_sample_s) * buffer_size);
	if ( buff == NULL ) {
		perror("malloc()");
		return 1;
	}
	lms_device_t* device = NULL;
	double host_sample_rate;
	if ( limesdr_init( sample_rate,
			   freq_output,
			   bandwidth_calibrating,
			   gain_output,
			   device_i,
			   channel_output,
			   antenna_output,
			   LMS_CH_TX,
			   &device,
			   &host_sample_rate) < 0 ) {
		return 1;
	}
	fprintf(stderr, "sample_rate: %f\n", host_sample_rate);

	if ( limesdr_set_channel( freq_input,
				  bandwidth_calibrating,
				  gain_input,
				  channel_input,
				  antenna_input,
				  LMS_CH_RX,
				  device ) < 0 ) {
		return 1;
	}
	lms_stream_t tx_stream = {
		.channel = channel_output,
		.fifoSize = 2 * buffer_size,
		.throughputVsLatency = 1,
		.isTx = LMS_CH_TX,
		.dataFmt = LMS_FMT_I16
	};
	if ( LMS_SetupStream(device, &tx_stream) < 0 ) {
		fprintf(stderr, "LMS_SetupStream() : %s\n", LMS_GetLastErrorMessage());
		return 1;
	}
	LMS_StartStream(&tx_stream);
	lms_stream_meta_t tx_meta;
	tx_meta.waitForTimestamp = true;
	tx_meta.flushPartialPacket = false;


	LMS_EnableChannel(device, LMS_CH_RX, channel_input, true);
	lms_stream_t rx_stream = {
		.channel = channel_input,
		.fifoSize = buffer_size * sizeof(*buff),
		.throughputVsLatency = 1,
		.isTx = LMS_CH_RX,
		.dataFmt = LMS_FMT_I16
	};
        if ( LMS_SetupStream(device, &rx_stream) < 0 ) {
		fprintf(stderr, "LMS_SetupStream() : %s\n", LMS_GetLastErrorMessage());
		return 1;
	}
	LMS_StartStream(&rx_stream);

	lms_stream_meta_t rx_meta;
	rx_meta.waitForTimestamp = false;
	rx_meta.flushPartialPacket = false;
	while( 1 ) {
		int nb_samples_to_send = LMS_RecvStream( &rx_stream, buff, buffer_size, &rx_meta, 1000 );
		if ( nb_samples_to_send < 0 ) {
			fprintf(stderr, "LMS_RecvStream() : %s\n", LMS_GetLastErrorMessage());
			break;
		}
		tx_meta.timestamp = rx_meta.timestamp + buffer_size;
	        int nb_samples = LMS_SendStream( &tx_stream, buff, nb_samples_to_send, &tx_meta, 1000 );
		if ( nb_samples < 0 ) {
			fprintf(stderr, "LMS_SendStream() : %s\n", LMS_GetLastErrorMessage());
			break;
		}
	}
	LMS_StopStream(&tx_stream);
	LMS_DestroyStream(device, &tx_stream);
	free( buff );
	LMS_EnableChannel( device, LMS_CH_TX, channel_output, false);
	LMS_Close(device);
	return 0;
}
