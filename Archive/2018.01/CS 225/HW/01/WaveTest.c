
/* WaveTest.c
** -- simple driver for WAVE file functions
** cs225 8/18
*/
#include <stdio.h>
#include <math.h>
#include "Wave.h"


#define TWOPI 6.283185307


/* ring modulator filter data */
struct RingModulatorData {
  double argument;
  double argument_inc;
  float mix;
};

/* initialize ring modulator filter data */
void ringModulatorInit(struct RingModulatorData *rmd, float f, float R, float m) {
  rmd->argument = 0.0;
  rmd->argument_inc = TWOPI * f / R;
  rmd->mix = m;
}
unsigned test = 0;
/* ring modulator filter function */
short ringModulator(short v, void *vp) {
  test++;
  struct RingModulatorData *rmd = (struct RingModulatorData*)vp;
  float x = (float)v,
        y = (1.0f-rmd->mix)*x + rmd->mix*(float)sin(rmd->argument)*x;
  rmd->argument += rmd->argument_inc;
  return (short)y;
}


int main(int argc, char *argv[]) {
  WaveData wave_data = NULL;
  struct RingModulatorData rmd;
  unsigned short bd;
  char file[] = "C:\\Users\\Cody\\Documents\\SVN\\18 Fall\\CS 225\\HW\\01\\Wave1.wav";

  ///* exactly one command line argument is expected */
  //if (argc != 2) {
  //  printf("*** name of a 16-bit WAVE file is expected ***\n");
  //  return 0;
  //}

  /* attempt to read the WAVE file */

  wave_data = waveRead(file);

  if (wave_data == NULL) {
    printf("*** not a valid 16-bit WAVE file ***\n%s", argv[1]);
    return -1;
  }

  /* filter channel 0 with ring modulator */
  ringModulatorInit(&rmd,350.0f,(float)waveSamplingRate(wave_data),0.5f);
  waveSetFilterData(wave_data,&rmd);
  waveFilter(wave_data,0,ringModulator);

  /* scale channel 0 by a factor of 1.2 (with clipping) */
  bd = waveBoostData(1.2f);
  waveSetFilterData(wave_data,&bd);
  PrintData(wave_data);
  waveFilter(wave_data,0,waveBoost);
  PrintData(wave_data);

  /* scale channel 0 by a factor of 0.5 */
  waveFilter(wave_data,0,waveCut6DB);
  PrintData(wave_data);

  /* if present, scale channel 1 by a factor of 1.41 (with clipping) */
  if (waveChannelCount(wave_data) == 2)
    waveFilter(wave_data,1,waveBoost3DB);

  PrintData(wave_data);
  waveWrite(wave_data,"WaveTest2.wav");

  waveDestroy(wave_data);
  return 0;
}

