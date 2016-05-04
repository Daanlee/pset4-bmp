/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resize a BMP
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize value infile outfile\n");
        return 1;
    }

    int ratio = atoi(argv[1]);
    if (ratio<1 || ratio>100)
    {
        printf("n, the resize factor, must satisfy 0 < n <=100\n");
        return 5;
    }
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    long oldWidth = bi.biWidth;
    long oldHeight = bi.biHeight;
    //Update height and width of outputfile
    bi.biWidth *= ratio;
    bi.biHeight *= ratio;
    // determine padding for scanlines
    int inpadding = padding; //padding for infile
    padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight);
    //Need to add 54 for the length of the header to get the values
    int headersize = 54;
    bf.bfSize = bi.biSizeImage + headersize;
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
        // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    /*Size Horizontally
    Check padding
    Size vertically*/
       // iterate over infile's scanlines
    for (int i = 0, biHeight = labs(oldHeight); i < biHeight; i++)
    {
        //copy line ratio times to get vertical expansion
        for(int m = 0 ; m < ratio ; m++)
        {
            //FIND start of each line in in string
            fseek(inptr, headersize + i * (oldWidth * sizeof(RGBTRIPLE) + inpadding), SEEK_SET);
        // iterate over pixels in scanline
            for (int j = 0; j < oldWidth; j++)
            {
            // temporary storage
                RGBTRIPLE triple;

            // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile ratio times
                for (int k = 0 ; k < ratio ; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    //rewind(inptr); 
                }
            }
            for (int l = 0; l < padding; l++)
                {
                    fputc(0x00, outptr);                
                }
            //skip over padding in the infile
            fseek(inptr, inpadding, SEEK_CUR);
            //Go back to the begginging of the row read
            //fseek(inptr, (1+i) * (oldWidth * sizeof(RGBTRIPLE) + inpadding), SEEK_CUR);
        }
    }
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
