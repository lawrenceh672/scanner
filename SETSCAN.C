#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "conio.h"
#include "time.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "sys\stat.h"
#include "sys\types.h"

#include "host.h"   //make sure that the preprocessor settings for the include files
#include "scanner.h"//have the correct drive letter

typedef struct {
int Xpos;
int Ypos;
int Xext;
int Yext;
int Xres;
int Yres;
int Xscale;
int Yscale;
int Pixbit;
int Dtype;
int compresion; //0 none, 1 RLE, 2 JPG
char defpath[60];
char bitmapo[100];
char bitrleo[100];
char bitjpgo[100];
} SetUpStruct;


void CheckError( void );
int  DoError( int );
void ScannerInfo();
void ScanAsBitMap( char *filename);
void Bmp2Jpg( char *inputname, char *outputname);
void RLEEncode( char *inputname, char *outputname);
void SetScanner(int Xpos, int Ypos, int Xext, int Yext, int Xres, int Yres, int Xscale, int Yscale, int Pixbit, int Dtype, char *name);
void LoadDefaults( SetUpStruct *sus );
void SetDefaults( SetUpStruct *sus );
void Orientate(char *name, char *defpth);
int WhatError(int ErrorType);

UINT8 GetNib(int byte, UINT8 *buffer);
UINT8 ComposeNib(UINT8 high, UINT8 low);
UINT8 Even(UINT8 ch);

#define    NAMEBUFFER 40
#define    HIGH 1
#define    LOW 0
#define    TRUE 1
#define    FALSE 0

SetUpStruct sus;

void main( int argc, char *argv[])
{

printf("Argument Count: %d\n", argc);
strcat(&sus.defpath[0], "d:\\ibmcppw\\working\\autoscan\\");
printf("Default Path: %s\n", &sus.defpath[0]);

if(argc != 12)
   LoadDefaults( &sus );
else
   {
   SetScanner( atoi(argv[1]), atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6]),atoi(argv[7]),atoi(argv[8]),atoi(argv[9]),atoi(argv[10]), argv[11]);
   SetDefaults( &sus );
   }

//ScannerInfo( &ScanState );                   //Output the scanner Stats
ScanAsBitMap(&sus.bitmapo[0]);//Output the scan to a bitmap file
Orientate( &sus.bitmapo[0], &sus.defpath[0] );
RLEEncode( &sus.bitmapo[0], &sus.bitrleo[0]);
exit(0);
}

int WhatError(ErrorType)
int ErrorType;
{
fprintf(stdout,"\nProgram Error: %d.",ErrorType);
switch(ErrorType)
   {/* Communication Errors */
    case SL_NOACCESS     : fprintf(stdout,"  No Access\n"); break;
    case SL_BADACCESS    : fprintf(stdout,"  Bad Access\n"); break;
    case SL_BADCOMMAND   : fprintf(stdout,"  Bad Command\n"); break;
    case SL_BADRESPONSE  : fprintf(stdout,"  Bad Response\n"); break;
    case SL_NULLRESPONSE : fprintf(stdout,"  Null Response\n"); break;
    case SL_BUSY         : fprintf(stdout,"  Scanner Busy\n"); break;
    /* Scanner Error */
    case SL_BADCMDFORMAT : fprintf(stdout,"  Bad Command Format\n"); break;
    case SL_ILLEGALCMD   : fprintf(stdout,"  Illegal Command Format\n"); break;
    case SL_ILLEGALWND   : fprintf(stdout,"  Illegal Window Size\n"); break;
    case SL_PARAMERROR   : fprintf(stdout,"  Parameter Format\n"); break;
    case SL_SCALEERROR   : fprintf(stdout,"  Scaling Error\n"); break;
    case SL_DITHERERROR  : fprintf(stdout,"  Ditther Error\n"); break;
    case SL_TONEMAPERROR : fprintf(stdout,"  Tone Map Error\n"); break;
    case SL_LAMPERROR    : fprintf(stdout,"  Lamp Error\n"); break;
    case SL_ADFJAM       : fprintf(stdout,"  ADF Jam\n"); break;
    case SL_HOMEMISSING  : fprintf(stdout,"  Home Missing\n"); break;
    case SL_SCANERROR    : fprintf(stdout,"  Scan Error\n"); break;
    case SL_MATRIXERROR  : fprintf(stdout,"  Matrix Error\n"); break;
    case SL_ADFEMPTY     : fprintf(stdout,"  ADF Empty\n"); break;
   }
return 0;
}

void ScannerInfo( )
{
FILE *Fhdr;
SCANSTATE ScanState;

/* open header file, write info */
if (( Fhdr=fopen("scan.hdr","w+")) == NULL)
   {
   printf("\nError opening scanner header file.");
   exit(1);
   }

fprintf(Fhdr,"\nScanner Stats:");
fprintf(Fhdr,"\n  DataType       %6d",ScanState.DataType);
fprintf(Fhdr,"\n  PixelBits      %6d",ScanState.PixelBits);
fprintf(Fhdr,"\n  X Resolution   %6d",ScanState.Xresolution);
fprintf(Fhdr,"\n  Y Resolution   %6d",ScanState.Yresolution);
fprintf(Fhdr,"\n  X org          %6d",ScanState.Window.X);
fprintf(Fhdr,"\n  Y org          %6d",ScanState.Window.Y);
fprintf(Fhdr,"\n  X extent       %6d",ScanState.Window.Xextent);
fprintf(Fhdr,"\n  Y extent       %6d",ScanState.Window.Yextent);
switch(ScanState.Info.Model)
   {
   case  SL_HP9190A: fprintf(stdout,"\nScanner:    %s","ScanJet"); break;
   case  SL_HP9195A: fprintf(stdout,"\nScanner:    %s","ScanJet +"); break;
   case  SL_HP1750A: fprintf(stdout,"\nScanner:    %s","ScanJet IIc"); break;
   case  SL_HP1790A: fprintf(stdout,"\nScanner:    %s","ScanJet IIp"); break;
   case  SL_HP2500A: fprintf(stdout,"\nScanner:    %s","ScanJet IIcx"); break;
   case  SL_HP2520A: fprintf(stdout,"\nScanner:    %s","ScanJet 3c"); break;
   case  SL_HP2570A: fprintf(stdout,"\nScanner:    %s","ScanJet 3p"); break;
   case  SL_HP1130A: fprintf(stdout,"\nScanner:    %s","ScanJet 4p"); break;
   case  SL_HP5110A: fprintf(stdout,"\nScanner:    %s","ScanJet 5p"); break;
   case  SL_HP5190A: fprintf(stdout,"\nScanner:    %s","ScanJet 5100C"); break;
   case  SL_HP6290A: fprintf(stdout,"\nScanner:    %s","ScanJet 4100C"); break;
   case  SL_HP6270A: fprintf(Fhdr,"\nScanner:    %s","ScanJet 6200C"); break;
   case  SL_HP7190A: fprintf(stdout,"\nScanner:    %s","ScanJet 5200C"); break;
   default: fprintf(Fhdr,"\nScanner:    %s","Unknown"); break;
   }
fprintf(Fhdr,"\nImage Data:");
fprintf(Fhdr,"\n  Width In Pixels  %7d",ScanState.Info.WidthPixels);
fprintf(Fhdr,"\n  Width In Bytes   %7d",ScanState.Info.WidthBytes);
fprintf(Fhdr,"\n  Length In Lines  %7d",ScanState.Info.Lines);
fprintf(Fhdr,"\n  Data Size        %7ld",ScanState.Info.Size);
fclose(Fhdr);
}

void ScanAsBitMap(char *filename)
{
RGBQUAD *aColors;
int loop;
INT16 i, status, extra, colors, counter;
INT32 received, bytesleft, bufferout;
unsigned char ch;
UINT8 *Buffer;
BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
SCANSTATE ScanState;
FILE *fp;
GetScanState(&ScanState, SL_NOM, SL_PIXEL);

//Set up the Color Palette
if(ScanState.PixelBits == 1 || ScanState.PixelBits == 4 || ScanState.PixelBits == 8)
   {
   colors = pow(2, ScanState.PixelBits);
   counter = colors - 1; //The highest color value will be the highest in the color palette (white is 1 etc...)
   aColors = (RGBQUAD *)malloc(sizeof(RGBQUAD) * colors);
   for(loop = 0; loop < 256; loop= loop + (256/colors))
      {
   aColors[counter].rgbBlue = loop;  //Scale from white to black
   aColors[counter].rgbGreen = loop;
   aColors[counter].rgbRed = loop;
   aColors[counter].rgbReserved = 0;
   counter--;
      }
   aColors[0].rgbBlue = 255;  //Put a white at the top of the palette
   aColors[0].rgbGreen = 255;
   aColors[0].rgbRed = 255;
   aColors[0].rgbReserved = 0;
   }
else
   {
   aColors = NULL; //for the 24 bit color case, there is no color palette.
   SetMatrix(0); //sRGB
   }

//output the header file info for the Bitmap
bmfh.bfType = 19778;
bmfh.bfSize = ScanState.Info.Size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + _msize((void *)aColors);
bmfh.bfReserved1 = 0;
bmfh.bfReserved2 = 0;
bmfh.bfOffBits  = bmfh.bfSize - ScanState.Info.Size;

bmih.biSize = sizeof(BITMAPINFOHEADER);
bmih.biWidth = ScanState.Info.WidthPixels;
bmih.biHeight = ScanState.Info.Lines;
bmih.biPlanes = 1;
bmih.biBitCount = ScanState.PixelBits;
bmih.biCompression = BI_RGB;
bmih.biSizeImage = ScanState.Info.Size;
bmih.biXPelsPerMeter = 2835;
bmih.biYPelsPerMeter = 2835;
bmih.biClrUsed = 0;
bmih.biClrImportant = 0;

//open up the output file DWORD = unsigned long* BYTE = unsigned char* LONG = long* WORD = unsigned short*
if( NULL == (fp = fopen(filename, "wb")))
   {
   printf("Can't save as Bitmap");
   exit(-1);
   }
//Write the header information
fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

for(loop = 0; loop < _msize((void *)aColors) / sizeof(RGBQUAD); loop++)
   fwrite(&aColors[loop], sizeof(RGBQUAD), 1, fp);

/* start scanner */
fprintf(stdout,"\n\nScanning: .");
bufferout = ScanState.Info.WidthBytes;
bytesleft = ScanState.Info.Size;

if ( bytesleft < bufferout )// Don't ask for more than needed.
   bufferout = bytesleft;
if((extra = (4-(bufferout % 4))) == 4) //pad it to a 32 bit boundry
   extra = 0;

Buffer = (UINT8 *)malloc(sizeof(UINT8) * (extra + bufferout));
status  = Scan(SL_FIRST, Buffer, bufferout, &received);

for(loop = 0; loop < extra; loop ++)
   Buffer[loop + bufferout] = 0;

received += extra; //Tack on those extra bits.
/* keep on scanning */
while (received)
   {
   bytesleft -= bufferout;  // Ask for full buffersize if possible
   if ( bytesleft < bufferout )  // On last scan ask for only bytes needed
      bufferout = bytesleft;

   fwrite(&Buffer[0], received, 1, fp);

   status = Scan(SL_NEXT, Buffer, bufferout, &received);
   if(received!=0)
      received += extra; //Tack on those extra bits if theres more data to be found
   }    /* while */
fclose(fp);
fprintf(stdout, "\n");

if (status != 0)
   {  // assume some scanner error, probably communication error.
   status=WhatError(status);
   }
}

void Bmp2Jpg( char *inputname, char *outputname)
{
/*struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;

cinfo.err = jpg_std_error(&jerr);
jpeg_create_compress(&cinfo);*/
}

void RLEEncode( char *inputname, char *outputname)
{
BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGBQUAD *Colors;
FILE *fp, *fpo;
int size, loop, oloop, aloop;
UINT8 *FileBuffer, currRun = 0, staggered = FALSE, n1, n2, n3, p1, p2, np;
UINT8 absStag = FALSE, pattern, tempNib, count = 1, RLEstart = FALSE, done = FALSE;
INT16 scanLength, extra, absStart = -1, absEnd = 0;
long byteswritten = 0;
struct stat buf;

if( NULL == (fp = fopen(inputname, "rb")))
   {
   printf("Can't open Bitmap to Compress with RLE");
   exit(-1);
   }

/*Read in the header*/
fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

//Read in the color palette Offset - header info bytes
size = pow(2, bmih.biBitCount) * sizeof(RGBQUAD);

if( (Colors = (RGBQUAD *)malloc(size)) == NULL)
   {
   printf("Can't allocate memory for palette");
   exit(-1);
   }

for(loop = 0; loop < size / sizeof(RGBQUAD); loop++)
   {
   fread(&Colors[loop], sizeof(RGBQUAD), 1, fp);
   printf("Color %d RGB: %d, %d, %d. \n", loop, Colors[loop].rgbRed, Colors[loop].rgbGreen, Colors[loop].rgbBlue);
   }

switch(bmih.biBitCount)
   {
   case 4: bmih.biCompression = BI_RLE4; break;
   //case 8: bmih.biCompression = BI_RLE8; break;
   default: return; //nothing else can be so compressed
   }

scanLength = bmih.biWidth * bmih.biBitCount; //Number of bits per line
if((extra = (32-(scanLength % 32))) == 32) //pad it to a 32 bit boundry
   extra = 0;
scanLength += extra; //Now its a 32 bit multiple
scanLength = scanLength >> 3; //Now its the number of bytes

if( (FileBuffer = (UINT8 *)malloc(scanLength)) == NULL) //Allocate enough space for scan lines
   {
   printf("memory allocation for file buffer failed");
   exit(-1);
   }
fread(&FileBuffer[0], scanLength, 1, fp); //Get the first scan line in there

if( NULL == (fpo = fopen(outputname, "wb")))
   {
   printf("Can't open Output File");
   exit(-1);
   }

//Write the file stats to the output file
printf("Info Header Size: %d\n",bmih.biSize);
printf("File Header Size: %d\n",bmfh.bfSize);
printf("Palette Size: %d\n", size);
printf("Height: %d\n",bmih.biHeight);
printf("Width: %d\n", bmih.biWidth * bmih.biBitCount);
printf("Bits per color: %d\n", bmih.biBitCount);
printf("Compression type: %d (%d BI_RLE4 %d BI_RLE8)\n",bmih.biCompression, BI_RLE4, BI_RLE8);
printf("Size of image: %d\n",bmih.biSizeImage);
printf("File Size: %d\n", bmfh.bfSize);
printf("Offset Bits: %d\n", bmfh.bfOffBits);

fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fpo);
fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, fpo);

//Assume for now just a 4 bit RLE, 8 bit may not be necessary
for(oloop = 0; oloop < bmih.biHeight; oloop++)
   {
   for(loop = 0; loop < scanLength*2; loop++)
      {
      if( (loop*4) == ((scanLength << 3)-extra))
         {
         if(RLEstart == TRUE) //output the current RLE string
            {
            fputc(currRun, fpo); //dump out the RLE
            fputc(ComposeNib(p1, p2), fpo);
            }
         if(absStart != -1) //output the current absolute string
            {
            fputc( (UINT8)0, fpo);
            fputc( (UINT8)(absEnd - absStart), fpo);
            for(aloop = absStart; aloop < absEnd - (absEnd % 2); aloop++)
               fputc(ComposeNib(GetNib(aloop, &FileBuffer[0]), GetNib(aloop, &FileBuffer[0])), fpo);
            if((absEnd % 2) == 1)
               fputc(ComposeNib(GetNib(absEnd, &FileBuffer[0]),0), fpo);
            }
         break;  //at the end of the original scan line
         }
      n1 = GetNib(loop, &FileBuffer[0]);
      n2 = GetNib(loop+1, &FileBuffer[0]);
      n3 = GetNib(loop+2, &FileBuffer[0]);
      if(RLEstart == FALSE)
         {
         if(n1==n3)
            {
            currRun = 3; //Start the RLE
            RLEstart = TRUE;
            p1 = n1;
            p2 = n2; //Set up the pattern
            np = p2; //the nibble pattern to be checked next time
            //Dump out the absolute buffer
            if(absStart != -1)
               {
               fputc( (UINT8)0, fpo);
               fputc( (UINT8)(absEnd - absStart), fpo);
               for(aloop = absStart; aloop < absEnd - (absEnd % 2); aloop=aloop+2)
                  fputc(ComposeNib(GetNib(aloop, &FileBuffer[0]), GetNib(aloop+1, &FileBuffer[0])), fpo);
               if((absEnd % 2) == 1)
                  fputc(ComposeNib(GetNib(absEnd, &FileBuffer[0]),0), fpo);
               absStart = -1;
               }
            }//End comparison equal
         else
            {
            if(absStart == -1)
               {
               absStart = loop; //the absolute string starts from there
               absEnd = absStart + 1;
               }
            else
               {
               absEnd = absEnd + 1;
               if(absEnd == 255) //Too long for 8 bytes
                  {
                  fputc( (UINT8)0, fpo);
                  fputc( (UINT8)(absEnd - absStart), fpo);
                  for(aloop = absStart; aloop < absEnd - (absEnd % 2); aloop++)
                     fputc(ComposeNib(GetNib(aloop, &FileBuffer[0]), GetNib(aloop, &FileBuffer[0])), fpo);
                  if((absEnd % 2) == 1)
                     fputc(ComposeNib(GetNib(absEnd, &FileBuffer[0]),0), fpo);
                  absStart = -1;
                  }
               }
            } //End comparison not equal
         } //End RLE if true
      else
         {
         if(np == n3) //The first 3 nibbles are already part of the RLE
            {
            currRun++;
            if(currRun == 255) //Too much for 8 bits
               {
               fputc(currRun, fpo); //dump out the RLE
               fputc(ComposeNib(p1, p2), fpo);
               RLEstart = FALSE;
               loop++; //RLE's are 3 nibs long so skip ahead one
               }
            else //Continue on as usual
               {
               if(np == p1)
                  np = p2;
               else
                  np = p1;
               }
            }
         else
            {
            fputc(currRun, fpo); //dump out the RLE
            fputc(ComposeNib(p1, p2), fpo);
            RLEstart = FALSE;
            loop++; //RLE's are 3 nibs long so skip ahead one
            }
         }
      }//End inner loop
   fputc( (UINT8)0, fpo); //End of line code
   fputc( (UINT8)0, fpo);
   fread(&FileBuffer[0], scanLength, 1, fp); //Get the next scan line in
   RLEstart = FALSE;
   absStart = -1;
   } //End outer loop
fputc( (UINT8)0, fpo); //End of bitmap code
fputc( (UINT8)1, fpo);
fclose(fp);
fclose(fpo);
if( NULL == (fp = fopen(outputname, "rb")))
   {
   printf("Can't read file stats\n");
   }

/*Read in the header*/
fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

stat(outputname, &buf);
bmfh.bfSize = buf.st_size; //Get the correct compressed size
bmih.biSizeImage = buf.st_size - bmfh.bfOffBits;
fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);
fclose(fp);

printf("%s RLE compressed to %s\n", inputname, outputname);
printf("%s compressed size: %d\n", outputname, byteswritten);
}

void SetScanner(int Xpos, int Ypos, int Xext, int Yext, int Xres, int Yres, int Xscale, int Yscale, int Pixbit, int Dtype, char *name)
{
SCANSTATE ScannerInput;
INT16         ID, status;
time_t curr_time;
sus.bitmapo[0] = 0;
sus.bitrleo[0] = 0;
sus.bitjpgo[0] = 0;

curr_time = time(&curr_time);

   sus.Xpos = Xpos;
   sus.Ypos = Ypos;
   sus.Xext = Xext;
   sus.Yext = Yext;
   sus.Xres = Xres;
   sus.Yres = Yres;
   sus.Xscale = Xscale;
   sus.Yscale = Yscale;
   sus.Pixbit = Pixbit;
   sus.Dtype = Dtype;

   strcat(&sus.bitmapo[0], sus.defpath);
   strcat(sus.bitmapo, name);
   strcat(sus.bitmapo, ".bmp");

   strcat(&sus.bitrleo[0], sus.defpath);
   strcat(sus.bitrleo, name);
   strcat(sus.bitrleo, "rleo");
   strcat(sus.bitrleo, ".bmp");

   strcat(&sus.bitjpgo[0], sus.defpath);
   strcat(sus.bitjpgo, name);
   strcat(sus.bitjpgo, "o");
   strcat(sus.bitjpgo, ".jpg");

//start the random number generator at a different place each time
srand( (unsigned)time( NULL ) );

// Generate a unique number as an identifier,  In a normal windows application,
// ScannerLock would use a window handle
ID = (INT16)rand();

//lock the library for a unique identifier's exclusive use.
status = ScannerLock(ID, 1);
InquireOldestError();
if(status != 0)
   {
   WhatError(status);
   exit(1);
   }
status = ResetScanner();
if(status != 0)
   {
   WhatError(status);
   exit(1);
   }

GetScanState(&ScannerInput, SL_NOM, SL_PIXEL);
ScannerInput.Window.X = Xpos;
ScannerInput.Window.Y = Ypos;
ScannerInput.Window.Xextent = Xext;
ScannerInput.Window.Yextent = Yext;
ScannerInput.Xresolution = Xres;
ScannerInput.Yresolution = Yres;
ScannerInput.Xscale = Xscale;
ScannerInput.Yscale = Yscale;
ScannerInput.PixelBits = Pixbit;
ScannerInput.DataType = Dtype;
SetScanState(&ScannerInput, SL_PIXEL);
/*XPOS YPOS XEXT YEXT XRes YRes XScale YScaleDEPTH DataType filename
DataType:
0 B/W Threshold
1 White
2 Black
3 B/W Dithered
4 B/W GrayScale 4 or 8 bit depth
5 24/30/30/36/36 bit color
6 Color Threshold
7 Color Dithered
8 Chunky Thresh
9 Chunky Dithered
50 Fast B/w Thresh
51 Fast B/W Dithered
52 Fast B/W Gray */
}

UINT8 GetNib(int byte, UINT8 *buffer)
{
UINT8 Nib;

if((byte % 2) == 0)
   {
   return (buffer[byte/2] >> 4);
   }
else
   {
   Nib = buffer[byte/2] << 4;
   Nib = Nib >> 4;
   return Nib;
   }
}

UINT8 ComposeNib(UINT8 high, UINT8 low)
{
return (high << 4) + low;
}

UINT8 Even(UINT8 ch)
{
UINT8 t;
t = !(ch & (UINT8)1);
t = (ch << 7);
t = !(t >> 7);
return t;
}

void LoadDefaults( SetUpStruct *sus )
{
FILE *fp;

if( NULL == (fp = fopen("defaults.aus", "rb")))
   {
   printf("\nCan't open default file setting");
   exit(-1);
   }
else
   fread(sus, sizeof(SetUpStruct), 1, fp);
   SetScanner(sus->Xpos, sus->Ypos, sus->Xext, sus->Yext, sus->Xres, sus->Yres, sus->Xscale, sus->Yscale, sus->Pixbit, sus->Dtype, "defaults");
   fclose(fp);
}

void SetDefaults( SetUpStruct *sus )
{
FILE *fp;

if( NULL == ( fp = fopen("defaults.aus", "wb")))
   {
   printf("\nCan't open defaults file for writing");
   exit(-1);
   }
else
   fwrite(sus, sizeof(SetUpStruct), 1, fp);
   fclose(fp);
}

void Orientate(char *name, char *defpth)
{
FILE *fpo, *fp;
BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGBQUAD* Colors;
int loop, oloop, size, scanLength, extra;
UINT8 *FileBuffer, *FileBuffer2;
char tempth[100];

if( NULL == (fp = fopen(name, "rb")))
   {
   printf("Can't open Bitmap to Compress with RLE.\n");
   exit(-1);
   }
strcat(&tempth[0], defpth);
strcat(&tempth[0], "tempscan.bmp");

if( NULL == (fpo = fopen(&tempth[0], "wb")))
   {
   printf("Can't open temporary file to orientate bitmap.\n");
   exit(-1);
   }


/*Read in the header*/
fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

//Write out to the temp file
fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fpo);
fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, fpo);

//Read in the color palette Offset - header info bytes
size = pow(2, bmih.biBitCount) * sizeof(RGBQUAD);

if( (Colors = (RGBQUAD *)malloc(size)) == NULL)
   {
   printf("Can't allocate memory for palette");
   exit(-1);
   }

for(loop = 0; loop < size / sizeof(RGBQUAD); loop++)
   {
   fread(&Colors[loop], sizeof(RGBQUAD), 1, fp);  //Copy everything as is
   fwrite(&Colors[loop], sizeof(RGBQUAD), 1, fpo);
   }

scanLength = bmih.biWidth * bmih.biBitCount; //Number of bits per line
if((extra = (32-(scanLength % 32))) == 32) //pad it to a 32 bit boundry
   extra = 0;
scanLength += extra; //Now its a 32 bit multiple
scanLength = scanLength >> 3; //Now its the number of bytes

if( (FileBuffer = (UINT8 *)malloc(scanLength)) == NULL) //Allocate enough space for scan lines
   {
   printf("memory allocation for file buffer failed");
   exit(-1);
   }
if( (FileBuffer2 = (UINT8 *)malloc(scanLength)) == NULL) //Allocate enough space for scan lines
   {
   printf("memory allocation for file buffer failed");
   exit(-1);
   }
fread(&FileBuffer[0], scanLength, 1, fp); //Get the first scan line in there
//memcpy((void *)&FileBuffer2[0], (const void *)&FileBuffer[0], (size_t)scanLength);

//Switch the bits in each scan line
for(oloop = 0; oloop < bmih.biHeight; oloop++)
   {
   for(loop = 0; loop <  (scanLength - extra)*2; loop=loop+2)
      {
      FileBuffer2[((scanLength - extra) - (loop/2)) - 1] = ComposeNib(GetNib(loop+1, &FileBuffer[0]), GetNib(loop, &FileBuffer[0]));
      }
   fwrite(&FileBuffer2[0], scanLength, 1, fpo); //Get the first scan line in there
   fread(&FileBuffer[0], scanLength, 1, fp);
   }

fseek(fpo, bmfh.bfOffBits,SEEK_SET);
//Switch the top scan line with the bottom scan line
for(oloop = bmih.biHeight; oloop > 0; oloop--)
   {
   fseek(fp, (((oloop-1)*scanLength)+bmfh.bfOffBits), SEEK_SET);
   fread(&FileBuffer[0], scanLength, 1, fp);
   fwrite(&FileBuffer[0], scanLength, 1, fpo);
   }
_fcloseall();
remove(name);
rename(&tempth[0], name);
}
