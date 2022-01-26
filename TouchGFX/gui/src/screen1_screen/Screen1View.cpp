#include <gui/screen1_screen/Screen1View.hpp>



Screen1View::Screen1View()
{
	tickCounter = 0;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

uint16_t Screen1View::_UpdateAddress(signed int temp_address) {
    if (temp_address >= buff_len2)
        temp_address = temp_address - buff_len2;
    else if (temp_address < 0)
        temp_address = temp_address + buff_len2;
    return temp_address;
}

void Screen1View::UpdateGraph() {
	for (int i = 0; i < 180; i++) {
		Address = (Buffer_Addr_At_Trigger*2 + (i - 90)*(Settings.timebase.div+1))
				+ (Settings.timebase.offset*9);
		dynamicGraph1.addDataPoint(ADC_Buffer2[_UpdateAddress(Address)]);
	}
	dynamicGraph1.invalidate();
}

void Screen1View::handleTickEvent()
{
	tickCounter++;
	if (End_Of_Capture) {
		tickCounter = 0;
		memcpy((uint16_t* )ADC_Buffer2, (const uint16_t* )ADC_Buffer, sizeof(ADC_Buffer));
		UpdateGraph();
		touchgfx::memset((uint16_t* )ADC_Buffer, 0, sizeof(ADC_Buffer));
		if(Settings.trigger.mode == single_mode) {
			Settings.runmode = Stopped;
			HAL_GPIO_WritePin(Red_LED_GPIO_Port, Red_LED_Pin, GPIO_PIN_SET);
		}
		HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_RESET);
		End_Of_Capture = 0;
	}
	if (Settings.runmode == Stopped) {
		SendButton.setVisible(1);
		SampleRateText.setVisible(0);
		SendButton.invalidate();
		SampleRateText.invalidate();
	} else {
		SendButton.setVisible(0);
		SampleRateText.setVisible(1);
		SendButton.invalidate();
		SampleRateText.invalidate();
	}
	if (Settings.runmode == Running && tickCounter > 2) {
		tickCounter = 0;
		ADC_Multi_Start();							//start ADCs
		for (int i = 0; i < 65000; i++) {
			__NOP();
		}
		EXTI->IMR |= (1U<<2);						//unmask trigger EXTI
		LTDC_Suspend();
	}
}

void Screen1View::SendButtonHandle()
{
	if (Settings.runmode == Stopped) {
		const char* _fill_buffer = "----";
		HAL_UART_Transmit(&huart1, (uint8_t *)&Buffer_Addr_At_Trigger, 4, HAL_MAX_DELAY);	//4		addr
		HAL_UART_Transmit(&huart1, (uint8_t *)_fill_buffer, 4, HAL_MAX_DELAY);				//8		----
		HAL_UART_Transmit(&huart1, (uint8_t *)_fill_buffer, 4, HAL_MAX_DELAY);				//12	----
		HAL_UART_Transmit(&huart1, (uint8_t *)_fill_buffer, 4, HAL_MAX_DELAY);				//16	----
		HAL_UART_Transmit(&huart1, (uint8_t *)ADC_Buffer2, buff_len2*2, HAL_MAX_DELAY);		//		data

	}
}


void Screen1View::ASNHandle()
{
	switch (Settings.trigger.mode) {
		case auto_mode:
			Settings.trigger.mode = normal_mode;
			Unicode::snprintf(asnButtonBuffer, ASNBUTTON_SIZE, "N");
			break;
		case normal_mode:
			Settings.trigger.mode = single_mode;
			Unicode::snprintf(asnButtonBuffer, ASNBUTTON_SIZE, "S");
			break;
		case single_mode:
			Settings.trigger.mode = auto_mode;
			Unicode::snprintf(asnButtonBuffer, ASNBUTTON_SIZE, "A");
			break;
	}
	asnButton.invalidate();
}

void Screen1View::TimDivHandle()
{
	if (TimDivCont.isVisible()) {
		TimDivCont.setVisible(false);
	} else {
		TimDivCont.setVisible(true);
	}
	TimDivCont.invalidate();
}

uint8_t SliderDontUpdategraph = 0;
void Screen1View::TimDivResetHandle()
{
	Settings.timebase.offset = 0;
	TimDivReset.setVisible(0);
	TimDivReset.invalidate();
	UpdateGraph();
	SliderDontUpdategraph = 1;
	TimDivSlider.setValue((int)Settings.timebase.offset);
	TimDivSlider.invalidate();
}

void Screen1View::TimDivRightHandle()
{
	Settings.timebase.offset -= Settings.timebase.div+1;

	Unicode::snprintf(TimDivOffsetBuffer, TIMDIVOFFSET_SIZE, "%d", Settings.timebase.offset);
	TimDivOffset.invalidate();

	if (Settings.timebase.offset == 0) {
		TimDivReset.setVisible(false);
		TimDivReset.invalidate();
	} else if (!TimDivReset.isVisible()) {
		TimDivReset.setVisible(true);
		TimDivReset.invalidate();
	}
	UpdateGraph();
	SliderDontUpdategraph = 1;

	OffsetArrow.moveRelative(16, 0);		//move arrowhead 16 pixels to right
	OffsetArrow.invalidate();

	if (Settings.timebase.offset == 323) {
		TimDivLeft.setVisible(false);
		TimDivLeft.invalidate();
	} else if (Settings.timebase.offset == -323) {
		TimDivRight.setVisible(false);
		TimDivRight.invalidate();
	} else {
		if (!TimDivLeft.isVisible())
		{
			TimDivLeft.setVisible(true);
			TimDivLeft.invalidate();
		}
		if (!TimDivRight.isVisible())
		{
			TimDivRight.setVisible(true);
			TimDivRight.invalidate();
		}
	}
	//TimDivSlider.setValue(Settings.timebase.offset);
	//TimDivSlider.invalidate();
}

void Screen1View::TimDivLeftHande()
{
	Settings.timebase.offset += Settings.timebase.div+1;

	Unicode::snprintf(TimDivOffsetBuffer, TIMDIVOFFSET_SIZE, "%d", Settings.timebase.offset);
	TimDivOffset.invalidate();

	if (Settings.timebase.offset == 0) {
		TimDivReset.setVisible(false);
		TimDivReset.invalidate();
	} else if (!TimDivReset.isVisible()) {
		TimDivReset.setVisible(true);
		TimDivReset.invalidate();
	}
	UpdateGraph();
	SliderDontUpdategraph = 1;

	OffsetArrow.moveRelative(-16, 0);		//move arrowhead 16 pixels to left
	OffsetArrow.invalidate();

	if (Settings.timebase.offset == 323) {
		TimDivLeft.setVisible(false);
		TimDivLeft.invalidate();
	} else if (Settings.timebase.offset == -323) {
		TimDivRight.setVisible(false);
		TimDivRight.invalidate();
	} else {
		if (!TimDivLeft.isVisible())
		{
			TimDivLeft.setVisible(true);
			TimDivLeft.invalidate();
		}
		if (!TimDivRight.isVisible())
		{
			TimDivRight.setVisible(true);
			TimDivRight.invalidate();
		}
	}
	//TimDivSlider.setValue(Settings.timebase.offset);
	//TimDivSlider.invalidate();
}

void Screen1View::TimDivSliderHandle(int value)
{
	Settings.timebase.offset = -value;
	Unicode::snprintf(TimDivOffsetBuffer, TIMDIVOFFSET_SIZE, "%d", Settings.timebase.offset);
	TimDivOffset.invalidate();
	if (Settings.timebase.offset == 0) {
		TimDivReset.setVisible(false);
		TimDivReset.invalidate();
	} else if (!TimDivReset.isVisible()) {
		TimDivReset.setVisible(true);
		TimDivReset.invalidate();
	}
	if (Settings.timebase.offset == 323) {
		TimDivLeft.setVisible(false);
		TimDivLeft.invalidate();
	} else if (Settings.timebase.offset == -323) {
		TimDivRight.setVisible(false);
		TimDivRight.invalidate();
	} else {
		if (!TimDivLeft.isVisible())
		{
			TimDivLeft.setVisible(true);
			TimDivLeft.invalidate();
		}
		if (!TimDivRight.isVisible())
		{
			TimDivRight.setVisible(true);
			TimDivRight.invalidate();
		}
	}
	if (!SliderDontUpdategraph)
		UpdateGraph();
	SliderDontUpdategraph = 0;
	OffsetArrow.moveTo(-Settings.timebase.offset*16 + 156, OffsetArrow.getY());
	OffsetArrow.invalidate();
}

int32_t OffsetArrowPos = 0;

 void Screen1View::TimDivDownHande()  //zoomout
{
	 Settings.timebase.div++;

	 OffsetArrowPos = Settings.timebase.offset * 16 / (Settings.timebase.div+1);
	 OffsetArrow.moveTo(156 - OffsetArrowPos, OffsetArrow.getY());
	 OffsetArrow.invalidate();

	 if (!TimDivUp.isVisible()) {
		 TimDivUp.setVisible(true);
		 TimDivUp.invalidate();
	 }
	 UpdateGraph();
}

 void Screen1View::TimDivUpHandle() //zoomin
{
	 if (Settings.timebase.div > 0) {
		 Settings.timebase.div--;

		 OffsetArrowPos = Settings.timebase.offset * 16 / (Settings.timebase.div+1);
		 OffsetArrow.moveTo(156 - OffsetArrowPos, OffsetArrow.getY());
		 OffsetArrow.invalidate();

		 UpdateGraph();
	 }
	 if (Settings.timebase.div == 0) {
		 TimDivUp.setVisible(false);
		 TimDivUp.invalidate();
	 }
}

