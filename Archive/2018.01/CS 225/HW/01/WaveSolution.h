/* waveSolution.h
** -- my C implementation of the waveSolution API
** cs225 8/18
*/

#ifndef CS225_WAVESOLUTION_H
#define CS225_WAVESOLUTION_H


typedef struct _WaveData *WaveData;


short waveCut6DB(short in, void *data);
short waveBoost3DB(short in, void *data);
short waveBoost(short in, void *data);


WaveData waveRead(const char *fname);

int waveChannelCount(WaveData w);

int waveSamplingRate(WaveData w);

void waveSetFilterData(WaveData w, void *vp);

void waveFilter(WaveData w, int channel, short (*filter)(short,void*));

void waveWrite(WaveData w, const char *filename);

void waveDestroy(WaveData w);


#endif

