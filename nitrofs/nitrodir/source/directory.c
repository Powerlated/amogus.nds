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

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
  //---------------------------------------------------------------------------------
  irqSet(IRQ_VBLANK, Vblank);

  consoleDemoInit();

  iprintf(amogus);

  nitroFSInit(NULL);
  chdir("nitro:/");

  // dirlist("/");
  // dirlist("/");

  // BOTH OF THESE FILES HAVE TO BE THE EXACT SAME LENGTH
  // iprintf("nitroFS inited");
  FILE *amongdripLFile = fopen("amogusdrip_left.signedpcm16", "rb");
  FILE *amongdripRFile = fopen("amogusdrip_right.signedpcm16", "rb");

  // iprintf("amogus2");

  // if (amongdripLFile != NULL && amongdripRFile != NULL) {
  //   iprintf("loaded among drip");
  // } else {
  //   iprintf("failed to load among drip");
  // }

  const uint32_t bufferSize = 131072;
  const uint32_t halfBufferSize = bufferSize / 2;
  uint8_t playingBuf = 0;
  uint8_t *soundBufL = calloc(bufferSize, 1); // 1 MiB;
  uint8_t *soundBufR = calloc(bufferSize, 1); // 1 MiB;
  uint8_t *soundBufLMid = soundBufL + halfBufferSize;
  uint8_t *soundBufRMid = soundBufR + halfBufferSize;

  soundEnable();

  const int halfSampleLengthCycles = 1024 * (halfBufferSize / 2);

  uint32_t targetTime = 0;

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

      playingBuf ^= 1;

      //   iprintf("new buf");
    }

    // print at using ansi escape sequence \x1b[line;columnH
  }

  return 0;
}
