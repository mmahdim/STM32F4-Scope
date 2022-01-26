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
#include <touchgfx/Utils.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
CanvasWidget::CanvasWidget()
    : Widget(),
      canvasPainter(0),
      maxRenderLines(0x7FFF),
      alpha(255)
{
}

void CanvasWidget::setPainter(AbstractPainter& painter)
{
    canvasPainter = &painter;
}

AbstractPainter& CanvasWidget::getPainter() const
{
    assert(canvasPainter != 0 && "No painter set");
    return *canvasPainter;
}

void CanvasWidget::draw(const Rect& invalidatedArea) const
{
    Rect area = invalidatedArea;

    int16_t* offset;
    int16_t* lines;
    int16_t* width;
    int16_t* height;

    int16_t wantedRenderLines;

    switch (HAL::DISPLAY_ROTATION)
    {
    default:
    case rotate0:
        offset = &area.y;
        lines = &area.height;
        width = &area.width;
        height = &wantedRenderLines;
        break;
    case rotate90:
        offset = &area.x;
        lines = &area.width;
        width = &wantedRenderLines;
        height = &area.height;
        break;
    }

    Rect minimalRect = getMinimalRect();

    bool failedAtLeastOnce = false;
    while (*lines)
    {
        wantedRenderLines = MIN(maxRenderLines, *lines);

        while (wantedRenderLines > 0)
        {
            Rect smallArea(area.x, area.y, *width, *height);
            if (!smallArea.intersect(minimalRect))
            {
                break;
            }
            if (drawCanvasWidget(smallArea))
            {
                break;
            }
#ifdef SIMULATOR
            if (CanvasWidgetRenderer::getWriteMemoryUsageReport())
            {
                if (wantedRenderLines > 1)
                {
                    touchgfx_printf("CWR will split draw into multiple draws due to limited memory.\n");
                }
                else
                {
                    touchgfx_printf("CWR was unable to complete a draw operation due to limited memory.\n");
                }
            }
#endif
            wantedRenderLines >>= 1;
            failedAtLeastOnce = true;
        }
        if (wantedRenderLines == 0)
        {
            // We did not manage to draw anything. Set wantedHeight to
            // one to skip a single raster line and try to render the
            // rest of the CanvasWidget.
            wantedRenderLines = 1;
        }
        else
        {
            if (failedAtLeastOnce && maxRenderLines == 0x7FFF)
            {
                // Only adjust maxRenderLines if it is the first draw for the CanvasWidget
                maxRenderLines = wantedRenderLines;
            }
        }
        *offset += wantedRenderLines;
        *lines -= wantedRenderLines;
    }
    if (maxRenderLines == 0x7FFF)
    {
        maxRenderLines--; // 0x7FFF means first draw
    }
}

void CanvasWidget::invalidate() const
{
    Rect minimalRect = getMinimalRect();
    minimalRect &= Rect(0, 0, getWidth(), getHeight());
    invalidateRect(minimalRect);
}

Rect CanvasWidget::getMinimalRect() const
{
    return Rect(0, 0, getWidth(), getHeight());
}

Rect CanvasWidget::getSolidRect() const
{
    return Rect(0, 0, 0, 0);
}

void CanvasWidget::resetMaxRenderLines()
{
    maxRenderLines = 0x7FFF;
}
} // namespace touchgfx
