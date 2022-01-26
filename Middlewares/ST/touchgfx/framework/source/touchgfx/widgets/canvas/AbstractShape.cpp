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
#include <touchgfx/widgets/canvas/AbstractShape.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
AbstractShape::AbstractShape()
    : CanvasWidget(),
      dx(0), dy(0), shapeAngle(0),
      xScale(CWRUtil::toQ10<int>(1)), yScale(CWRUtil::toQ10<int>(1)),
      minimalRect()
{
    Drawable::setWidthHeight(0, 0);
}

bool AbstractShape::drawCanvasWidget(const Rect& invalidatedArea) const
{
    Canvas canvas(this, invalidatedArea);
    int numPoints = getNumPoints();
    if (numPoints == 0)
    {
        return true;
    }

    canvas.moveTo(getCacheX(0), getCacheY(0));
    for (int i = 1; i < numPoints; i++)
    {
        canvas.lineTo(getCacheX(i), getCacheY(i));
    }
    return canvas.render();
}

void AbstractShape::updateAbstractShapeCache()
{
    int numPoints = getNumPoints();

    int xMin = 0;
    int xMax = 0;
    int yMin = 0;
    int yMax = 0;

    for (int i = 0; i < numPoints; i++)
    {
        CWRUtil::Q5 xCorner = getCornerX(i);
        CWRUtil::Q5 yCorner = getCornerY(i);

        CWRUtil::Q5 xCache = dx + ((CWRUtil::mulQ5(xCorner, xScale) * CWRUtil::cosine(shapeAngle))) - ((CWRUtil::mulQ5(yCorner, yScale) * CWRUtil::sine(shapeAngle)));
        if (i == 0 || xCache.to<int>() > xMax)
        {
            xMax = xCache.to<int>();
        }
        if (i == 0 || xCache.to<int>() < xMin)
        {
            xMin = xCache.to<int>();
        }
        CWRUtil::Q5 yCache = dy + ((CWRUtil::mulQ5(yCorner, yScale) * CWRUtil::cosine(shapeAngle))) + ((CWRUtil::mulQ5(xCorner, xScale) * CWRUtil::sine(shapeAngle)));
        if (i == 0 || yCache.to<int>() > yMax)
        {
            yMax = yCache.to<int>();
        }
        if (i == 0 || yCache.to<int>() < yMin)
        {
            yMin = yCache.to<int>();
        }
        setCache(i, xCache, yCache);
    }
    minimalRect = Rect(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1);
}

Rect AbstractShape::getMinimalRect() const
{
    return minimalRect;
}
} // namespace touchgfx
