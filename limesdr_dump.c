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
#include "limesdr_util.h"

int main(int argc, char** argv)
{
	if ( argc < 2 ) {
		printf("Usage: %s <OPTIONS>\n", argv[0]);
		printf("  -f <FREQUENCY>\n"
		       "  -b <BANDWIDTH_CALIBRATING> (default: 8e6)\n"
		       "  -s <SAMPLE_RATE> (default: 2e6)\n"
		       "  -g <GAIN_NORMALIZED> (default: 1)\n"
                       "  -l <BUFFER_SIZE>  (default: 1024*1024)\n"
		       "  -d <DEVICE_INDEX> (default: 0)\n"
		       "  -c <CHANNEL_INDEX> (default: 0)\n"
		       "  -a <ANTENNA> (LNAL | LNAH | LNAW) (default: LNAW)\n"
		       "  -o <OUTPUT_FILENAME> (default: stdout)\n");
		return 1;
	}
	int i;
	unsigned int freq = 0;
	double bandwidth_calibrating = 8e6;
	double sample_rate = 2e6;
	double gain = 1;
	unsigned int buffer_size = 1024*1024;
	unsigned int device_i = 0;
	unsigned int channel = 0;
	char* antenna = "LNAW";
	char* output_filename = NULL;
	for ( i = 1; i < argc-1; i += 2 ) {
		if      (strcmp(argv[i], "-f") == 0) { freq = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-b") == 0) { bandwidth_calibrating = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-s") == 0) { sample_rate = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-g") == 0) { gain = atof( argv[i+1] ); }
		else if (strcmp(argv[i], "-l") == 0) { buffer_size = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-d") == 0) { device_i = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-c") == 0) { channel = atoi( argv[i+1] ); }
		else if (strcmp(argv[i], "-a") == 0) { antenna = argv[i+1]; }
		else if (strcmp(argv[i], "-o") == 0) { output_filename = argv[i+1]; }
	}
	if ( freq == 0 ) {
		fprintf( stderr, "ERROR: invalid frequency : %d\n", freq );
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
	FILE* fd = stdout;
	if ( output_filename != NULL ) {
		fd = fopen( output_filename, "w+b" );
		if ( fd == NULL ) {
			perror("fopen()");
			return 1;
		}
	} else {
		stdout = stderr;
	}
	lms_device_t* device = NULL;
	double host_sample_rate = 10.0;
	if ( limesdr_init( sample_rate,
			   freq,
			   bandwidth_calibrating,
			   gain,
			   device_i,
			   channel,
			   antenna,
			   LMS_CH_RX,
			   &device,
			   &host_sample_rate) < 0 ) {
		return 1;
	}
	fprintf(stderr, "sample_rate: %f\n", host_sample_rate);

	lms_stream_t rx_stream = {
		.channel = channel,
		.fifoSize = buffer_size * sizeof(*buff),
		.throughputVsLatency = 0.5,
		.isTx = LMS_CH_RX,
		.dataFmt = LMS_FMT_I16
	};
	if ( LMS_SetupStream(device, &rx_stream) < 0 ) {
		fprintf(stderr, "LMS_SetupStream() : %s\n", LMS_GetLastErrorMessage());
		return 1;
	}
	LMS_StartStream(&rx_stream);
	while( 1 ) {
		int nb_samples = LMS_RecvStream( &rx_stream, buff, buffer_size, NULL, 1000 );
		if ( nb_samples < 0 ) {
			fprintf(stderr, "LMS_RecvStream() : %s\n", LMS_GetLastErrorMessage());
			break;
		}
		fwrite( buff, sizeof( *buff ), nb_samples, fd );
		fflush( fd );
	}
	LMS_StopStream(&rx_stream);
	LMS_DestroyStream(device, &rx_stream);
	free( buff );
	fclose( fd );
	LMS_Close(device);
	return 0;
}
