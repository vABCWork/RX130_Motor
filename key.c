
#include   "iodefine.h"
#include   "misratypes.h"

#include "ctsu.h"
#include "led.h"
#include "control.h"
#include "key.h"


// スイッチ入力関係の構造体
//  
struct SW_info
{
	uint8_t status;		// 今回の タッチ有り(Low=0),非タッチ(High=1)状態 (10msec毎)

	uint8_t pre_status;	// 前回(1msec前)の タッチ有り(Low=0),非タッチ(High=1)状態 
	
	uint8_t low_cnt;	// タッチ有りの回数

	uint8_t one_push;	// 0:キー入力処理要求なし 1:キー入力処理要求(1度押し)　（キー入力処理後に0クリア）
	
	uint8_t edge_flg;	// 1: 立下り検出済み
	
	uint16_t threshold;	// ONと判定する CTSUのカウント値 

};


volatile struct  SW_info  Key_sw[CTSU_CH_NUM];	// スイッチ個数分の情報格納領域




//
//  各キーのタッチ有り判定値のセット
// 
void touch_key_on_val(void)
{
	uint8_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
	
		Key_sw[i].threshold = TOUCH_ON_VALUE;
	}
	
}


//
//   タッチキー入力処理
//
// ctsu_scnt[0](b0-b15) : TS14:  Key_sw[0]:  SW1:Run/Stop (1/0)
// ctsu_scnt[1](b0-b15) : TS15:  Key_sw[1]:  SW2:Auto/Manual (1/0)
//
//

void touch_key_proc(void)
{
	uint16_t sensor_cnt;
	uint32_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
		
	  sensor_cnt = 	ctsu_scnt[i];	// 	センサカウント値
	
	  if ( sensor_cnt > Key_sw[i].threshold ) {  // 今回タッチ有り タッチ有り(ON)判定値より大きい
	       
	       Key_sw[i].status = 0;		    // タッチ有り		     
		
	       if ( Key_sw[i].pre_status == 1 ) {   // 今回タッチ有りで、前回無し
	           Key_sw[i].low_cnt =  1;       // キー入力判定カウント = 1 セット
	       }
	       else {				    // 今回タッチ有りで、前回有り
	            Key_sw[i].low_cnt = Key_sw[i].low_cnt + 1;  // キー入力判定カウントのインクリメント
	       }
	      
	  }
	  else {			        // 今回タッチなし
	  	Key_sw[i].status = 1;		// タッチ無し
		
	        if ( Key_sw[i].pre_status == 0 ) {   // 今回タッチなしで、前回タッチあり 
		
		   if ( Key_sw[i].low_cnt > 9 ) {	// キー入力判定カウント > 9 の場合
			touch_key_on_mode( i );		// モード変更
		    }
		}
		Key_sw[i].low_cnt =  0;         // キー入力判定カウントのクリア
	  }
	  
	  Key_sw[i].pre_status = Key_sw[i].status;	// 今回の状態をコピー
	}
	
}


//
//    タッチキーON確定後のモード変更とLED処理
//  
//    SW1 ON
//   　前回 RunならばStopに変更。LED 消灯
//     前回 Stopならば、Runに変更。LED 点灯
//
//    SW2 ON Auto→Manual, Manual→Auto　に変更する。
//     前回 AutoならばManualに変更。LED　消灯
//     前回 ManualならばAutoに変更。LED　点灯
//
//     入力: id = 0: SW1
//              = 1: SW2
//
void touch_key_on_mode(uint8_t id)
{
       if ( id == 0 ) {    // SW1   
	  
	  if ( pre_mode_run_stop == 1 ) { // 前回 RUNの場合
	   	mode_run_stop = 0;	  // 今回 Stop
		Run_LED_PODR = 0;	  // Run LED 消灯 
	   }
	   else {			// 前回 Stopの場合
	   	mode_run_stop = 1;	// 今回 Run
		Run_LED_PODR = 1;  	// Run LED 点灯 
	   }
	   
	   pre_mode_run_stop = mode_run_stop;   // 今回の状態をコピー
	}
	
	else if ( id == 1 ) {   // SW2 
	  
	   if ( pre_mode_auto_manual == 1 ) {  // 前回 Autoの場合
		mode_auto_manual = 0;		// 今回 Manual
		Auto_LED_PODR = 0;		// Auto LED 消灯 
	   }
	   else{			// 前回　Manualの場合
	   	mode_auto_manual = 1;   // 今回 Auto
	   	Auto_LED_PODR = 1;  	// Auto LED 点灯 
	   }
	   
	   pre_mode_auto_manual = mode_auto_manual;  // 今回の状態をコピー
	}
}
