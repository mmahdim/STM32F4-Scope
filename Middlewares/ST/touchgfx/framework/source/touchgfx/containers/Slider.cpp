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
#include <touchgfx/Drawable.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/containers/Slider.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>

namespace touchgfx
{
Slider::Slider()
    : Container(),
      sliderOrientation(HORIZONTAL),
      currentValue(0),
      valueRangeMin(0),
      valueRangeMax(1),
      background(),
      backgroundSelected(),
      indicator(),
      backgroundSelectedViewPort(),
      indicatorMinPosition(0),
      indicatorMaxPosition(1),
      startValueCallback(0),
      stopValueCallback(0),
      newValueCallback(0)
{
    setTouchable(true);

    // The backgroundSelectedViewPort is a container into which the bitmap for the "filled" background
    // is placed. Containers are viewports, so the dimensions of this container controls how
    // much of the filled background is visible.
    backgroundSelectedViewPort.add(backgroundSelected);

    Container::add(background);
    Container::add(backgroundSelectedViewPort);
    Container::add(indicator);

    // Default value range
    Slider::setValueRange(0, 100);
}

void Slider::setBitmaps(const Bitmap& sliderBackground, const Bitmap& sliderBackgroundSelected, const Bitmap& indicatorBitmap)
{
    assert(sliderBackground.getWidth() == sliderBackgroundSelected.getWidth() &&
           sliderBackground.getHeight() == sliderBackgroundSelected.getHeight() &&
           "Slider::setBitmaps - background and backgroundFilled must have same dimensions");

    background.setBitmap(sliderBackground);
    backgroundSelected.setBitmap(sliderBackgroundSelected);
    indicator.setBitmap(indicatorBitmap);
    backgroundSelectedViewPort.setWidthHeight(backgroundSelected);
}

void Slider::setBitmaps(const BitmapId sliderBackground, const BitmapId sliderBackgroundSelected, const BitmapId indicatorBitmap)
{
    setBitmaps(Bitmap(sliderBackground), Bitmap(sliderBackgroundSelected), Bitmap(indicatorBitmap));
}

void Slider::setupHorizontalSlider(uint16_t backgroundX, uint16_t backgroundY, uint16_t indicatorY, uint16_t indicatorMinX, uint16_t indicatorMaxX)
{
    assert(indicatorMinX < indicatorMaxX && "Slider::setupHorizontalSlider - indicatorMinX must be smaller than indicatorMaxX");

    sliderOrientation = HORIZONTAL;

    background.setXY(backgroundX, backgroundY);
    backgroundSelectedViewPort.setXY(backgroundX, backgroundY);
    backgroundSelected.setXY(0, 0);
    indicator.setY(indicatorY);

    uint16_t backgroundWidth = backgroundX + static_cast<uint16_t>(background.getWidth());
    uint16_t indicatorWidth = indicatorMaxX + static_cast<uint16_t>(indicator.getWidth());
    int16_t newWidth = static_cast<int16_t>(MAX(backgroundWidth, indicatorWidth));

    uint16_t backgroundHeight = backgroundY + static_cast<uint16_t>(background.getHeight());
    uint16_t indicatorHeight = indicatorY + static_cast<uint16_t>(indicator.getHeight());
    int16_t newHeight = static_cast<int16_t>(MAX(backgroundHeight, indicatorHeight));

    indicatorMinPosition = indicatorMinX;
    indicatorMaxPosition = indicatorMaxX;

    setWidthHeight(newWidth, newHeight);

    setValue(currentValue);
}

void Slider::setupVerticalSlider(uint16_t backgroundX, uint16_t backgroundY, uint16_t indicatorX, uint16_t indicatorMinY, uint16_t indicatorMaxY)
{
    assert(indicatorMinY < indicatorMaxY && "Slider::setupVerticalSlider - indicatorMinY must be smaller than indicatorMaxY");

    sliderOrientation = VERTICAL;

    background.setXY(backgroundX, backgroundY);
    backgroundSelectedViewPort.setXY(backgroundX, backgroundY);
    indicator.setX(indicatorX);

    uint16_t backgroundWidth = backgroundX + static_cast<uint16_t>(background.getWidth());
    uint16_t indicatorWidth = indicatorX + static_cast<uint16_t>(indicator.getWidth());
    int16_t newWidth = static_cast<int16_t>(MAX(backgroundWidth, indicatorWidth));

    uint16_t backgroundHeight = backgroundY + static_cast<uint16_t>(background.getHeight());
    uint16_t indicatorHeight = indicatorMaxY + static_cast<uint16_t>(indicator.getHeight());
    int16_t newHeight = static_cast<int16_t>(MAX(backgroundHeight, indicatorHeight));

    indicatorMinPosition = indicatorMinY;
    indicatorMaxPosition = indicatorMaxY;

    setWidthHeight(newWidth, newHeight);

    setValue(currentValue);
}

void Slider::setValue(int value)
{
    updateIndicatorPosition(valueToPosition(value));
}

void Slider::handleClickEvent(const ClickEvent& event)
{
    if ((event.getType() == ClickEvent::PRESSED) || (event.getType() == ClickEvent::RELEASED))
    {
        // Communicate the start value if a listener is registered
        if ((event.getType() == ClickEvent::PRESSED) && (startValueCallback != 0) && startValueCallback->isValid())
        {
            startValueCallback->execute(*this, currentValue);
        }

        if (sliderOrientation == HORIZONTAL)
        {
            updateIndicatorPosition(event.getX() - getIndicatorRadius());
        }
        else
        {
            updateIndicatorPosition(event.getY() - getIndicatorRadius());
        }

        // Communicate the stop value if a listener is registered
        if ((event.getType() == ClickEvent::RELEASED) && (stopValueCallback != 0) && stopValueCallback->isValid())
        {
            stopValueCallback->execute(*this, currentValue);
        }
    }
}

void Slider::handleDragEvent(const DragEvent& event)
{
    if (sliderOrientation == HORIZONTAL)
    {
        updateIndicatorPosition(event.getNewX() - getIndicatorRadius());
    }
    else
    {
        updateIndicatorPosition(event.getNewY() - getIndicatorRadius());
    }
}

int16_t Slider::valueToPosition(int value) const
{
    value = MIN(valueRangeMax, value);
    value = MAX(value, valueRangeMin);

    int coordinateOffset = ((value - valueRangeMin) * (getIndicatorPositionRangeSize() + 1)) / getValueRangeSize();

    int result = indicatorMinPosition + coordinateOffset;

    if (sliderOrientation == VERTICAL)
    {
        // Vertical slider grows as the position decreases so invert the coordinate
        result = indicatorMinPosition + (indicatorMaxPosition - result);
    }

    return result;
}

int Slider::positionToValue(int16_t position) const
{
    int result;

    if (position == indicatorMinPosition)
    {
        // Ensure that min coordinate always results in min value
        result = valueRangeMin;
    }
    else if (position == indicatorMaxPosition)
    {
        // Ensure that max coordinate always results in max value
        result = valueRangeMax;
    }
    else
    {
        int rounding = getIndicatorPositionRangeSize() / 2;
        int valueOffset = (((position - indicatorMinPosition) * getValueRangeSize()) + rounding) / getIndicatorPositionRangeSize();

        result = valueRangeMin + valueOffset;
    }

    if (sliderOrientation == VERTICAL)
    {
        // Vertical slider grows as the position decreases so invert the value
        result = valueRangeMin + (valueRangeMax - result);
    }

    return result;
}

void Slider::updateIndicatorPosition(int16_t position)
{
    // Cut off positions outside the slider area
    position = MAX(position, indicatorMinPosition);
    position = MIN(position, indicatorMaxPosition);

    if (sliderOrientation == HORIZONTAL)
    {
        indicator.moveTo(position, indicator.getY());

        backgroundSelectedViewPort.invalidate();
        backgroundSelectedViewPort.setWidth((position - backgroundSelectedViewPort.getX()) + getIndicatorRadius());
        backgroundSelectedViewPort.invalidate();
    }
    else
    {
        indicator.moveTo(indicator.getX(), position);

        backgroundSelectedViewPort.invalidate();
        int16_t newViewPortHeight = background.getRect().bottom() - (position + getIndicatorRadius());
        backgroundSelectedViewPort.setPosition(backgroundSelectedViewPort.getX(), position + getIndicatorRadius(), backgroundSelectedViewPort.getWidth(), newViewPortHeight);
        backgroundSelected.setY(-(backgroundSelected.getHeight() - newViewPortHeight));
        backgroundSelectedViewPort.invalidate();
    }

    currentValue = positionToValue(position);

    // Communicate the new value if a listener is registered
    if ((newValueCallback != 0) && newValueCallback->isValid())
    {
        newValueCallback->execute(*this, currentValue);
    }
}

uint16_t Slider::getIndicatorRadius() const
{
    uint16_t result;

    if (sliderOrientation == HORIZONTAL)
    {
        result = indicator.getWidth() / 2;
    }
    else
    {
        result = indicator.getHeight() / 2;
    }

    return result;
}

void Slider::setValueRange(int minValue, int maxValue, int newValue)
{
    assert(minValue < maxValue && "Slider::setValueRange - minValue must be smaller than maxValue");

    valueRangeMin = minValue;
    valueRangeMax = maxValue;

    setValue(newValue);
}

void Slider::setValueRange(int minValue, int maxValue)
{
    int newValue = currentValue;

    if (currentValue < minValue)
    {
        newValue = minValue;
    }
    else if (currentValue > maxValue)
    {
        newValue = maxValue;
    }

    setValueRange(minValue, maxValue, newValue);
}

int Slider::getIndicatorPositionRangeSize() const
{
    return indicatorMaxPosition - indicatorMinPosition;
}

int Slider::getValueRangeSize() const
{
    return valueRangeMax - valueRangeMin;
}
} // namespace touchgfx
