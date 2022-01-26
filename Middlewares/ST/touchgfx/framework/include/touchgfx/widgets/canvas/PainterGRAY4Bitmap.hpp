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
 * @file touchgfx/widgets/canvas/PainterGRAY4Bitmap.hpp
 *
 * Declares the touchgfx::PainterGRAY4Bitmap class.
 */
#ifndef TOUCHGFX_PAINTERGRAY4BITMAP_HPP
#define TOUCHGFX_PAINTERGRAY4BITMAP_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY4.hpp>

namespace touchgfx
{
/**
 * PainterGRAY4Bitmap will take the color for a given point in the shape from a bitmap. Please
 * be aware, the the bitmap is used by the CanvasWidgetRendere (not Shape), so any
 * rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterGRAY4Bitmap : public AbstractPainterGRAY4
{
public:
    /**
     * Initializes a new instance of the PainterGRAY4Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     */
    PainterGRAY4Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterGRAY4(), bitmapGRAY4Pointer(0), bitmapAlphaPointer(0), bitmap(), bitmapRectToFrameBuffer()
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

    virtual bool renderNext(uint8_t& gray, uint8_t& alpha);

    const uint8_t* bitmapGRAY4Pointer; ///< Pointer to the bitmap (GRAY4)
    const uint8_t* bitmapAlphaPointer; ///< Pointer to the bitmap alpha data for GRAY4

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERGRAY4BITMAP_HPP
