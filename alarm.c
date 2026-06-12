#include "iodefine.h"
#include  "misratypes.h"
#include   "led.h"
#include  "crc_16.h"
#include "ad.h"
#include  "alarm.h"



// SLGとのI/O用ポート 　
//  SLGとのI/O用:
//      P27 = SLG_TO_RX (SLG->RX)
//      PC7 = RX_TO_SLG (RX->SLG)
//
void SLG_Port_Set(void)
{
			       // P27
	SLG_RX_PMR = 0;  	// 汎用入出力ポート
	SLG_RX_PDR = 0;  	// 入力ポート設定
	
		       		// PC7
	RX_SLG_PMR= 0;  	// 汎用入出力ポート
	RX_SLG_PDR = 1;  	// 出力ポート設定

}




//  ALM LEDの点灯
// 条件:
//  1) 受信データのCRCエラー
//  2) SLG47011からの熱電対の断線エラー
//  3) モータ電流(m_volt) 300[mA]以上
//      m_volt = (AD cnt / 4095.0) * 550;
//　　　AD cnt = (300/550)*4095 = 2234
//
void alarm_check(void)
{
	uint32_t  alm_flg; 
	
	alm_flg = 0;
	
	if ( crc_16_err == 1 ) {	// 受信データのCRCエラー
		alm_flg = 1;
	}
	
	if(  SLG_RX_PIDR == 1) {  // PC27=1の場合、熱電対の断線エラー
	       alm_flg = 1;
	}
	
	if( ad_cnt[0] > 2200 ) {    // モータ電流 300[mA]以上
	      alm_flg = 1;
	}
	
	if ( alm_flg == 1 ) {
	    ALM_LED_PODR = 1;	// ALM LED 点灯
	}
	else {
	    ALM_LED_PODR = 0;
	}
	
}