/*---------------------------------------------------------------------------------

        $Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

        Simple console print demo
        -- dovoto


---------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <filesystem.h>
#include <nds.h>
#include <stdint.h>


#include <stdio.h>

volatile int frame = 0;

//---------------------------------------------------------------------------------
void Vblank() {
  //---------------------------------------------------------------------------------
  frame++;
}

const char *amogus = "                    "
                     "            \r\n"
                     "              .****,"
                     "            \r\n"
                     "             **,    "
                     ".           \r\n"
                     "            ,****%%%"
                     "%%%%        \r\n"
                     "        ,,, ,**, ,**"
                     "***         \r\n"
                     "        ,,, ,*******"
                     "***         \r\n"
                     "        ,,, ,,,*****"
                     "**.         \r\n"
                     "         ,, ,,,,,,,,"
                     ",,          \r\n"
                     "            ,,,,   ,"
                     ",,          \r\n"
                     "                    "
                     "            ";

//---------------------------------------------------------------------------------
int main(void) {
  //---------------------------------------------------------------------------------
  irqSet(IRQ_VBLANK, Vblank);

  consoleDemoInit();

  iprintf(amogus);

  nitroFSInit(NULL);
  FILE *amongdripLFile = fopen("amogusdrip_left.signedpcm16", "rb");
  FILE *amongdripRFile = fopen("amogusdrip_right.signedpcm16", "rb");

  //   if (amongdripLFile != NULL && amongdripRFile != NULL) {
  //     iprintf("loaded among drip");
  //   } else {
  //     iprintf("failed to load among drip");
  //   }

  const uint32_t bufferSize = 131072;
  const uint32_t halfBufferSize = bufferSize / 2;
  uint8_t playingBuf = 0;
  uint8_t *soundBufL = calloc(bufferSize, 1); // 1 MiB;
  uint8_t *soundBufR = calloc(bufferSize, 1); // 1 MiB;
  uint8_t *soundBufLMid = soundBufL + halfBufferSize;
  uint8_t *soundBufRMid = soundBufR + halfBufferSize;

  soundEnable();

  const int halfSampleLengthCycles = 1024 * (halfBufferSize / 2);

  uint32_t targetTime = halfSampleLengthCycles / 2;

  fread(soundBufL, halfBufferSize, 1, amongdripLFile);
  fread(soundBufR, halfBufferSize, 1, amongdripRFile);

  soundPlaySample(soundBufL, SoundFormat_16Bit, bufferSize, 32768, 127, 0, true,
                  0);
  soundPlaySample(soundBufR, SoundFormat_16Bit, bufferSize, 32768, 127, 127,
                  true, 0);

  cpuStartTiming(0);
  while (1) {
    if (cpuGetTiming() >= targetTime) {
      targetTime += halfSampleLengthCycles;

      if (playingBuf == 1) {
        fread(soundBufL, halfBufferSize, 1, amongdripLFile);
        fread(soundBufR, halfBufferSize, 1, amongdripRFile);
      } else {
        fread(soundBufLMid, halfBufferSize, 1, amongdripLFile);
        fread(soundBufRMid, halfBufferSize, 1, amongdripRFile);
      }

      playingBuf ^= 1;

    //   iprintf("new buf");
    }

    // print at using ansi escape sequence \x1b[line;columnH
  }

  return 0;
}
