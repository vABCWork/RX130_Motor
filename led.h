//
//  PB0: Run LED
//  PA4: Auto LED
//  PA3: ALM LED
//
					  // PB0: Run LED
#define Run_LED_PMR  (PORTB.PMR.BIT.B0)   //  汎用入出力ポート
#define Run_LED_PDR  (PORTB.PDR.BIT.B0)   //  出力または入力ポートポートに指定
#define Run_LED_PODR (PORTB.PODR.BIT.B0)  //  出力データ

					  // PA4: Auto LED
#define Auto_LED_PMR  (PORTA.PMR.BIT.B4)  //  汎用入出力ポート
#define Auto_LED_PDR  (PORTA.PDR.BIT.B4)  //  出力または入力ポートポートに指定
#define Auto_LED_PODR (PORTA.PODR.BIT.B4) //  出力データ

					  // PA3: ALM LED
#define ALM_LED_PMR  (PORTA.PMR.BIT.B3)  //  汎用入出力ポート
#define ALM_LED_PDR  (PORTA.PDR.BIT.B3)  //  出力または入力ポートポートに指定
#define ALM_LED_PODR (PORTA.PODR.BIT.B3) //  出力データ


void led_port_ini(void);
