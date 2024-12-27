#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include "bb8.h"

TFT_eSPI tft = TFT_eSPI();
AnimatedGIF gif;

void setup()
{
    Serial.begin(115200);
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    gif.begin(GIF_PALETTE_RGB565_BE);
    gif.setDrawType(GIF_DRAW_COOKED);
    const bool opened = openGif((uint8_t *)bb8, sizeof(bb8));
    if (!opened)
    {
        Serial.println("Cannot open GIF!");
        while (true)
        {
            /* useless to continue */
        };
    }
}

void loop()
{
    tft.startWrite();
    gif.playFrame(true, NULL);
    tft.endWrite();
}

// Open Gif and allocate memory
bool openGif(uint8_t *gifdata, size_t gifsize)
{
    if (gif.open(gifdata, gifsize, GIFDraw))
    {
        Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
        Serial.printf("GIF memory size is %ld (%2.2f MB)\n", gifsize, (float)gifsize / (1024 * 1024));
        if (!gif.getFrameBuf())
        {
            if (gif.allocFrameBuf(GIFAlloc) != GIF_SUCCESS)
            {
                Serial.println("Not Enough RAM memory for frame buffer");
                return false;
            }
        }
        return true;
        tft.fillScreen(TFT_BLACK);
    }
    else
    {
        printGifErrorMessage(gif.getLastError());
        return false;
    }
}

void GIFDraw(GIFDRAW *pDraw)
{
    if (pDraw->y == 0)
    { // set the memory window (once per frame) when the first line is rendered
        tft.setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
    }
    // For all other lines, just push the pixels to the display. We requested 'COOKED'big-endian RGB565 and
    tft.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}

void *GIFAlloc(uint32_t u32Size)
{
    return malloc(u32Size);
}

// Get human-readable error related to GIF
void printGifErrorMessage(int errorCode)
{
    switch (errorCode)
    {
    case GIF_DECODE_ERROR:
        Serial.println("GIF Decoding Error");
        break;
    case GIF_TOO_WIDE:
        Serial.println("GIF Too Wide");
        break;
    case GIF_INVALID_PARAMETER:
        Serial.println("Invalid Parameter for gif open");
        break;
    case GIF_UNSUPPORTED_FEATURE:
        Serial.println("Unsupported feature in GIF");
        break;
    case GIF_FILE_NOT_OPEN:
        Serial.println("GIF File not open");
        break;
    case GIF_EARLY_EOF:
        Serial.println("GIF early end of file");
        break;
    case GIF_EMPTY_FRAME:
        Serial.println("GIF with empty frame");
        break;
    case GIF_BAD_FILE:
        Serial.println("GIF bad file");
        break;
    case GIF_ERROR_MEMORY:
        Serial.println("GIF memory Error");
        break;
    default:
        Serial.println("Unknown Error");
        break;
    }
}
