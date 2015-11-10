#include "SplayAlg.h"

using namespace Splay;

SplayAlg::SplayAlg(void)
{
	//MessageBox(0,_T("Not found"), _T("Error"),  MB_OK |MB_ICONERROR);
}

/* initialize the splay tree - as a balanced tree */
void SplayAlg::InitializeSplay(void)
{
    DownIndex I;
    int /*UpIndex*/ J;
    DownIndex K;
    
    for (I = 1; I <= TwiceMax; I++)
        Up[I] = (I - 1) >> 1;
    for (J = 0; J <= PredMax; J++)
    {
        K = ((byte)J + 1) << 1;
        Left[J] = K - 1;
        Right[J] = K;
    }
}

/* read a compressed file header */
void SplayAlg::ReadHeader(void)
{
    FileHeader Header;
    
    fread(&Header, sizeof(FileHeader), 1, InF);
    if (Header.Signature != Sig)
    {
        printf("Unrecognized file format!\n");
    }
}

/* return next byte from compressed input */
byte SplayAlg::GetByte(void)
{
    Index++;
    if (Index > InSize)
    {
        /* reload file buffer */
        InSize = fread(InBuffer+1, sizeof(byte), BufSize, InF);
        Index = 1;
        /* end of file handled by special marker in compressed file */
    }
    
    /* get next byte from buffer */
    return InBuffer[Index];
}

/* return next char from compressed input */
CodeType SplayAlg::Expand(void)
{
    DownIndex A;
    
    /* scan the tree to a leaf, which determines the character */
    A = Root;
    do
    {
        if (BitPos == 7)
        {
            /* used up bits in current byte, get another */
            InByte = GetByte();
            BitPos = 0;
        }
        else
            BitPos++;
        
        if ((InByte & BitMask[BitPos]) == 0)
            A = Left[A];
        else
            A = Right[A];
    } while (A <= PredMax);
    
    /* Update the code tree */
    A -= MaxChar;
    Splay(A);
    
    /* return the character */
    return A;
}

/* uncompress the file and write output */
void SplayAlg::ExpandFile(void)
{
    /* force buffer load first time */
    Index = 0;
    InSize = 0;
    /* nothing in output buffer */
    OutSize = 0;
    /* force bit buffer load first time */
    BitPos = 7;
    
    /* read and expand the compressed input */
    OutByte = Expand();
    while (OutByte != EofChar)
    {
        WriteByte();
        OutByte = Expand();
    }
    
    /* flush the output buffer */
    FlushOutBuffer();
}

void SplayAlg::mainSplay(int count, char *args[])
{

    if (count < 3)
    {
        printf(Usage);
    }
    
    if (count == 4 && (strlen(args[1]) == 1) && toupper(args[1][0]) == 'X')
    {
        strcpy(InName, args[2]);
        strcpy(OutName, args[3]);
        CompressFlag = false;
    }
    else
    {
        if (count == 4)
        {
            printf(Usage);
        }
        CompressFlag = true;
        strcpy(InName, args[1]);
        strcpy(OutName, args[2]);
    }
        
    InitializeSplay();
    
    if ((InF = fopen(InName, "rb")) == NULL)
    {
        printf("Unable to open input file: %s\n", InName);
    }
    if ((OutF = fopen(OutName, "wb")) == NULL)
    {
        printf("Unable to open output file: %s\n", OutName);
    }
    
    if (CompressFlag)
        CompressFile();
    else
    {
        ReadHeader();
        ExpandFile();
    }
    
    fclose(InF);
    fclose(OutF);
}