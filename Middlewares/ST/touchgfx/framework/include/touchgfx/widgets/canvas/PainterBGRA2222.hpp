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
 * @file touchgfx/widgets/canvas/PainterBGRA2222.hpp
 *
 * Declares the touchgfx::PainterBGRA2222 class.
 */
#ifndef TOUCHGFX_PAINTERBGRA2222_HPP
#define TOUCHGFX_PAINTERBGRA2222_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBGRA2222.hpp>

namespace touchgfx
{
/**
 * The PainterBGRA2222 class allows a shape to be filled with a given color
 * value. This allows anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterBGRA2222 : public AbstractPainterBGRA2222
{
public:
    /**
     * Initializes a new instance of the PainterBGRA2222 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterBGRA2222(colortype color = 0)
        : AbstractPainterBGRA2222(), painterColor(0), painterRed(0), painterGreen(0), painterBlue(0)
    {
        setColor(color);
    }

    /**
     * Sets color to use when drawing the CanvasWidget.
     *
     * @param  color The color.
     */
    void setColor(colortype color)
    {
        painterColor = color;
        painterRed = Color::getRed(color);
        painterGreen = Color::getGreen(color);
        painterBlue = Color::getBlue(color);
    }

    /**
     * Gets the current color.
     *
     * @return The color.
     */
    colortype getColor() const
    {
        return painterColor;
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);

    colortype painterColor; ///< The color
    uint8_t painterRed;     ///< The red part of the color, scaled up to [0..255]
    uint8_t painterGreen;   ///< The green part of the color, scaled up to [0..255]
    uint8_t painterBlue;    ///< The blue part of the color, scaled up to [0..255]
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERBGRA2222_HPP
