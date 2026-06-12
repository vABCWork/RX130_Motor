
#include   "iodefine.h"
#include   "misratypes.h"

#include "ctsu.h"
#include "mtu.h"
#include "cmt.h"
#include "led.h"
#include  "debug.h"

volatile uint8_t flg_1msec;
volatile uint8_t flg_100msec;

volatile uint16_t cmt0_cnt;
volatile uint16_t cmt1_cnt;

//
//  コンペアマッチタイマ CMT
//   1msec毎の割り込み 
//  (フォトインタラプタからの信号入力の監視用)
//
#pragma interrupt (Excep_CMT0_CMI0(vect=28))

void Excep_CMT0_CMI0(void){
	
    cmt0_cnt = cmt0_cnt + 1;
    
    if ( cmt0_cnt > 173 ) {
    	capt_ovrflow_flag = 1;   // キャプチャ未発生フラグのセット
	
	CMT.CMSTR0.BIT.STR0 = 0;	// CMT0 カウント停止
    }
	
}


//
//  コンペアマッチタイマ CMT
//   1msec毎の割り込み 
//  1)  CMT1のコンペアマッチによりイベント信号出力
//     イベントリンク　AD変換開始 , CTSU計測開始
//  2) タッチセンサのON/OFF 判定
//
#pragma interrupt (Excep_CMT1_CMI1(vect=29))
void Excep_CMT1_CMI1(void){
		
	//P31_PODR = 1;		// P31 出力データ = 1
	
	flg_1msec = 1;		// 1msecフラグのセット
	
	cmt1_cnt = cmt1_cnt + 1;
    
    	if ( cmt1_cnt > 99) {
	   flg_100msec = 1;	// 10msecフラグのセット
	   
           cmt1_cnt = 0;	// cmt1カウンタクリア
	}
	
}


//   CMTユニット0のCMT0 ( 1 msec タイマ )
//    インプットキャプチャ監視用
// (フォトインタラプタからの信号入力の監視)
//
//  PCLKB(=24MHz)の8分周でカウント 24/8 =3 MHz
//      1カウント = 1/3 = 0.333 [usec]  
//    1*1000 usec =  N * 0.333 usec 
//      N = 3000
//
//  インプットキャプチャ 割り込み処理 void Excep_MTU0_TGIB0(void)  (mtu.c)で、
//   タイマースタート
//

void cmt_ch0_start(void)
{		
	CMT0.CMCR.BIT.CKS = 0x0;        // PCLKB/8       
	CMT0.CMCOR = 2999;		// CMCNTは0からカウント 	

	IPR(CMT0,CMI0) = 3;		// 割り込みレベル = 3　　（15が最高レベル)
	IEN(CMT0,CMI0) = 1;		// CMI0 割込み許可
	
	CMT0.CMCR.BIT.CMIE = 1;		// コンペアマッチ割込み　許可

}


//
//    CMTユニット0のCMT1 ( 1msec タイマ )
//
//  イベントリンクコントローラのイベント用 タイマ (CMT1) 
//   ADとCTSUを開始するためのイベント発生(1msec毎)
//
//  PCLKB(=24MHz)の8分周でカウント 24/8 =3 MHz
//      1カウント = 1/3 = 0.333 [usec]  
//    1*1000 usec =  N * 0.333 usec 
//      N = 3000

void cmt_ch1_start(void)
{
	CMT1.CMCR.BIT.CKS = 0x0;        // PCLKB/8    
	CMT1.CMCOR = 2999;		// CMCNTは0からカウント 
	
	IPR(CMT1,CMI1) = 4;		// 割り込みレベル = 4　　（15が最高レベル)
	IEN(CMT1,CMI1) = 1;		// CMI1 割込み許可
	
	CMT1.CMCR.BIT.CMIE = 1;		// コンペアマッチ割込み　許可
	CMT.CMSTR0.BIT.STR1 = 1;	// CMT1 カウント開始
	
	cmt1_cnt = 0;			// cmt1カウンタクリア
	
}
