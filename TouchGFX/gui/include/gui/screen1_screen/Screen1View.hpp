#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "menu.h"
#include "BitmapDatabase.hpp"
#include "main.h"
#include <stdlib.h>
#include "cmsis_os.h"
#include <string.h>
extern uint16_t ADC_Buffer2[buff_len2];
extern __IO uint32_t Buffer_Addr_At_Trigger;
extern uint8_t Data_Is_Updated;
extern __IO uint8_t End_Of_Capture;
extern TIM_HandleTypeDef htim13;
extern __IO uint16_t ADC_Buffer[buff_len2];
extern Settings_t Settings;
//extern Status_t OSC_Status;
extern UART_HandleTypeDef huart1;


class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent ();
    virtual void SendButtonHandle();
    virtual void ASNHandle();
    virtual void TimDivHandle();
    virtual void TimDivResetHandle();
    virtual void TimDivRightHandle();
    virtual void TimDivLeftHande();
    virtual void TimDivDownHande();
    virtual void TimDivUpHandle();
    virtual void TimDivSliderHandle(int value);
    void UpdateGraph();
protected:
   
private:
    volatile uint16_t EnableEXTI = 0;
    volatile uint8_t Auto_Timeout_Counter = 0;
    signed int Address = 0;
    uint8_t tickCounter = 0;
    uint16_t _UpdateAddress(signed int temp_address);
    uint8_t SendData = 0;
};

#endif // SCREEN1VIEW_HPP
