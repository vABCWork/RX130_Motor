#include	"iodefine.h"
#include	"misratypes.h"
#include        "ctsu.h"
#include	"ad.h"
#include	"cmt.h"
#include	"mtu.h"
#include	"key.h"
#include	 "control.h"
#include 	"sci1.h"
#include 	"riic_slg47011.h"
#include        "riic_master.h"
#include 	"temp_cal.h"
#include	"led.h"
#include        "alarm.h"
#include 	 "iwdt.h"
#include	"delay.h"
#include	"debug.h"


void un_used_port(void);
void clear_module_stop(void);

 
void main(void)
{
	ICU.NMIER.BIT.IWDTEN = 1;	// IWDTアンダーフロー/リフレッシュエラー割り込み許可　(NMI割り込み)

	un_used_port();		// 未使用端子の処理 (PE1,PE2出力ポート設定)
	clear_module_stop();	// モジュールストップの解除
		
	led_port_ini();		// LEDポートの設定

	touch_key_on_val();	// タッチセンサON判定値の設定
	ctsu_self_set();	// CTSU 設定 (タッチセンサ用 TSCAP(PC4)の放電=100[msec])  
	
	initAD();		// ADコンバータ設定
	
	SCI_1_Port_Set();	// SCI1 ポート設定
	SCI_1_Reg_Set();	// SCI1 通信条件の設定 1.5[Mbps] (8bit-non parity-1stop) 
	SCI1_Init_interrupt();  // SCI1　割り込み設定
	
	initDTC();		// DTC起動

				  // イベントリンク設定
	ELC.ELSR15.BYTE = 0x1f;   // CMT1のコンペアマッチで、AD変換開始
	ELC.ELSR14.BYTE = 0x1f;   // CMT1のコンペアマッチで、CTSU計測開始
	ELC.ELCR.BIT.ELCON = 1;	  // ELC機能有効
	
	cmt_ch1_start();	  // イベントリンクトリガ用タイマの起動 (1msec}
	
	Input_capture_port_set(); // インプットキャプチャ機能　入力端子 MTIOC0B(PA1) 設定 
	Input_capture_Init_Reg(); //  インプットキャプチャ モード の設定
 	cmt_ch0_start();	   // キャプチャ監視タイマの起動 ( 1msec)
	
	
	PWM_Port_Set();		// PWM 出力端子 (MTIOC1A,MTIOC1B）設定 
	PWM_Init_Reg();		// PWM の設定 ( PWM モード 2)  
	
	RIIC0_Port_Set();	//   RIIC ポート設定	(P16: SCL0,  P17: SDA0 )  
	RIIC0_Init();		//   RIIC 初期化
        RIIC0_Init_interrupt();	// RIIC0 割り込み許可 
	
	SLG_Port_Set();		// SLGとのI/O用ポート ( P27:SLG->RX, PC7:RX->SLG )
	
	iic_slave_adrs = 0x08;  // SLG47011 スレーブアドレス = 0x08 

	pid_para_ini();		// PIDパラメータ初期化
	
	while(1) {
	  
	  IWDT_Refresh();	// ウオッチドックタイマリフレッシュ
	
	 // delay_msec(150);	// 150[msec]待ち WDT アンダフロー発生
	  
	  
	  if ( flg_1msec == 1 ) {		// タイマで 1[msec]計測
		    
	       touch_key_proc();	// タッチキー処理
		 
		cal_rpm();		// 速度計算
		 
		cal_control();		// 制御演算 
		   
		 flg_1msec = 0;
	   }
	  
	   if ( flg_100msec == 1 ) {
	      
	       slg_47011_buffer_rd();   // SLG47011 よりAD値の読みだし
	       slg_temp_cal();	   //  SLG データによる温度計算
	
	       if ( rcv_over == 1 )  {   // SCI1 コマンド受信有り
	
	 	  sci1_resp_make();     // SCI1 レスポンス作成と送信
		
		  rcv_over = 0;
	        } 
	       
	       alarm_check();		// アラームのチェック
	       
	       flg_100msec = 0;
	   }
	     	
  }
	
}

//
//  未使用端子の設定
//   P35/NMI:  入力専用 Pull up　要
//   P36/EXTAL: 出力 (使用不可)
//   P31: 出力 (使用不可)
//   PE1: 出力 (デバック用ポート)
//   PE2: 出力 (デバック用ポート)
//
// 18.5 未使用端子の処理
//  P36/EXTAL: メインクロックを使用しない場合は、MOSCCR.MOSTPビットを“1” (汎用ポートP36)に設定
//            ポートP36としても使用しない場合は、ポート1～3、5、ポートA～E、H、J (J6、J7以外)の処理と同様
//

void un_used_port(void)
{
				// P36:出力ポート
	PORT3.PDR.BIT.B6 = 1;
	
	PORT3.PMR.BIT.B1 = 0;	// P31:出力ポート
	PORT3.PDR.BIT.B1 = 1;
	
	PORTE.PMR.BIT.B1 = 0;	// PE1:出力ポート
	PORTE.PDR.BIT.B1 = 1;
	
	PORTE.PMR.BIT.B2 = 0;	// PE2:出力ポート
	PORTE.PDR.BIT.B2 = 1;
}


// モジュールストップの解除
// コンペアマッチタイマ CMTユニット0(CMT0, CMT1)
// マルチファンクションタイマパルスユニット（MTU0 ～ MTU5）
//  I2Cバスインターフェイス0  RIIC0
//  シリアルコミュニケーションインタフェース SCI1
//  タッチセンサコントロールユニット CTSU
//  データトランスファコントローラ DTC
//  イベントリンクコントローラ ELC
//  12 ビットA/D コンバータ (S12ADE)
//  CRC モジュールストップ(CRC)
//
	
void clear_module_stop(void)
{
					
	SYSTEM.PRCR.WORD = 0xA50F;	// クロック発生、消費電力低減機能関連レジスタの書き込み許可	
	
	MSTP(CMT0) = 0;			// コンペアマッチタイマ CMTユニット0(CMT0, CMT1) モジュールストップの解除 SYSTEM.MSTPCRA.BIT.MSTPA15
       // MSTP(CMT1) = 0;			// SYSTEM.MSTPCRA.BIT.MSTPA15
	
	MSTP(MTU) = 0;			// マルチファンクションタイマパルスユニット モジュールストップの解除
		
	MSTP(RIIC0) = 0;                //  RIIC0モジュールストップ解除 (I2C通信)
	MSTP(SCI1) = 0;			//  SCI1  モジュールストップの解除
	
	MSTP(CRC) = 0;			// CRC モジュールストップの解除	
	
	MSTP(CTSU) = 0;			//  CTSU タッチセンサコントロールユニット モジュールストップ解除
	
	MSTP(DTC) = 0;			// DTC データトランスファコントローラ モジュールストップの解除
	
	MSTP(ELC) = 0;			// ELC イベントリンクコントローラ モジュールストップの解除
	
	MSTP(S12AD) =0;			// D12ADE 12 ビットA/D コンバータ モジュールストップの解除
	
	SYSTEM.PRCR.WORD = 0xA500;	// クロック発生、消費電力低減機能関連レジスタ書き込み禁止
}
