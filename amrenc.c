/*
 * ===================================================================
 *  TS 26.104
 *  REL-5 V5.4.0 2004-03
 *  REL-6 V6.1.0 2004-03
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/*
 * encoder.c
 *
 *
 * Project:
 *    AMR Floating-Point Codec
 *
 * Contains:
 *    Speech encoder main program
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "typedef.h"
#include "interf_enc.h"

#ifndef ETSI
#ifndef IF2
#define AMR_MAGIC_NUMBER "#!AMR\n"
#endif
#endif

static void Usage(char* name)
{
   fprintf (stderr,
      "Usage of %s:\n\n"
      "\tinfile.wav outfile.amr\n\n",
      name);
}
void Copyright(void){
fprintf (stderr,
"===================================================================\n"
" 3GPP AMR Floating-point Speech Encoder  REL-7 V7.0.0 2007-06      \n"
"===================================================================\n"
);
}
/*
 * main
 *
 *
 * Function:
 *    Speech encoder main program
 *
 *    Usage: encoder speech_file bitstream_file mode dtx mode_file
 *
 *    Format for speech_file:
 *       Speech is read from a binary file of 16 bits data.
 *
 *    Format for ETSI bitstream file:
 *       1 word (2-byte) for the TX frame type
 *       244 words (2-byte) containing 244 bits.
 *          Bit 0 = 0x0000 and Bit 1 = 0x0001
 *       1 word (2-byte) for the mode indication
 *       4 words for future use, currently written as zero
 *
 *    Format for 3GPP bitstream file:
 *       Holds mode information and bits packed to octets.
 *       Size is from 1 byte to 31 bytes.
 *
 *    ETSI bitstream file format is defined using ETSI as preprocessor
 *    definition
 *
 *    mode        : MR475, MR515, MR59, MR67, MR74, MR795, MR102, MR122
 *    mode_file   : reads mode information from a file
 * Returns:
 *    0
 */
int main (int argc, char * argv[]){

   /* file strucrures */
   FILE * file_speech = NULL;
   FILE * file_encoded = NULL;

   /* input speech vector */
   short speech[160];
   unsigned char wav[44];

   /* counters */
   int byte_counter, frames = 0, bytes = 0, allframes, oldpercent = -1;

   /* pointer to encoder state structure */
   int *enstate;

   /* requested mode */
   enum Mode req_mode = MR122;

   /* bitstream filetype */
   unsigned char serial_data[32];

   /* Process command line options */

   if (argc == 3){
      if (!strcmp(argv[argc - 1], "-"))
         file_encoded = stdout;
      else
         file_encoded = fopen(argv[argc - 1], "wb");
      if (file_encoded == NULL){
         Usage(argv[0]);
         return 1;
      }
      if (!strcmp(argv[argc - 2], "-"))
         file_speech = stdin;
      else
         file_speech = fopen(argv[argc - 2], "rb");
      if (file_speech == NULL){
         fclose(file_encoded);
         Usage(argv[0]);
         return 1;
      }
   }
   else {
      Usage(argv[0]);
      return 1;
   }


   enstate = Encoder_Interface_init(0);

   Copyright();
   	bytes = fwrite(AMR_MAGIC_NUMBER, sizeof(char), strlen(AMR_MAGIC_NUMBER), file_encoded);

   /* read WAV header*/
   fread(wav, 1, 44, file_speech);
   if(wav[0] != 'R' || wav[1] != 'I' || wav[2] != 'F' || wav[3] != 'F') {
       fprintf(stderr, "Invalid WAV file: %s", argv[1]);
       exit(0);
   }

   allframes = (((wav[4] | (wav[5] << 8) | (wav[6] << 16) | (wav[7] << 24)) - 8) / 320 + 1);

   /* read file */
   while (fread( speech, sizeof (Word16), 160, file_speech ) > 0)
   {
      int newpercent;
      /* call encoder */
      byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, serial_data, 0);

      bytes += byte_counter;
      fwrite(serial_data, sizeof (UWord8), byte_counter, file_encoded );
      fflush(file_encoded);

      frames++;
      newpercent = frames * 100 / allframes;
      if(oldpercent != newpercent) {
          oldpercent = newpercent;
          fprintf(stderr, "\r[%d%%]", newpercent);
          fflush(stderr);
      }
   }
   Encoder_Interface_exit(enstate);
   fprintf(stderr, "\n");
   fclose(file_speech);
   fclose(file_encoded);

   return 0;
}
