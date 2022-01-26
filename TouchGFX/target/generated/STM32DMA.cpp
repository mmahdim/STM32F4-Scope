
/**
  ******************************************************************************
  * File Name          : STM32DMA.cpp
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma2d.h"
#include <STM32DMA.hpp>
#include <cassert>
#include <touchgfx/Color.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/lcd/LCD.hpp>

/* Makes touchgfx specific types and variables visible to this file */
using namespace touchgfx;

typedef struct
{
    const uint16_t format;
    const uint16_t size;
    const uint32_t* const data;
} clutData_t;

extern "C" DMA2D_HandleTypeDef hdma2d;

extern "C" {
    static void DMA2D_XferCpltCallback(DMA2D_HandleTypeDef* handle)
    {
        (void)handle; // Unused argument
        HAL::getInstance()->signalDMAInterrupt();
    }
}

STM32F4DMA::STM32F4DMA()
    : DMA_Interface(dma_queue), dma_queue(queue_storage, sizeof(queue_storage) / sizeof(queue_storage[0]))
{
}

STM32F4DMA::~STM32F4DMA()
{
    /* Disable DMA2D global Interrupt */
    NVIC_DisableIRQ(DMA2D_IRQn);
}

void STM32F4DMA::initialize()
{
    /* Ensure DMA2D Clock is enabled */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    /* Add transfer complete callback function */
    hdma2d.XferCpltCallback = DMA2D_XferCpltCallback;

    /* Enable DMA2D global Interrupt */
    NVIC_EnableIRQ(DMA2D_IRQn);
}

inline uint32_t STM32F4DMA::getChromARTInputFormat(Bitmap::BitmapFormat format)
{
    // Default color mode set to ARGB8888
    uint32_t dma2dColorMode = DMA2D_INPUT_ARGB8888;

    switch (format)
    {
    case Bitmap::ARGB8888: /* DMA2D input mode set to 32bit ARGB */
        dma2dColorMode = DMA2D_INPUT_ARGB8888;
        break;
    case Bitmap::RGB888: /* DMA2D input mode set to 24bit RGB */
        dma2dColorMode = DMA2D_INPUT_RGB888;
        break;
    case Bitmap::RGB565: /* DMA2D input mode set to 16bit RGB */
        dma2dColorMode = DMA2D_INPUT_RGB565;
        break;
    case Bitmap::ARGB2222: /* Fall through */
    case Bitmap::ABGR2222: /* Fall through */
    case Bitmap::RGBA2222: /* Fall through */
    case Bitmap::BGRA2222: /* Fall through */
    case Bitmap::L8:       /* DMA2D input mode set to 8bit Color Look up table*/
        dma2dColorMode = DMA2D_INPUT_L8;
        break;
    case Bitmap::BW:     /* Fall through */
    case Bitmap::BW_RLE: /* Fall through */
    case Bitmap::GRAY4:  /* Fall through */
    case Bitmap::GRAY2:  /* Fall through */
    default:             /* Unsupported input format for DMA2D */
        assert(0 && "Unsupported Format!");
        break;
    }

    return dma2dColorMode;
}

inline uint32_t STM32F4DMA::getChromARTOutputFormat(Bitmap::BitmapFormat format)
{
    // Default color mode set to ARGB8888
    uint32_t dma2dColorMode = DMA2D_OUTPUT_ARGB8888;

    switch (format)
    {
    case Bitmap::ARGB8888: /* DMA2D output mode set to 32bit ARGB */
        dma2dColorMode = DMA2D_OUTPUT_ARGB8888;
        break;
    case Bitmap::RGB888:   /* Fall through */
    case Bitmap::ARGB2222: /* Fall through */
    case Bitmap::ABGR2222: /* Fall through */
    case Bitmap::RGBA2222: /* Fall through */
    case Bitmap::BGRA2222: /* DMA2D output mode set to 24bit RGB */
        dma2dColorMode = DMA2D_OUTPUT_RGB888;
        break;
    case Bitmap::RGB565: /* DMA2D output mode set to 16bit RGB */
        dma2dColorMode = DMA2D_OUTPUT_RGB565;
        break;
    case Bitmap::L8:     /* Fall through */
    case Bitmap::BW:     /* Fall through */
    case Bitmap::BW_RLE: /* Fall through */
    case Bitmap::GRAY4:  /* Fall through */
    case Bitmap::GRAY2:  /* Fall through */
    default:             /* Unsupported output format for DMA2D */
        assert(0 && "Unsupported Format!");
        break;
    }

    return dma2dColorMode;
}

BlitOperations STM32F4DMA::getBlitCaps()
{
    return static_cast<BlitOperations>(BLIT_OP_FILL
                                       | BLIT_OP_FILL_WITH_ALPHA
                                       | BLIT_OP_COPY
                                       | BLIT_OP_COPY_L8
                                       | BLIT_OP_COPY_WITH_ALPHA
                                       | BLIT_OP_COPY_ARGB8888
                                       | BLIT_OP_COPY_ARGB8888_WITH_ALPHA
                                       | BLIT_OP_COPY_A4
                                       | BLIT_OP_COPY_A8);
}

/*
 * void STM32F4DMA::setupDataCopy(const BlitOp& blitOp) handles blit operation of
 * BLIT_OP_COPY
 * BLIT_OP_COPY_L8
 * BLIT_OP_COPY_WITH_ALPHA
 * BLIT_OP_COPY_ARGB8888
 * BLIT_OP_COPY_ARGB8888_WITH_ALPHA
 * BLIT_OP_COPY_A4
 * BLIT_OP_COPY_A8
 */
void STM32F4DMA::setupDataCopy(const BlitOp& blitOp)
{
    uint32_t dma2dForegroundColorMode = getChromARTInputFormat(static_cast<Bitmap::BitmapFormat>(blitOp.srcFormat));
    uint32_t dma2dBackgroundColorMode = getChromARTInputFormat(static_cast<Bitmap::BitmapFormat>(blitOp.dstFormat));
    uint32_t dma2dOutputColorMode = getChromARTOutputFormat(static_cast<Bitmap::BitmapFormat>(blitOp.dstFormat));

    /* DMA2D OOR register configuration ------------------------------------------*/
    WRITE_REG(DMA2D->OOR, blitOp.dstLoopStride - blitOp.nSteps);

    /* DMA2D BGOR register configuration -------------------------------------*/
    WRITE_REG(DMA2D->BGOR, blitOp.dstLoopStride - blitOp.nSteps);

    /* DMA2D FGOR register configuration -------------------------------------*/
    WRITE_REG(DMA2D->FGOR, blitOp.srcLoopStride - blitOp.nSteps);

    /* DMA2D OPFCCR register configuration ---------------------------------------*/
    WRITE_REG(DMA2D->OPFCCR, dma2dOutputColorMode);

    /* Configure DMA2D data size */
    WRITE_REG(DMA2D->NLR, (blitOp.nLoops | (blitOp.nSteps << DMA2D_NLR_PL_Pos)));

    /* Configure DMA2D destination address */
    WRITE_REG(DMA2D->OMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

    /* Configure DMA2D source address */
    WRITE_REG(DMA2D->FGMAR, reinterpret_cast<uint32_t>(blitOp.pSrc));

    switch (blitOp.operation)
    {
    case BLIT_OP_COPY_A4:
        /* Set DMA2D color mode and alpha mode */
        WRITE_REG(DMA2D->FGPFCCR, DMA2D_INPUT_A4 | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

        /* set DMA2D foreground color */
        WRITE_REG(DMA2D->FGCOLR, ((blitOp.color & 0xF800) << 8) | ((blitOp.color & 0x07E0) << 5) | ((blitOp.color & 0x001F) << 3));

        /* Write DMA2D BGPFCCR register */
        WRITE_REG(DMA2D->BGPFCCR, dma2dBackgroundColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* Configure DMA2D Stream source2 address */
        WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Set DMA2D mode */
        WRITE_REG(DMA2D->CR, DMA2D_M2M_BLEND | DMA2D_IT_TC | DMA2D_CR_START);
        break;
    case BLIT_OP_COPY_A8:
        /* Set DMA2D color mode and alpha mode */
        WRITE_REG(DMA2D->FGPFCCR, DMA2D_INPUT_A8 | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

        /* set DMA2D foreground color */
        WRITE_REG(DMA2D->FGCOLR, ((blitOp.color & 0xF800) << 8) | ((blitOp.color & 0x07E0) << 5) | ((blitOp.color & 0x001F) << 3));

        /* Write DMA2D BGPFCCR register */
        WRITE_REG(DMA2D->BGPFCCR, dma2dBackgroundColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* Configure DMA2D Stream source2 address */
        WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Set DMA2D mode */
        WRITE_REG(DMA2D->CR, DMA2D_M2M_BLEND | DMA2D_IT_TC | DMA2D_CR_START);
        break;
    case BLIT_OP_COPY_WITH_ALPHA:
        /* Set DMA2D color mode and alpha mode */
        WRITE_REG(DMA2D->FGPFCCR, dma2dForegroundColorMode | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

        /* Write DMA2D BGPFCCR register */
        WRITE_REG(DMA2D->BGPFCCR, dma2dBackgroundColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* Configure DMA2D Stream source2 address */
        WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Set DMA2D mode */
        WRITE_REG(DMA2D->CR, DMA2D_M2M_BLEND | DMA2D_IT_TC | DMA2D_CR_START);
        break;
    case BLIT_OP_COPY_L8:
        {
            bool blend = true;
            const clutData_t* const palette = reinterpret_cast<const clutData_t*>(blitOp.pClut);

            /* Write foreground CLUT memory address */
            WRITE_REG(DMA2D->FGCMAR, reinterpret_cast<uint32_t>(&palette->data));

            /* Set DMA2D color mode and alpha mode */
            WRITE_REG(DMA2D->FGPFCCR, dma2dForegroundColorMode | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

            /* Write DMA2D BGPFCCR register */
            WRITE_REG(DMA2D->BGPFCCR, dma2dBackgroundColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

            /* Configure DMA2D Stream source2 address */
            WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

            /* Configure CLUT */
            switch ((Bitmap::ClutFormat)palette->format)
            {
            case Bitmap::CLUT_FORMAT_L8_ARGB8888:
                /* Write foreground CLUT size and CLUT color mode */
                MODIFY_REG(DMA2D->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM), (((palette->size - 1) << DMA2D_FGPFCCR_CS_Pos) | (DMA2D_CCM_ARGB8888 << DMA2D_FGPFCCR_CCM_Pos)));
                break;
            case Bitmap::CLUT_FORMAT_L8_RGB888:
                if(blitOp.alpha == 255)
                {
                  blend = false;
                }
                MODIFY_REG(DMA2D->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM), (((palette->size - 1) << DMA2D_FGPFCCR_CS_Pos) | (DMA2D_CCM_RGB888 << DMA2D_FGPFCCR_CCM_Pos)));
                break;

            case Bitmap::CLUT_FORMAT_L8_RGB565:
            default:
                assert(0 && "Unsupported format");
                break;
            }

            /* Enable the CLUT loading for the foreground */
            SET_BIT(DMA2D->FGPFCCR, DMA2D_FGPFCCR_START);

            while ((READ_REG(DMA2D->FGPFCCR) & DMA2D_FGPFCCR_START) != 0U)
            {
                OSWrappers::taskYield();
            }
            DMA2D->IFCR = (DMA2D_FLAG_CTC);

            /* Set DMA2D mode */
            if(blend)
            {
                WRITE_REG(DMA2D->CR, DMA2D_M2M_BLEND | DMA2D_IT_TC | DMA2D_CR_START);
            }
            else
            {
                WRITE_REG(DMA2D->CR, DMA2D_M2M_PFC | DMA2D_IT_TC | DMA2D_CR_START);
            }
        }
      break;
    case BLIT_OP_COPY_ARGB8888:
    case BLIT_OP_COPY_ARGB8888_WITH_ALPHA:
        /* Set DMA2D color mode and alpha mode */
        WRITE_REG(DMA2D->FGPFCCR, dma2dForegroundColorMode | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

        /* Write DMA2D BGPFCCR register */
        WRITE_REG(DMA2D->BGPFCCR, dma2dBackgroundColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* Configure DMA2D Stream source2 address */
        WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Set DMA2D mode */
        WRITE_REG(DMA2D->CR, DMA2D_M2M_BLEND | DMA2D_IT_TC | DMA2D_CR_START);
        break;
    default: /* BLIT_OP_COPY */
        /* Set DMA2D color mode and alpha mode */
        WRITE_REG(DMA2D->FGPFCCR, dma2dForegroundColorMode | (DMA2D_COMBINE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | (blitOp.alpha << 24));

        /* Perform pixel-format-conversion (PFC) If Bitmap format is not same format as framebuffer format */
        if (blitOp.srcFormat != blitOp.dstFormat)
        {
            /* Start DMA2D : PFC Mode */
            WRITE_REG(DMA2D->CR, DMA2D_M2M_PFC | DMA2D_IT_TC | DMA2D_CR_START);
        }
        else
        {
            /* Start DMA2D : M2M Mode */
            WRITE_REG(DMA2D->CR, DMA2D_M2M | DMA2D_IT_TC | DMA2D_CR_START);
        }
        break;
    }
}

/*
 * void STM32F4DMA::setupDataFill(const BlitOp& blitOp) handles blit operation of
 * BLIT_OP_FILL
 * BLIT_OP_FILL_WITH_ALPHA
 */
void STM32F4DMA::setupDataFill(const BlitOp& blitOp)
{
    uint32_t dma2dOutputColorMode = getChromARTOutputFormat(static_cast<Bitmap::BitmapFormat>(blitOp.dstFormat));

    /* DMA2D OPFCCR register configuration ---------------------------------------*/
    WRITE_REG(DMA2D->OPFCCR, dma2dOutputColorMode);

    /* Configure DMA2D data size */
    WRITE_REG(DMA2D->NLR, (blitOp.nLoops | (blitOp.nSteps << DMA2D_NLR_PL_Pos)));

    /* Configure DMA2D destination address */
    WRITE_REG(DMA2D->OMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

    /* DMA2D OOR register configuration ------------------------------------------*/
    WRITE_REG(DMA2D->OOR, blitOp.dstLoopStride - blitOp.nSteps);

    if (blitOp.operation == BLIT_OP_FILL_WITH_ALPHA)
    {
        /* DMA2D BGOR register configuration -------------------------------------*/
        WRITE_REG(DMA2D->BGOR, blitOp.dstLoopStride - blitOp.nSteps);

        /* DMA2D FGOR register configuration -------------------------------------*/
        WRITE_REG(DMA2D->FGOR, blitOp.dstLoopStride - blitOp.nSteps);

        /* Write DMA2D BGPFCCR register */
        WRITE_REG(DMA2D->BGPFCCR, dma2dOutputColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* Write DMA2D FGPFCCR register */
        WRITE_REG(DMA2D->FGPFCCR, CM_A8 | (DMA2D_REPLACE_ALPHA << DMA2D_BGPFCCR_AM_Pos) | ((blitOp.alpha << 24) & DMA2D_BGPFCCR_ALPHA));

        /* DMA2D FGCOLR register configuration -------------------------------------*/
        WRITE_REG(DMA2D->FGCOLR, (((blitOp.color & 0xF800) << 8) | ((blitOp.color & 0x07E0) << 5) | ((blitOp.color & 0x001F) << 3)) & (DMA2D_FGCOLR_BLUE | DMA2D_FGCOLR_GREEN | DMA2D_FGCOLR_RED));

        /* Configure DMA2D Stream source2 address */
        WRITE_REG(DMA2D->BGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Configure DMA2D source address */
        WRITE_REG(DMA2D->FGMAR, reinterpret_cast<uint32_t>(blitOp.pDst));

        /* Enable the Peripheral and Enable the transfer complete interrupt */
        WRITE_REG(DMA2D->CR, (DMA2D_IT_TC | DMA2D_CR_START | DMA2D_M2M_BLEND));
    }
    else
    {
        /* Write DMA2D FGPFCCR register */
        WRITE_REG(DMA2D->FGPFCCR, dma2dOutputColorMode | (DMA2D_NO_MODIF_ALPHA << DMA2D_BGPFCCR_AM_Pos));

        /* DMA2D FGOR register configuration -------------------------------------*/
        WRITE_REG(DMA2D->FGOR, 0);

        if (blitOp.dstFormat == Bitmap::RGB565)
        {
            // set color
            WRITE_REG(DMA2D->OCOLR, blitOp.color);
        }
        else
        {
            // set color
            WRITE_REG(DMA2D->OCOLR, (blitOp.alpha << 24) | ((blitOp.color & 0xF800) << 8) | ((blitOp.color & 0x07E0) << 5) | ((blitOp.color & 0x001F) << 3));
        }

        /* Enable the Peripheral and Enable the transfer complete interrupt */
        WRITE_REG(DMA2D->CR, (DMA2D_IT_TC | DMA2D_CR_START | DMA2D_R2M));
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
