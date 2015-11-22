#include "stdafx.h"
#include "FinAlg.h"

using namespace Fin;

// table with predictions
char pcTable[32768U];

FinAlg::FinAlg(void)
{
}

void Compress (FILE *pfIn, FILE *pfOut)
{
   int c;                // character
   int i;                // loop counter
   char p1=0, p2=0;      // previous 2 characters
   char buf[8];          // keeps characters temporarily
   int ctr=0;            // number of characters in mask
   int bctr=0;           // position in buf
   unsigned char mask=0; // mask to mark successful predictions

   memset (pcTable, 32, 32768U); // space (ASCII 32) is the most used char

   c = fgetc (pfIn);
   while (c!=EOF)
   {
      // try to predict the next character
      if (pcTable[INDEX(p1,p2)]==(char)c)
	  {
         // correct prediction, mark bit for correct prediction
         mask = mask ^ (1<<ctr);
      } 
	  else 
	  {
         // wrong prediction, but next time ...
         pcTable[INDEX(p1,p2)]=(char)c;

         // buf keeps character temporarily in buffer
         buf[bctr++] = (char)c;
      }

      // test if mask is full (8 characters read)
      if (++ctr==8)
	  {
         // write mask
         fputc ((char)mask, pfOut);

         // write kept characters
         for (i=0;i<bctr;i++)
            fputc (buf[i], pfOut);

         // reset variables
         ctr=0;
         bctr=0;
         mask=0;
      }

      // shift characters
      p1 = p2; 
	  p2 = (char)c;

      c = fgetc (pfIn);
   }

   // EOF, but there might be some left for output
   if (ctr)
   {
      // write mask
      fputc ((char)mask, pfOut);

      // write kept characters
      for (i=0;i<bctr;i++)
         fputc (buf[i], pfOut);
   }
}

void Decompress (FILE *pfin, FILE *pfout)
{
   int ci,co;            // characters (in and out)
   char p1=0, p2=0;      // previous 2 characters
   int ctr=8;            // number of characters processed for this mask
   unsigned char mask=0; // mask to mark successful predictions

   memset (pcTable, 32, 32768U); // space (ASCII 32) is the most used char

   ci = fgetc (pfin);
   while (ci!=EOF)
   {
      // get mask (for 8 characters)
      mask = (unsigned char)(char)ci;

      // for each bit in the mask
      for (ctr=0; ctr<8; ctr++)
	  {
         if (mask & (1<<ctr))
		 {
            // predicted character
            co = pcTable[INDEX(p1,p2)];
         } 
		 else 
		 {
            // not predicted character
            co = fgetc (pfin);
            if (co==EOF) 
				return; // decompression completed !
			pcTable[INDEX(p1,p2)] = (char)co;
         }
         fputc (co, pfout);
         p1 = p2; 
		 p2 = co;
      }
      ci = fgetc (pfin);
   }
}

///* test program by compressing and decompressing a file */
//void main (){
//   FILE *a = fopen ("in","rb");
//   FILE *b = fopen ("out","wb");
//   Compress (a,b);
//   fclose (a);
//   fclose (b);
//
//   a = fopen ("out","rb");
//   b = fopen ("rin","wb");
//   Decompress (a, b);
//   fclose (a);
//   fclose (b);
//}
