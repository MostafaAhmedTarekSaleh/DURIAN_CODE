//Code snippets:

//1) Flash BLUE LED:

MX_USB_HOST_Process();
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
    HAL_Delay(1000);

//2)Toggle through colours:

if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) // button pressed
        {
          HAL_Delay(20); // debounce

          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET); // wait for release
          HAL_Delay(20); // debounce release

          // turn off all LEDs
          HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

          // choose which LED to turn on
          switch (led_state)
          {
            case 0: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); break; // Green
            case 1: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); break; // Orange
            case 2: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); break; // Red
            case 3: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); break; // Blue
          }

          // move to next LED (wrap around at 4)
          led_state = (led_state + 1) % 4;
        }

//3) Sending data to terminal:

uint8_t msg[] = "Hello Guys ! \r\n";
	      CDC_Transmit_FS(msg, sizeof(msg)-1);
	      HAL_Delay(500);

//4)Receive data from terminal:
//From usbd_cdc_if.c file:

static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
	   // Point the USB driver to your buffer
	    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
	    USBD_CDC_ReceivePacket(&hUsbDeviceFS);

	    // 👉 This is the part that sends it back to Tera Term
	    CDC_Transmit_FS(Buf, *Len);
	    uint8_t len = (uint8_t) * Len;
	    memcpy (buffer, Buf, len);

	    return USBD_OK;
}
