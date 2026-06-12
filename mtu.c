#include "iodefine.h"
#include  "misratypes.h"
#include  "ad.h"
#include   "cmt.h"
#include "control.h"
#include  "mtu.h"


// PWM周期

volatile uint16_t  motor_a_pwm_cnt;	// PWM duty用 　
volatile uint16_t  motor_b_pwm_cnt;


// 　インプットキャプチャデータ
volatile uint8_t   capt_ovrflow_flag;	// キャプチャ未発生フラグ  TCNT 65535(174 [msec])以内にキャプチャされない場合 = 1 

volatile uint16_t  capt_pt;
volatile uint16_t  capt_data[10];




//
//  MTU2.TGRAコンペアマッチ割り込み(TGIA2) (100 usec毎)
//
//   PWM周期 = 100usec 
//  タイマカウントのクロック = 24[MHz]  (PCLKB = 24 MHz , PCLK/1)
//  タイマカウント周期 = 0.0416[uec] 
//   PWM周期分のカウント値 N = 100/0.0416 = 2400
// PWM duty   設定値 
//     0% = 2400
//    25% = 2400 - 600 - 1 = 1799
//    50% = 2400 - 1200 - 1 = 1199
//    75% = 2400 - 1800 - 1 = 599
//   100% = 0
//
//  0% : コンペア値 > 周期レジスタ値(2399)
// 100%: コンペア値 = 0
//
// 注) RUNモードで100%　出力(コンペア値=0)から、STOP（出力=0%)にする場合、
//     コンペア値=2399にすると、PWM出力は100%が継続する。
//     コンペア値=2400ならば、0%になる。
//

#pragma interrupt (Excep_MTU2_TGIA2(vect=125))

void Excep_MTU2_TGIA2(void){
	
	if ( mode_run_stop == 1 ) {  // Runモードの場合
	 
	   motor_a_pwm_cnt = 2399.0 -  (pid_mv  * 2399.0 );    // モータA用 制御実験用 PWM dutyに対応するカウント値
	 
	}
	else {		// Stopモードの場合
	   motor_a_pwm_cnt = 2400.0;
	}
	
	motor_b_pwm_cnt = 2399.0 -  ( ad_cnt[1] / 4095.0 ) * 2399.0;  // モータB用 (モータAへの負荷用) 
	
	MTU1.TGRA = motor_a_pwm_cnt;
	
	MTU1.TGRB = motor_b_pwm_cnt;   
}



//
// インプットキャプチャ 割り込み処理  端子:MTIOC0B
//

#pragma interrupt (Excep_MTU0_TGIB0(vect=115))

void Excep_MTU0_TGIB0(void){
       
	capt_data[capt_pt] = MTU0.TGRB;   // 最新データの格納

	if ( capt_pt < 9 ) {
		capt_pt = capt_pt + 1;
	}
	else {
		capt_pt = 0;
	}
	
	capt_ovrflow_flag = 0;   // キャプチャ未発生フラグのクリア
	
	CMT.CMSTR0.BIT.STR0 = 1;  // キャプチャ監視タイマ CMT0 カウント開始(1msecタイマ)
	
	cmt0_cnt = 0;		 // 1msecタイマのカウント値クリア
	
}



// PWM機能　出力端子  設定
//
//   PE4 (23pin): MTIOC1A  PWM 出力端子 (Motor-A用) 
//   PE3 (24pin): MTIOC1B  PWM 出力端子 (Motor-B用)
//
void  PWM_Port_Set(void)
{
	MPC.PWPR.BIT.B0WI = 0;  	 // マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
    	 
	MPC.PE4PFS.BYTE = 0x02;		// PE4 = MTIOC1A
	MPC.PE3PFS.BYTE = 0x02;		// PE3 = MTIOC1B
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定
	
	PORTE.PMR.BIT.B4 = 1;          // PE4:周辺機能ポートとして使用
	PORTE.PMR.BIT.B3 = 1;          // PE3:周辺機能ポートとして使用
}



// PWM の設定  
//  
//  PWMモード 2 を使用
//  MTU1,MTU2を使用
//  出力端子: MTIOC1A (モータA用 PWM出力)
//            MTIOC1B (モータB用 PWM出力)
//  初期出力 = Low
//  PWM周期 = 100[usec]
//  タイマカウントのクロック = 24[MHz]  (PCLKB = 24 MHz , PCLK/1)
//  タイマカウント周期 = 0.0416[uec] 
//  周期レジスタ:MTU2.TGRA
//    タイマカウンタクリア要因は、MTU2.TGRAのコンペアマッチ。
//    
//  デューティレジスタ:MTU1.TGRA 　(モータA用)
//     MTU1.TGRA コンペアマッチでHigh出力
//  デューティレジスタ:MTU1.TGRB 　(モータB用)
//     MTU1.TGRB コンペアマッチでHigh出力
//
//  PWM周期毎に、デューティ変更
//     MTU2.TGRAコンペアマッチ割り込みで、デューティレジスタを書き換え
//
//
// 資料: 「RXファミリ MTU3/GPTWを用いたPWM出力方法」 R01AN5995JJ0110 Rev.1.10
//       「RX230グループ、RX231グループ　ユーザーズマニュアル　ハードウェア編」R01UH0496JJ0120 Rev.1.20
//
void PWM_Init_Reg(void)
{
	
	MTU.TSTR.BIT.CST1 = 0;		// MTU1.TCNT　カウント停止
	MTU.TSTR.BIT.CST2 = 0;		// MTU2.TCNT　カウント停止
	
	MTU.TSYR.BIT.SYNC1 = 1;		// MTU1.TCNTカウンタは同期動作
	MTU.TSYR.BIT.SYNC2 = 1;		// MTU2.TCNTカウンタは同期動作
	
	
	MTU1.TCR.BIT.TPSC = 0x00;	// MTU1 PCLKB/1 でカウント
	MTU1.TCR.BIT.CKEG = 0x00;	// MTU1 立ち上がりエッジでカウント	
	MTU1.TCR.BIT.CCLR = 0x03;	// 同期クリア/同期動作をしている他のチャネルのカウンタクリアでTCNTをクリア
	
	MTU2.TCR.BIT.TPSC = 0x00;	// MTU2 PCLK/1 でカウント
	MTU2.TCR.BIT.CKEG = 0x00;	// MTU2 立ち上がりエッジでカウント	
	MTU2.TCR.BIT.CCLR = 0x01;	// TGRAのコンペアマッチでTCNTクリア
	
	
	MTU1.TMDR.BIT.MD = 3;		// MTU1 PWM モード 2
	MTU2.TMDR.BIT.MD = 3;		// MTU2 PWM モード 2
	
							
	MTU1.TIOR.BIT.IOA = 2;		// MTIOC1A端子: 初期出力はLow,TCNTとMTU1.TGRAのコンペアマッチでHigh (表20.22)
	MTU1.TIOR.BIT.IOB = 2;		// MTIOC1B端子: 初期出力はLow,TCNTとMTU1.TGRBのコンペアマッチでHigh (表20.14)

	MTU2.TGRA = 2400 - 1;		// PWM周期 = 100usec ( 100/0.04166 = 2400 )  周期 = 周期レジスタの値 + 1
	MTU2.TCNT = 0;			// タイマカウンタ = 0x0000	
	
		
	IPR(MTU2,TGIA2) = 4;		// 割り込みレベル = 4　　（15が最高レベル)
	IEN(MTU2,TGIA2) = 1;		// TGIA2 割込み許可
	
	MTU2.TIER.BIT.TGIEA = 1;	// TGIEA 割り込み要求の許可 ( MTU2.TGRAコンペアマッチ割り込み(TGIA2))
		
	
	MTU.TSTR.BYTE = MTU.TSTR.BYTE | 0x06;      // MTU1.TCNT, MTU2.TCNT　カウント開始

}



// インプットキャプチャ機能　入力端子 MTIOC0B 設定
//
//   PA1 ( 22 pin) : MTIOC0B : インプットキャプチャ入力端子 
// 

void Input_capture_port_set(void)
{
	MPC.PWPR.BIT.B0WI = 0;  	// マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
	
	MPC.PA1PFS.BYTE = 0x01;		// PA1 = MTIOC0B
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定
	
	PORTA.PMR.BIT.B1 = 1;          // PA1:周辺機能ポートとして使用
	
}



// インプットキャプチャ モード の設定 
//  MTU0.TGRB :インプットキャプチャ レジスタ 
// カウント間隔:
// PCLKB = 24 MHz
//   24/64 = 0.375[MHz] でカウント。　1カウントの時間は、64/24= 2.67[usec]
// オーバフロー時間: 175[msec]  ( 2.67 x 65535[usec] = 174760[usec] )
//    
void Input_capture_Init_Reg(void)
{
	
	MTU.TSTR.BIT.CST0 = 0;		// MTU0.TCNT　カウント停止
	
					// MTU0 フリーランニングカウンタで使用
	MTU0.TCR.BIT.TPSC = 0x03;	// MTU0 PCLKB/64 でカウント
	MTU0.TCR.BIT.CKEG = 0x00;	// MTU0 立ち上がりエッジでカウント
	MTU0.TCR.BIT.CCLR = 0x02;	// TGRBのインプットキャプチャでTCNTクリア
	
	MTU0.TMDR.BIT.MD = 0;		// MTU0 ノーマルモード
	
	MTU0.TIORH.BIT.IOB = 0x08; 	// 立ち上がりエッジでインプットキャプチャ , MTICO0Bの端子機能(表 20.12)
	
	IPR(MTU0,TGIB0) = 3;		// 割り込みレベル = 3　　（15が最高レベル)
	IEN(MTU0,TGIB0) = 1;		// TGIB0 割込み許可
	
	MTU0.TIER.BIT.TGIEB = 1;	// TGIB 割り込み許可
	
	MTU.TSTR.BIT.CST0 = 1;		// MTU0.TCNT　カウント開始
}




