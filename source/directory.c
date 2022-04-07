/*---------------------------------------------------------------------------------

        $Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

        Simple console print demo
        -- dovoto


---------------------------------------------------------------------------------*/
#include <dirent.h>
#include <fat.h>
#include <filesystem.h>
#include <nds.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
void dirlist(const char *path) {
  //---------------------------------------------------------------------------------

  DIR *pdir = opendir(path);

  if (pdir != NULL) {

    while (true) {
      struct dirent *pent = readdir(pdir);
      if (pent == NULL)
        break;

      if (strcmp(".", pent->d_name) != 0 && strcmp("..", pent->d_name) != 0) {
        if (pent->d_type == DT_DIR) {
          printf("%s/%s <DIR>\n", (strcmp("/", path) == 0) ? "" : path,
                 pent->d_name);
          char *dnbuf = (char *)malloc(strlen(pent->d_name) + strlen(path) + 2);
          sprintf(dnbuf, "%s/%s", (strcmp("/", path) == 0) ? "" : path,
                  pent->d_name);
          dirlist(dnbuf);
          free(dnbuf);
        } else {
          printf("%s/%s\n", (strcmp("/", path) == 0) ? "" : path, pent->d_name);
        }
      }
    }

    closedir(pdir);
  } else {
    printf("opendir() failure.\n");
  }
}

uint8_t playingBuf = 0;
const uint32_t bufferSize = 32768;
const uint32_t halfBufferSize = bufferSize / 2;
const uint32_t halfSampleLengthCycles = 1024 * (halfBufferSize / 2);
uint8_t *soundBufL;
uint8_t *soundBufR;
uint8_t *soundBufLMid;
uint8_t *soundBufRMid;

FILE *amongdripLFile;
FILE *amongdripRFile;

void timerCallback() {
  // iprintf("callback\n");

  playingBuf ^= 1;

  uint32_t elementsRead;
  if (playingBuf == 1) {
    fread(soundBufL, 1, halfBufferSize, amongdripLFile);
    elementsRead = fread(soundBufR, 1, halfBufferSize, amongdripRFile);
  } else {
    fread(soundBufLMid, 1, halfBufferSize, amongdripLFile);
    elementsRead = fread(soundBufRMid, 1, halfBufferSize, amongdripRFile);
  }

  if (elementsRead < halfBufferSize) {
    for (uint32_t i = elementsRead; i < halfBufferSize; i++) {
      if (playingBuf == 1) {
        soundBufL[i] = 0;
        soundBufR[i] = 0;
      } else {
        soundBufLMid[i] = 0;
        soundBufRMid[i] = 0;
      }
    }

    rewind(amongdripLFile);
    rewind(amongdripRFile);
  }
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
  //---------------------------------------------------------------------------------
  irqSet(IRQ_VBLANK, Vblank);

  consoleDemoInit();

  nitroFSInit(NULL);
  chdir("nitro:/");

  // dirlist("/");
  // dirlist("/");

  // BOTH OF THESE FILES HAVE TO BE THE EXACT SAME LENGTH
  // iprintf("nitroFS inited");
  amongdripLFile = fopen("amogusdrip_left.signedpcm16", "rb");
  amongdripRFile = fopen("amogusdrip_right.signedpcm16", "rb");

  // iprintf("amogus2");

  // if (amongdripLFile != NULL && amongdripRFile != NULL) {
  //   iprintf("loaded among drip");
  // } else {
  //   iprintf("failed to load among drip");
  // }

  soundBufL = calloc(bufferSize, 1); // 1 MiB;
  soundBufR = calloc(bufferSize, 1); // 1 MiB;
  soundBufLMid = soundBufL + halfBufferSize;
  soundBufRMid = soundBufR + halfBufferSize;

  soundEnable();

  // iprintf("Allocated %ld bytes", bufferSize * 2);
  iprintf(amogus);

  fread(soundBufLMid, halfBufferSize, 1, amongdripLFile);
  fread(soundBufRMid, halfBufferSize, 1, amongdripRFile);


  soundPlaySample(soundBufL, SoundFormat_16Bit, bufferSize, 32768, 127, 0, true,
                  0);
  soundPlaySample(soundBufR, SoundFormat_16Bit, bufferSize, 32768, 127, 127,
                  true, 0);

  timerStart(0, ClockDivider_256, halfSampleLengthCycles / 256, timerCallback);


  while (1) {
    swiWaitForVBlank();
  }

  return 0;
}
