/* Wave.template.c
** -- template for homework assignment #1
** cs225
*/

#include "Wave.h"
#include "WaveSolution.h"


struct _WaveData
{
  int channel_count;
  int frame_count;
  int sampling_rate;
  void *filter_data;
  short *data16;

  unsigned dataSize;
  unsigned ignoreUintAt28;
  unsigned short ignoreUshAt32;
};
#define HEADER_SIZE 44

short waveCut6DB(short in, void *data) {
  return waveSolutionCut6DB(in,data);
}


short waveBoost3DB(short in, void *data) {
  return waveSolutionBoost3DB(in,data);
}


unsigned short waveBoostData(float gain) {
  int i;
  unsigned short bd = 0,
                 mask = 0x01;
  for (i=0; i < 16; ++i) {
    if (gain >= 1) {
      bd |= mask;
      gain -= 1.0f;
    }
    mask <<= 1;
    gain *= 2.0f;
  }
  return bd;
}


short waveBoost(short in, void *data) {
  return waveSolutionBoost(in,data);
}



WaveData waveRead(const char *fname) {
  return waveSolutionRead(fname);
}


int waveChannelCount(WaveData w) {
  return waveSolutionChannelCount(w);
}


int waveSamplingRate(WaveData w) {
  return waveSolutionSamplingRate(w);
}


void waveSetFilterData(WaveData w, void *vp) {
  waveSolutionSetFilterData(w,vp);
}


void waveFilter(WaveData w, int channel, short (*filter)(short,void*)) {
  
  waveSolutionFilter(w,channel,filter);
}


void waveWrite(WaveData w, const char *fname) {
  waveSolutionWrite(w,fname);
}


void waveDestroy(WaveData w) {
  waveSolutionDestroy(w);
}


unsigned PrintData(WaveData wave)
{
  for (int i = 0; i < 128; i++)
  {
    printf("%.3i : %#010x %i\n", (i * 2) + HEADER_SIZE, wave->data16[i], wave->data16[i]);
  }
}
