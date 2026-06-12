
#include   "iodefine.h"
#include   "misratypes.h"
#include "control.h"
#include   "led.h"


//  PB0: Run LED
//  PA4: Auto LED
//  PA3: ALM LED
//
void led_port_ini(void)
{
				// Run LED
	Run_LED_PMR = 0;	// 汎用入出力ポート
	Run_LED_PDR = 1;	// 出力ポートに指定
	Run_LED_PODR = 0;	// 出力 = 0
	
				// Auto LED
	Auto_LED_PMR = 0;	// 汎用入出力ポート
	Auto_LED_PDR = 1;	// 出力ポートに指定
	Auto_LED_PODR = 0;	// 出力 = 0
	
				// ALM LED
	ALM_LED_PMR = 0;	// 汎用入出力ポート
	ALM_LED_PDR = 1;	// 出力ポートに指定
	ALM_LED_PODR = 0;	// 出力 = 0
	
}