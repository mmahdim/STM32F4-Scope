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
#include <touchgfx/Drawable.hpp>
#include <touchgfx/Font.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>

namespace touchgfx
{
ButtonWithLabel::ButtonWithLabel()
    : Button(), typedText(), color(0), colorPressed(0), rotation(TEXT_ROTATE_0), textHeightIncludingSpacing(0)
{
}

void ButtonWithLabel::draw(const Rect& area) const
{
    Button::draw(area);

    if (typedText.hasValidId())
    {
        const Font* fontToDraw = typedText.getFont(); //never return 0
        uint8_t height = textHeightIncludingSpacing;
        int16_t offset;
        Rect labelRect;
        switch (rotation)
        {
        default:
        case TEXT_ROTATE_0:
        case TEXT_ROTATE_180:
            offset = (this->getHeight() - height) / 2;
            labelRect = Rect(0, offset, this->getWidth(), height);
            break;
        case TEXT_ROTATE_90:
        case TEXT_ROTATE_270:
            offset = (this->getWidth() - height) / 2;
            labelRect = Rect(offset, 0, height, this->getHeight());
            break;
        }
        Rect dirty = labelRect & area;

        if (!dirty.isEmpty())
        {
            dirty.x -= labelRect.x;
            dirty.y -= labelRect.y;
            translateRectToAbsolute(labelRect);
            LCD::StringVisuals visuals(fontToDraw, pressed ? colorPressed : color, alpha, typedText.getAlignment(), 0, rotation, typedText.getTextDirection(), 0, WIDE_TEXT_NONE);
            HAL::lcd().drawString(labelRect, dirty, visuals, typedText.getText());
        }
    }
}
} // namespace touchgfx
