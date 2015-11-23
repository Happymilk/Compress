#include "stdafx.h"
#include "FinAlg.h"

using namespace Fin;

char pcTable[32768U];// table with predictions

FinAlg::FinAlg(void)
{
}

void FinAlg::Compress(FILE *pfIn, FILE *pfOut)
{
   int c;                // character
   int i;                // loop counter
   char p1=0, p2=0;      // previous 2 characters
   char buf[8];          // keeps characters temporarily
   int ctr=0;            // number of characters in mask
   int bctr=0;           // position in buf
   unsigned char mask=0; // mask to mark successful predictions

   memset(pcTable, 32, 32768U); // space (ASCII 32) is the most used char

   c = fgetc(pfIn);
   while (c!=EOF)
   {
      if (pcTable[INDEX(p1,p2)]==(char)c)// try to predict the next character
         mask = mask ^ (1<<ctr);// correct prediction, mark bit for correct prediction
	  else 
	  {
         pcTable[INDEX(p1,p2)]=(char)c;// wrong prediction, but next time ...

         buf[bctr++] = (char)c; // buf keeps character temporarily in buffer
      }

      if (++ctr==8)// test if mask is full (8 characters read)
	  {
         fputc ((char)mask, pfOut);// write mask

         for (i=0;i<bctr;i++)// write kept characters
            fputc (buf[i], pfOut);

         ctr=0;
         bctr=0;
         mask=0;
      }

      //shift characters
      p1 = p2; 
	  p2 = (char)c;

      c = fgetc(pfIn);
   }

   if (ctr)// EOF, but there might be some left for output
   {
      fputc ((char)mask, pfOut);// write mask

      for (i=0;i<bctr;i++)// write kept characters
         fputc (buf[i], pfOut);
   }
}

void FinAlg::Decompress(FILE *pfin, FILE *pfout)
{
   int ci,co;            // characters (in and out)
   char p1=0, p2=0;      // previous 2 characters
   int ctr=8;            // number of characters processed for this mask
   unsigned char mask=0; // mask to mark successful predictions

   memset (pcTable, 32, 32768U); // space (ASCII 32) is the most used char

   ci = fgetc (pfin);
   while (ci!=EOF)
   {      
      mask = (unsigned char)(char)ci;// get mask (for 8 characters)

      for (ctr=0; ctr<8; ctr++)// for each bit in the mask
	  {
         if (mask & (1<<ctr))
            co = pcTable[INDEX(p1,p2)];// predicted character
		 else 
		 {
            co = fgetc (pfin);// not predicted character
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
