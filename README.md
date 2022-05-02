# ESP-Now_TTGO_T-Display_T-Watch2020
Adapting ESP-Now to TTGO T-Watch 2020 and T-Display.

Here I am mashing up the example sketches Master and Slave from ESP-Now in Arduino IDE. Yes this is 2022 so I went to the trouble of renaming these to Sender and Client to avoid any offense. The beauty of ESP-Now is the devices don't need a WiFi router. The ESP-Now devices just find each other and pair up.

First I mashup the minimum code to display text on TTGO T-Watch 2020 with ESP-Now Sender and Client. I found T-Watch had some difficulty with making connection initially. As T-Watch that is Sender got warmed up, it would spontaneously restart. I think this is a combination of power requirements and the WiFi antenna arrangement. Try it for yourself and see? Let me know.

Then I do the same mashup for the T-Display. These seem to pair up right away and run more stable. I will take these to the park for a range test.

Some items of note.

  * The Sender and Client sketches require the same channel number on both sides to make connection. I have set these to one.
   * In Arduino IDE, after selecting TTGO T-Watch as the Board, there is a Board Revision option lower down in the in the Arduino IDE menu to select T-Watch Base, 2020 V1, or 2020 V2.
  * Make sure when setting TFT_eSPI text color, include a background color so new text will overright whatever is already on the display.

ESP-Now on TTGO T-Display and T-Watch 2020
![]()
