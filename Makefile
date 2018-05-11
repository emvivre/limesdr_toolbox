FLAGS = -Wall -Werror -lLimeSuite -g -O2

all:
	gcc -o limesdr_dump limesdr_dump.c limesdr_util.c $(FLAGS)
	gcc -o limesdr_replay limesdr_replay.c limesdr_util.c $(FLAGS)
	gcc -o limesdr_stopchannel limesdr_stopchannel.c $(FLAGS)
	gcc -o limesdr_forward limesdr_forward.c limesdr_util.c $(FLAGS)
