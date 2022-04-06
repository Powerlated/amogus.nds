/*---------------------------------------------------------------------------------

        $Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

        Simple console print demo
        -- dovoto


---------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>


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
  touchPosition touchXY;

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

  const int bufferSize = 131072;
  uint8_t *soundBufL = malloc(bufferSize); // 1 MiB;
  uint8_t *soundBufR = malloc(bufferSize); // 1 MiB;

  fread(soundBufL, bufferSize, 1, amongdripLFile);
  fread(soundBufR, bufferSize, 1, amongdripRFile);

  soundEnable();
  soundPlaySample(soundBufL, SoundFormat_16Bit, bufferSize, 32768, 127, 0,
                  false, 0);
  soundPlaySample(soundBufR, SoundFormat_16Bit, bufferSize, 32768, 127, 127,
                  false, 0);

  while (1) {

    swiWaitForVBlank();
    scanKeys();
    int keys = keysDown();
    if (keys & KEY_START)
      break;

    touchRead(&touchXY);

    // print at using ansi escape sequence \x1b[line;columnH
  }

  return 0;
}
