/******************************************************************************
* Copyright (c) 2018(-2021) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.17.0 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565Bitmap.hpp>

namespace touchgfx
{
void PainterRGB565Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB565 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB565 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGB565Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint16_t* p = reinterpret_cast<uint16_t*>(ptr) + (x + xAdjust);

    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;

    if (!renderInit())
    {
        return;
    }

    if (currentX + (int)count > bitmapRectToFrameBuffer.width)
    {
        count = bitmapRectToFrameBuffer.width - currentX;
    }

    const uint16_t* const p_lineend = p + count;
    if (bitmap.getFormat() == Bitmap::RGB565)
    {
        const uint16_t* src = bitmapRGB565Pointer;
        const uint8_t* srcAlpha = bitmapAlphaPointer;
        if (srcAlpha)
        {
            if (widgetAlpha == 0xFF)
            {
                do
                {
                    const uint8_t alpha = LCD::div255((*covers++) * (*srcAlpha++));
                    if (alpha == 0xFF)
                    {
                        // Solid pixel
                        *p = *src;
                    }
                    else if (alpha)
                    {
                        // Non-Transparent pixel
                        *p = mixColors(*src, *p, alpha);
                    }
                    p++;
                    src++;
                } while (p < p_lineend);
            }
            else
            {
                do
                {
                    const uint8_t alpha = LCD::div255((*covers++) * LCD::div255((*srcAlpha++) * widgetAlpha));
                    if (alpha) // This can never get to max=0XFF as totalAlpha<0xFF
                    {
                        // Non-Transparent pixel
                        *p = mixColors(*src, *p, alpha);
                    }
                    p++;
                    src++;
                } while (p < p_lineend);
            }
        }
        else
        {
            if (widgetAlpha == 0xFF)
            {
                do
                {
                    //use alpha from covers directly
                    const uint8_t alpha = *covers++;
                    if (alpha == 0xFF)
                    {
                        // Solid pixel
                        *p = *src;
                    }
                    else
                    {
                        // Non-Transparent pixel
                        *p = mixColors(*src, *p, alpha);
                    }
                    p++;
                    src++;
                } while (p < p_lineend);
            }
            else
            {
                do
                {
                    const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);

                    *p = mixColors(*src, *p, alpha);

                    p++;
                    src++;
                } while (p < p_lineend);
            }
        }
    }
    else if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        const uint32_t* src = bitmapARGB8888Pointer;
        if (widgetAlpha == 0xFF)
        {
            uint32_t newpix;
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
                newpix = *src;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p = ((newpix >> 8) & RMASK) | ((newpix >> 5) & GMASK) | ((newpix >> 3) & BMASK);
                }
                else if (alpha)
                {
                    // Non-Transparent pixel
                    *p = mixColors((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *p, alpha);
                }
                p++;
                src++;
            } while (p < p_lineend);
        }
        else
        {
            uint32_t newpix;
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
                if (alpha)
                {
                    // Non-Transparent pixel
                    newpix = *src;
                    *p = mixColors((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *p, alpha);
                }
                p++;
                src++;
            } while (p < p_lineend);
        }
    }
}

bool PainterRGB565Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB565Pointer = 0;
    bitmapAlphaPointer = 0;

    if (bitmap.getId() == BITMAP_INVALID)
    {
        return false;
    }

    if ((currentX >= bitmapRectToFrameBuffer.width) || (currentY >= bitmapRectToFrameBuffer.height))
    {
        // Outside bitmap area, do not draw anything
        // Consider the following instead of "return" to get a tiled image:
        //   currentX %= bitmapRectToFrameBuffer.width
        //   currentY %= bitmapRectToFrameBuffer.height
        return false;
    }

    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        bitmapARGB8888Pointer = (const uint32_t*)bitmap.getData();
        if (!bitmapARGB8888Pointer)
        {
            return false;
        }
        bitmapARGB8888Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    if (bitmap.getFormat() == Bitmap::RGB565)
    {
        bitmapRGB565Pointer = (const uint16_t*)bitmap.getData();
        if (!bitmapRGB565Pointer)
        {
            return false;
        }
        bitmapRGB565Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        // Get alpha data (RGB565 format)
        bitmapAlphaPointer = (const uint8_t*)bitmap.getExtraData();
        if (bitmapAlphaPointer)
        {
            bitmapAlphaPointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        }
        return true;
    }

    return false;
}

bool PainterRGB565Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapARGB8888Pointer != 0)
    {
        uint32_t argb8888 = *bitmapARGB8888Pointer++;
        alpha = (argb8888 >> 24) & 0xFF;
        red = (argb8888 >> 16) & 0xF8;
        red |= red >> 5; // To get full range 0-0xFF, not just 0-0xF8
        green = (argb8888 >> 8) & 0xFC;
        green |= green >> 6; // To get full range 0-0xFF, not just 0-0xFC
        blue = argb8888 & 0xF8;
        blue |= blue >> 5; // To get full range 0-0xFF, not just 0-0xF8
    }
    else if (bitmapRGB565Pointer != 0)
    {
        uint16_t rgb565 = *bitmapRGB565Pointer++;
        red = (rgb565 & RMASK) >> 8;
        red |= red >> 5; // To get full range 0-0xFF, not just 0-0xF8
        green = (rgb565 & GMASK) >> 3;
        green |= green >> 6; // To get full range 0-0xFF, not just 0-0xFC
        blue = (rgb565 & BMASK) << 3;
        blue |= blue >> 5; // To get full range 0-0xFF, not just 0-0xF8
        if (bitmapAlphaPointer)
        {
            alpha = *bitmapAlphaPointer++;
        }
        else
        {
            alpha = 0xFF; // No alpha per pixel in the image, it is solid
        }
    }
    return true;
}
} // namespace touchgfx
