#include <machine.h>
#include "iodefine.h"
#include "misratypes.h"
#include "ctsu.h"
#include "delay.h"

#include "debug.h"


volatile uint32_t ctsu_sr[CTSU_CH_NUM];		// CTSU ステータスレジスタ
volatile uint32_t ctsu_so[CTSU_CH_NUM];;	// CTSU センサオフセットレジスタ b0-b9:センサオフセット調整ビット, b10-b17:計測期間設定ビット 
						// b20-b23:スペクトラム拡散サンプリング周期制御ビット,b24-b31:ベースクロック設定ビット
			
volatile uint32_t ctsu_scnt[CTSU_CH_NUM];	// b0-b15: センサカウンタ, b16-b31:センサユニットクロックカウンタ


volatile uint8_t  ctsu_data_index;              // ctsu_sc[]等の格納位置	

volatile uint8_t  ctsu_reading_fg;		// 1 = CTSU計測中 , 0 = 計測完了

//
// CTSU レジスタ設定要求割り込み(CTSUWR)
///

#pragma interrupt (Excep_CTSU_CTSUWR(vect=60))
void Excep_CTSU_CTSUWR(void){
		
	CTSU.CTSUSO.LONG = ctsu_so[ctsu_data_index];
	
	ctsu_reading_fg = 1;	// CTSU計測中

	PE1_PODR = 1;		// PE1 = ON CTSUWRからCTSUFNまでの時間計測用
	
}




//
// CTSU 計測結果読み出し要求割り込み(CTSURD)
//
//

#pragma interrupt (Excep_CTSU_CTSURD(vect=61))
void Excep_CTSU_CTSURD(void){
	
	ctsu_scnt[ctsu_data_index] = CTSU.CTSUSCNT.LONG;	// CTSU センサカウンタ(CTSUSCNT)
	
	ctsu_sr[ctsu_data_index] = CTSU.CTSUSR.LONG;		// CTSU ステータスレジスタ（CTSUSR)
	
	ctsu_data_index = ctsu_data_index + 1; 
	
	
}


//
// CTSU計測終了割り込み(CTSUFN)
//  全タッチセンサの読み出し完了
//
#pragma interrupt (Excep_CTSU_CTSUFN(vect=62))
void Excep_CTSU_CTSUFN(void){
	
		
	ctsu_data_index = 0;		//  incdexをクリア
	
	ctsu_reading_fg = 0;		// CTSU計測完了
	
	
	PE1_PODR = 0;		// PE1 = ON CTSUWRからCTSUFNまでの時間計測用
		
}


//
//  自己容量マルチスキャンモードと割り込みの設定
//
void ctsu_self_set(void)
{
	ctsu_ini();			// 静電容量式タッチセンサ(CTSU)の初期化
			   
	ctsu_ch_enable(); 		// CTSU 有効チャンネルの設定 (TS14,TS15)
	
	 set_ctsu_so();			//  CTSU センサオフセットレジスタ 設定

					// CTSUWR割り込み動作設定
	IPR(CTSU,CTSUWR) = 5;		// 割り込みレベル = 5　　（15が最高レベル)
	IEN(CTSU,CTSUWR) = 1;		//  割込み許可
	
					// CTSURD割り込み動作設定
	IPR(CTSU,CTSURD) = 5;		// 割り込みレベル = 5　　（15が最高レベル)
	IEN(CTSU,CTSURD) = 1;		//  割込み許可
	
					// CTSUFN割り込み動作設定
	IPR(CTSU,CTSUFN) = 5;		// 割り込みレベル = 5　　（15が最高レベル)
	IEN(CTSU,CTSUFN) = 1;		//  割込み許可
	
	
}

//
//  CTSU センサオフセットレジスタ 設定
//    b0-b9:センサオフセット調整ビット
//    b10-b17:計測期間設定ビット 
//    b20-b23:スペクトラム拡散サンプリング周期制御ビット
//    b24-b31:ベースクロック設定ビット
// 
// 1) センサオフセット調整ビット = 0
// 2)  測定回数(n) = 0  ランダムパルスモードの場合 n+1回 = 1回
// 3)  スペクトラム拡散 サンプリング周期ビット =  0 = 1分周
// 4)  ベースクロックの設定ビット(n) = 0
//　  ランダムパルスモードの場合、ベースクロックの周波数は動作クロックの2(n+1)分周
//    動作クロック PCLKB = 24[MHz] 
//    ベースクロック = 24/ 2 = 12[MHz]
//    (ベースクロックをスペクトラム拡散させたものが、センサドライブパルス)
//
//  4-1) n = 0x17(23) ,  24/2*(23+1) = 0.5
//  CTSU.CTSUSO.LONG = 0x17000000;  // ベースクロック 0.5[MHz]
//
//  4-2) n = 0x0b(11) ,  24/2*(11+1) = 1
//  CTSU.CTSUSO.LONG = 0x0b000000;  // ベースクロック 1[MHz]
//
// 4-3) n = 0x05(5) ,  24/2*(5+1) = 2
//  CTSU.CTSUSO.LONG = 0x05000000;  // ベースクロック 2[MHz]
//
// 4-4) n = 0x02(2) ,  24/2*(2+1) = 4
//  CTSU.CTSUSO.LONG = 0x02000000;  // ベースクロック 4[MHz]
//
// 4-5) n = 0x01(1) ,  24/2*(1+1) = 6
//  CTSU.CTSUSO.LONG = 0x01000000;  // ベースクロック 6[MHz]


void set_ctsu_so(void)
{
	//ctsu_so[0] =  0x1700000f;     // ベースクロック 0.5[MHz] , センサオフセット=0x0f
	//ctsu_so[1] =  0x1700000f;     // ベースクロック 0.5[MHz] , センサオフセット=0x0f
	
	//ctsu_so[0] =  0x0b000000;     // ベースクロック 1[MHz] , センサオフセット=0
	//ctsu_so[1] =  0x0b000000;     // ベースクロック 1[MHz] , センサオフセット=0	
			
	//ctsu_so[0] =  0x05000000; 	// ベースクロック 2[MHz] , センサオフセット=0
	//ctsu_so[1] =  0x05000000;     // ベースクロック 2[MHz] , センサオフセット=0
	
	ctsu_so[0] =  0x02000000; 	// ベースクロック 4[MHz] , センサオフセット=0
	ctsu_so[1] =  0x02000000;       // ベースクロック 4[MHz] , センサオフセット=0
	
	//ctsu_so[0] =  0x02010000; 	// ベースクロック 4[MHz] , センサオフセット=0, 計測回数=2 　タッチ無しでオーバフロー
	//ctsu_so[1] =  0x02010000;       // ベースクロック 4[MHz] , センサオフセット=0, 計測回数=2
	
	
	//ctsu_so[0] =  0x01000000; 	// ベースクロック 6[MHz] , センサオフセット=0
	//ctsu_so[1] =  0x01000000;       // ベースクロック 6[MHz] , センサオフセット=0
}


//
//   静電容量式タッチセンサ(CTSU)の初期化
//
void ctsu_ini(void)
{

	   tscap_clear();		// タッチセンサ用 TSCAPの放電
	   
	   touch_port_set();		// タッチセンサのポート設定
	   
	   ctsu_cra_set();		// CTSU 制御レジスタA 設定
	   
	   ctsu_crb_set();		// CTSU 制御レジスタB 設定

					// TSCAP端子に外付けしたLPF容量へのチャージ安定待ち
	   delay_msec(1);		//	1msec待つ   (CTSU電源安定時間 )
	  
}

//
// CTSU 制御レジスタ A (CTSUCRA)
//   外部トリガ(イベントリンクコントローラ ELCからのイベント)で計測開始
//   イベントはCMT1のコンペアマッチ(10msec毎)
//
//  CAP ビットが“1” ( 外部トリガ) のときにSTRT ビットを“1” にすると、外部トリガ待ち状態になり、
//  外部トリガが入力されると計測を開始します。計測が終了すると、再び外部トリガ待ち状態になります。
//
// ・  計測電源の最大供給電 を小さくすると、カウント値が大
//
void ctsu_cra_set(void)
{
					
	CTSU.CTSUCRA.BIT.CAP = 1;	// 外部トリガ
	CTSU.CTSUCRA.BIT.STRT = 0;	// 計測動作停止
	
	CTSU.CTSUCRA.BIT.SNZ = 0;	// スヌーズモード時計測無効/通常動作
	
	CTSU.CTSUCRA.BIT.INIT =1;	//　初期化
	CTSU.CTSUCRA.BIT.PUMPON = 1;    // 昇圧回路 ON ( Vcc <4.5[V] 時 )
	
	CTSU.CTSUCRA.BIT.TXVSEL = 0;	// 送信電源 I/O電源
	CTSU.CTSUCRA.BIT.PON = 1;	// 計測電源 電源ON
	CTSU.CTSUCRA.BIT.CSW = 1;	// TSCAP 接続
	
	CTSU.CTSUCRA.BIT.ATUNE0 = 0;	// 計測電源電圧= 1.5 V
	
//	CTSU.CTSUCRA.BIT.ATUNE1 = 0;	// 計測電源の最大供給電 80[uA] タッチ無しのカウント値=6300,タッチあり=26000
//	CTSU.CTSUCRA.BIT.ATUNE2 = 0;	// 
	
        CTSU.CTSUCRA.BIT.ATUNE1 = 1;	// 計測電源の最大供給電 40[uA] タッチ無しのカウント値=12000,タッチあり=40000 (計測電源電圧= 1.5 V)
        CTSU.CTSUCRA.BIT.ATUNE2 = 0;	// 
	
//	CTSU.CTSUCRA.BIT.ATUNE1 = 0;	// 計測電源の最大供給電 20[uA] タッチ無しのカウント値=22000 ,タッチあり=47000
//	CTSU.CTSUCRA.BIT.ATUNE2 = 1;	// 
	
	
	CTSU.CTSUCRA.BIT.CLK = 0;	// 動作クロック PCLKB(=24[MHz])
	
	CTSU.CTSUCRA.BIT.MD0 = 1;	// マルチスキャンモード
	CTSU.CTSUCRA.BIT.MD1 = 0;	// 自己容量方式

	CTSU.CTSUCRA.BIT.LOAD = 0;	// 定電流負荷モード(通常計測用)
	
	CTSU.CTSUCRA.BIT.POSEL = 0;	// 非計測チャンネル出力 Low
	
	CTSU.CTSUCRA.BIT.SDPSEL = 0;	// センサドライブパルス ランダムパルスモード
	CTSU.CTSUCRA.BIT.PCSEL = 0;	// 昇圧回路クロック センサドライブパルス

	       				// ステートクロック(STCLK)=500[KHz] ,動作クロック=24[MHz]
					//  500[KHz] = 24[MHz] / 分周値
	CTSU.CTSUCRA.BIT.STCLK = 23;	// 分周値 = 48 , 分周値 = ( n + 1 ) * 2

	CTSU.CTSUCRA.BIT.DCMODE = 0;    // 電流計測モード 通常モード
	CTSU.CTSUCRA.BIT.DCBACK = 0;	// DCMODE ビットが“1” のときのみ有効
	
	
	CTSU.CTSUCRA.BIT.STRT = 1;	// 計測動作開始  外部トリガ待ち状態
}


//
// CTSU 制御レジスタ B (CTSUCRB)
//
// 計測パルス数= 基本パルス数 * (PRRATIO[3:0] ビット+ 1)
// 基本パルス数は、PRMODE[1:0] ビットによって選択した周期の2 倍です。
// 計測時間= ( 基本パルス数× (PRRATIO[3:0] ビット+ 1) + ( 基本パルス数? 2) × 0.25) × ベースクロック周期
//
// (*注) RL78/G16のタッチセンサ(CTSUb)の「CTSU同期ノイズ低減設定レジスタ(CTSUSDPRS)」の推奨値を使用
//
// 参考: RL78ファミリ 静電容量センサユニット（CTSU2L）動作説明 R01AN5744JJ0100 Rev.1.00
//       page27 
//      計測期間は、基本単位 × (CTSUSO0.SNUM[5:0] + 1) となります。
//      基本単位 :表2.7

void ctsu_crb_set(void)
{
					// 基本単位=637 2ch計測時間=350[usec], タッチなし＝12000,タッチあり=38000
	CTSU.CTSUCRB.BIT.PRRATIO = 0x0;   // 疑似乱数更新周期設定ビット
	CTSU.CTSUCRB.BIT.PRMODE = 0x0;   // 疑似乱数生成周期設定ビット (255周期=510パルス)
	
	
					// 基本単位=7  2ch計測時間=104[usec] , タッチなし＝3000,タッチあり=10000
//	CTSU.CTSUCRB.BIT.PRRATIO = 0x0;   // 疑似乱数更新周期設定ビット
//	CTSU.CTSUCRB.BIT.PRMODE = 0x11;   // 疑似乱数生成周期設定ビット (3周期=6パルス)
	
	
	
	CTSU.CTSUCRB.BIT.SOFF = 0x0;	  // 0=周波数拡散機能 ON
	CTSU.CTSUCRB.BIT.PROFF = 0x0;	  // 0=疑似乱数制御を行う
	
	CTSU.CTSUCRB.BIT.SST = 0x10;	  // センサ安定待ち時間設定ビット 34サイクル (*注) 
	
	CTSU.CTSUCRB.BIT.SSMOD = 0x0;	 // SUCLK拡散モード選択ビット( CTSUbには無い)
	CTSU.CTSUCRB.BIT.SSCNT = 0x0;    // SUCLK拡散制御ビット( CTSUbには無い)
}


// CTSU 有効チャンネルの設定
//
// CTSU計測対象: 
// SW1: PC6 : TS14  
// SW2: PC5 : TS15  
//

void ctsu_ch_enable(void)
{
	CTSU.CTSUCHACA.BIT.CHAC14 = 1;	// TS14 有効
	CTSU.CTSUCHACA.BIT.CHAC15 = 1;	// TS15 有効	
	
	
	CTSU.CTSUCHTRCA.BIT.CHTRC14 = 0; // TS14 受信
	CTSU.CTSUCHTRCA.BIT.CHTRC15 = 0; // TS15 受信
}


//
//  タッチセンサ用 TSCAP(PC4)の放電  
//  TSCAP(PC4)を出力ポートとして Low を一定期間出力し、すでに充電されているLPF 容量を放電	
//

void tscap_clear(void)
{
	PORTC.PMR.BIT.B4 = 0;	// 汎用入出力ポート
	
	PORTC.PDR.BIT.B4 = 1;   // 出力ポート

	PORTC.PODR.BIT.B4 = 0;  // 出力 0 
	
	delay_msec(100);	//　100msec待つ  
}



//
// 　タッチセンサ用 I/O ポート設定
//    
// SW1: PC6 : TS14  
// SW2: PC5 : TS15  
//      PC4 : TSCAP 
//

void touch_port_set(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;  	 // マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
    	 
	MPC.PC6PFS.BYTE = 0x19;		// PC6 = TS14
	MPC.PC5PFS.BYTE = 0x19;		// PC5 = TS15
	MPC.PC4PFS.BYTE = 0x19;		// PC4 = TSCAP
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定

	
	PORTC.PMR.BIT.B6 = 1;          // PC6:周辺機能ポートとして使用
	PORTC.PMR.BIT.B5 = 1;          // PC5:周辺機能ポートとして使用
	
	PORTC.PMR.BIT.B4 = 1;          // PC4:周辺機能ポートとして使用
}


