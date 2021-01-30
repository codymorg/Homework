/* Wave.template.c
 * Cody Morgan
 * Homework 1
 * 19 SEP 2018
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Wave.h"
#pragma warning(disable:4996)

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

typedef struct Header
{
  char riff[4];
  unsigned data36;
  char wave[4];
  char fmt[4];
  unsigned uInt16;
  unsigned short uShort1;
  unsigned short channelCount;
  unsigned sampling_rate;
  unsigned ignoreUintAt28;
  unsigned short ignoreUshAt32;
  unsigned short uShortSixteen;
  char data[4];
  unsigned datasize;
} Header;

#define HEADER_SIZE 44 //the size in Bytes of the header 

/******************************************************************************
Function: isWave16

Description: validates a wave file and returns the size of the data

Inputs: fname - the name of the file

Outputs: the size of the wave data
******************************************************************************/
unsigned int isWave16(const char *fname, FILE* file)
{
  // #1 check: file esists
  if (file)
  {
    // #2 : WAVE marker at index 8
    char buffer[4] = { 0 };
    fseek(file, 8, SEEK_SET);
    fread(buffer, sizeof(char), 4, file);
    if (strcmp(buffer, "WAVE"))
    {
      // #3 : 16 found at index 34
      fseek(file, 34, SEEK_SET);
      unsigned short sixteen = 0;
      fread(&sixteen, sizeof(unsigned short), 1, file);
      if (sixteen == 16)
      {
        fseek(file, 40, SEEK_SET);
        unsigned size = 0;
        fread(&size, 1, sizeof(unsigned), file);

        fseek(file, 0, SEEK_END);
        unsigned actualSize = ftell(file) - HEADER_SIZE;
        if (actualSize == size)
        {
          // #4 : valid data size
          return size;
        }
      }
    }
  }
  else
  {
    printf("no file pointer given to wave 16");
  }
  return 0;
}

/******************************************************************************
Function: waveRead 

Description: fills in the WaveData struct for wave files

Inputs: fname - the name of the file

Outputs: the WaveData stuct
******************************************************************************/
WaveData waveRead(const char *fname) 
{
  FILE* file;
  int error = fopen_s(&file, fname, "rb");
  Header header;

  if (file)
  {
    int size = sizeof(struct _WaveData);
    WaveData wave = (WaveData)calloc(1,size);
    wave->dataSize = isWave16(fname, file);

    fseek(file, 0, SEEK_SET);
    fread(&header, HEADER_SIZE, 1, file);
    wave->channel_count = header.channelCount;
    wave->frame_count = header.datasize / 2;
    wave->dataSize = header.datasize;
    wave->sampling_rate = header.sampling_rate;

    wave->data16 = (short*)calloc(1, header.datasize + 2);
    fread(&wave->data16, header.datasize, 1, file);

    PrintData(wave);

    fclose(file);
    return wave;
  }
  else
  {
    printf("cant read from file error %i\n", error);
    return NULL;
  }
}

int waveChannelCount(WaveData w)
{
  if (w)
    return w->channel_count;
  else
    printf("bad wave ch count\n");
  return 0;
}

int waveSamplingRate(WaveData w) 
{
  if (w)
    return w->sampling_rate;
  else
    printf("bad wave sampling data\n");
  return 0;
}

void waveSetFilterData(WaveData w, void *vp) 
{
  if (w)
    w->filter_data = vp;
  else
    printf("bad filter data\n");
}

void waveFilter(WaveData w, int channel, short (*filter)(short,void*)) 
{
  for (int i = 0; i < w->frame_count; i++)
  {
    if (w->channel_count > 1 && ( i % 2 == channel))
    {
      w->data16[i] = filter(w->data16[i], w->filter_data);
    }
    else if(w->channel_count == 1)
    {
      w->data16[i] = filter(w->data16[i], w->filter_data);
    }
  }
}

void waveWrite(WaveData w, const char *fname) 
{
  FILE* file;
  file = fopen(fname, "wb");
  Header header;

  if (file)
  {

    strncpy_s(header.riff, 5, "RIFF", 4);
    header.data36 = 36 + w->dataSize;
    strncpy_s(header.wave, 5, "WAVE", 4);
    strncpy_s(header.fmt, 5, "fmt ", 4);
    header.uInt16 = (unsigned)16;
    header.uShort1 = (unsigned short)1;
    header.channelCount = w->channel_count;
    header.sampling_rate = w->sampling_rate;
    header.ignoreUintAt28 = 2 * w->channel_count*w->sampling_rate;// w->ignoreUintAt28;
    header.ignoreUshAt32 = 2 * w->channel_count;// w->ignoreUshAt32;
    header.uShortSixteen = (unsigned short)16;
    strncpy_s(header.data, 5, "data", 4);
    header.datasize = w->dataSize;
    printf("%d %d %d\n", w->channel_count, w->sampling_rate, w->dataSize);


    fwrite(&header, sizeof(header), 1, file);
    fwrite(w->data16, 1, w->dataSize, file);
    fclose(file);
  }
  else
  {
    printf("can't open file for writing error %i");
  }
}

void waveDestroy(WaveData w)
{
  free(w);
}

short waveCut6DB(short in, void *data) 
{
  //returns the result of scaling the value of in by 0.5 via bitwise shift
  return in >> 1;
}

short waveBoost3DB(short in, void *data) 
{
  //multiply in by 1.40625, apx 1.41
  int inT = in;
  int boosted = (inT + (inT >> 2) + (inT >> 3) + (inT >> 5));
  if (boosted > SHRT_MAX)
    boosted = SHRT_MAX;
  else if (boosted < SHRT_MIN)
    boosted = SHRT_MIN;

  return (short)boosted;
}

unsigned short waveBoostData(float gain) 
{
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

short waveBoost(short in, void *data)
{
  unsigned short bytePair = *((short*)data);
  unsigned mask = 0x1;
  unsigned bit = 0x0;
  int value = 0;

  for (int i = 0; i < sizeof(bytePair) * 8; i++)
  {
    bit = bytePair & mask;
    bit >>= i;

    if (bit)
      value += (in >> i);
    mask <<= 1;
  }

  if (value > SHRT_MAX)
    value = SHRT_MAX;
  else if (value < SHRT_MIN)
    value = SHRT_MIN;

  return value;
}

unsigned PrintData(WaveData wave)
{
  for (int i = 0; i < 128; i++)
  {
    printf("%.3i : %#010x %i\n", (i * 2) + HEADER_SIZE, wave->data16[i], wave->data16[i]);
  }
}