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

/**
 * @file touchgfx/widgets/canvas/PainterRGB565L8Bitmap.hpp
 *
 * Declares the touchgfx::PainterRGB565L8Bitmap class.
 */
#ifndef TOUCHGFX_PAINTERRGB565L8BITMAP_HPP
#define TOUCHGFX_PAINTERRGB565L8BITMAP_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB565.hpp>

namespace touchgfx
{
/**
 * PainterRGB565L8Bitmap will take the color for a given point in the shape from a
 * bitmap. Please be aware, the the bitmap is used by the CanvasWidgetRenderer (not
 * Shape), so any rotation you might specify for a Canvas Widget (e.g. Shape) is not
 * applied to the bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterRGB565L8Bitmap : public AbstractPainterRGB565
{
public:
    /**
     * Initializes a new instance of the PainterRGB565L8Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     */
    PainterRGB565L8Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterRGB565(), bitmapPointer(0), bitmapExtraPointer(0), bitmap(), bitmapRectToFrameBuffer()
    {
        setBitmap(bmp);
    }

    /**
     * Sets a bitmap to be used when drawing the CanvasWidget.
     *
     * @param  bmp The bitmap.
     */
    void setBitmap(const Bitmap& bmp);

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderInit();

    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);

    const uint8_t* bitmapPointer;      ///< Pointer to the bitmap (L8)
    const uint8_t* bitmapExtraPointer; ///< Pointer to the bitmap alpha data for RGB565 / CLUT for L8

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERRGB565L8BITMAP_HPP
