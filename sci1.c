#include "iodefine.h"
#include "misratypes.h"
#include  "mtu.h"
#include "ad.h"
#include "ctsu.h"
#include "dtc.h"
#include "control.h"
#include "temp_cal.h"
#include "sci1.h"
#include "crc_16.h"
//
//  SCI1 シリアル通信(調歩同期)処理
//
// パソコンとの通信　受信用
volatile uint8_t  rcv_data[16];
volatile uint8_t rxdata;
volatile uint8_t rcv_over;
volatile uint8_t  rcv_cnt;

// パソコンとの通信 送信用
volatile uint8_t sd_data[64];
volatile uint8_t  send_cnt;
volatile uint8_t  send_pt;


//
// SCI1 受信データフル割り込み
//  受信データは、DTCにより受信バッファへ格納される。
//  受信する全データ(8byte固定)が受信バッファへ格納された後、この割り込み処理が実行される。
//　
//   
#pragma interrupt (Excep_SCI1_RXI1(vect=219))
void Excep_SCI1_RXI1(void){

	uint32_t i;
	
						// DTC 転送情報の再設定 (ノーマル転送)	
	SCI1_RCV_DTC.DAR = &rcv_data[0];	 // デスティネーションアドレス
	SCI1_RCV_DTC.CRA = 0x0008;		 // 転送回数 8回 ( 8 byte)  コマンドは、8byte固定
	
	ICU.DTCER[VECT(SCI1,RXI1)].BIT.DTCE = 1; //  SCI 1 受信データフル割り込み(RXI1(vect=219)発生時、DTC起動
	
	Init_CRC();		// CRC演算器の初期化
	
	for ( i = 0 ; i < 8 ; i++ ) {	// CRC計算
		CRC.CRCDIR = rcv_data[i];
	}
	
	if ( CRC.CRCDOR == 0 ) {   // CRCの演算結果 OKの場合、レスポンスを返す
		rcv_over = 1;      // 受信完了フラグのセット
		crc_16_err = 0;
		
	}
	else {   		   // CRCの演算結果 NGの場合、レスポンスを返さない
		crc_16_err = 1;
	}
	
	
}

//
//  SCI1　送信データエンプティ割り込み
//   DTCにより送信バッファから、トランスミットデータレジスタ(TDR)へ書き込まれる。
//  全データ書き込み後、この割り込みが実行される。
//
#pragma interrupt (Excep_SCI1_TXI1(vect=220))
void Excep_SCI1_TXI1(void){
	
	SCI1.SCR.BIT.TEIE = 1;  	// TEI割り込み(送信終了割り込み)許可
	
}

//
// SCI1 送信終了割り込み
//
#pragma interrupt (Excep_SCI1_TEI1(vect=221))
void Excep_SCI1_TEI1(void)
{	 
	SCI1.SCR.BIT.TE = 0;            // 送信禁止
        SCI1.SCR.BIT.TIE = 0;           // 送信割り込み禁止	        
	SCI1.SCR.BIT.TEIE = 0;  	// TEI割り込み(送信終了割り込み)禁止
 }
 


//
//  コマンド受信時の処理 
//
void sci1_resp_make(void){
	
	if ( rcv_data[0] == 0x03 ){  // パラメータ書き込みコマンド受信
	        control_para_write();
	}
	else if ( rcv_data[0] == 0x10 ) {	//  CTSUセンサカウント値　読み出しコマンド受信
		ctsu_resp_make();
	}
	else if (  rcv_data[0] == 0x20 ) {  //  制御データ　読み出しコマンド受信
		control_resp_make();
	}
	
	set_dtc_sci1_snd();	// 送信用 DTC 設定

    			           // 一番最初の送信割り込み(TXI)を発生させる処理
	SCI1.SCR.BIT.TIE = 1;      // 送信割り込み許可
	SCI1.SCR.BIT.TE = 1;	   // 送信許可

}

//
// パラメータ書き込とレスポンス作成
//
// SV :0～4000 [rpm]　（目標回転速度)
// Kp :0.1～25.5
// Ti :0.1～25.5[sec]
//
// 受信データ:
// rcv_data[0]  0x03 :パラメータ書き込みコマンド 
//         [1]  SV値　下位バイト
//         [2]  SV値  上位バイト
//         [3]  Kp (10倍した値)   Kp=0.1ならば、1を設定
//         [4]  Ti (10倍した値)   Ti=0.1[sec]ならば、1を設定
//         [5]  dummy data 0x00
//         [6]  CRC 上位バイト
//         [7]  CRC 下位バイト
//
// 送信データ:
//     sd_data[0] :  0x83 (パラメータ書き込みコマンドのレスポンス)
//     sd_data[1] : dummy 0x00
//     sd_data[2] : CRC 上位バイト
//     sd_data[3] :  CRC 下位バイト   

void control_para_write(void)
{
	uint16_t t_sv;
	uint16_t crc_cd;
	
	t_sv = rcv_data[2];
	
	pid_sv_rpm = (( t_sv << 8 ) | rcv_data[1] );  // SV
		
	pid_kp = rcv_data[3] / 10.0;    // Kp
	pid_i = rcv_data[4] / 10.0;     // I 
	
	send_cnt = 4;			// 送信バイト数

	sd_data[0] = 0x83;
	sd_data[1] = 0x00;

	crc_cd = cal_crc_sd_data( send_cnt - 2 );   // CRCの計算
	
	sd_data[2] = crc_cd >> 8;	// CRC上位バイト
	sd_data[3] = crc_cd;		// CRC下位バイト

	
}


//  制御データの読み出しコマンドのレスポンス作成
//
// 受信データ:
//  rcv_data[0];　0x20 (コマンド)
//  rcv_data[1]:  dummy 0
//  rcv_data[2]:  dummy 0
//  rcv_data[3]:  dummy 0
//  rcv_data[4]:  dummy 0
//  rcv_data[5]:  dummy 0
//  rcv_data[6]: CRC(上位バイト側)
//  rcv_data[7]: CRC(下位バイト側)

// 送信データ :capt_data
//     sd_data[0] :  0xa0 (制御データ読み出しコマンドのレスポンス)
//     sd_data[1] : mode_run_stop  (1 = Run , 0 = Stop )
//     sd_data[2] : mode_auto_manual  ( 1 = Auto, 0 = Manual ) 
//     sd_data[3] :  キャプチャ未発生フラグ          
//     sd_data[4] :  回転速度[rpm] 測定値(PV) Lowバイト側   
//     sd_data[5] :             :             Highバイト側        
//     sd_data[6] :  目標回転速度[rpm} 設定値(SV) Lowバイト側
//     sd_data[7] :            　:                Highバイト側
//     sd_data[8] :　出力(操作量)(MV)(0～100[%]) 
//     sd_data[9] :  比例ゲイン Kp (0.1～25.5) (10倍した値 1ならば0.1)
//     sd_data[10]:  積分時間(I) (0.1～25,5[sec])(10倍した値 1ならば0.1)
//     sd_data[11]:  ダミー(0x00)
//     sd_data[12] :モータA 電流測定用ADカウント値(AN000) ad_cnt[0] の Lowバイト側
//     sd_data[13] :               :                                   Highバイト側
//     sd_data[14] :モータB 速度調整ボリューム ADカウント値 (AN001) ad_cnt[1]のLowバイト側
//     sd_data[15] :              : 					       Highバイト側	 
//     sd_data[16] :モータA 速度調整ボリューム ADカウント値 (AN002) ad_cnt[2]のLowバイト側
//     sd_data[17] :              : 					       Highバイト側 
//     sd_data[18] :  SLG47011  ch0_open_err  ch0 熱電対 断線時 = 1
//     sd_data[19] :  SLG47011  ch1_opne_err  ch1 熱電対 断線時 = 1
//     sd_data[20] :SLG47011 Ch0 温度 (Low  byte側) 10倍した値(300なら、30.0[℃])     
//     sd_data[21] :      :           (High byte側)
//     sd_data[22] :SLG47011 Ch1 温度 (Low  byte側) 10倍した値(300なら、30.0[℃])        
//     sd_data[23] :      :	      (High byte側)
//     sd_data[24] :SLG47011 Ch2 温度 (Low  byte側) (未使用)       :           
//     sd_data[25] :      :           (High byte側)
//     sd_data[26] :SLG47011 Ch3 温度 (Low  byte側) 10倍した値(300なら、30.0[℃])      
//     sd_data[27] :      :           (High byte側)
//     sd_data[28] : CRC 上位バイト 
//     sd_data[29] : CRC 下位バイト

void control_resp_make(void)
{
	uint16_t pv_rpm;
	uint16_t sv_rpm;
	
	uint16_t slg_ch0;
	uint16_t slg_ch1;
	uint16_t slg_ch2;
	uint16_t slg_ch3;

	uint16_t crc_cd;
	
	float  percent_mv;
	
	send_cnt = 30;			// 送信バイト数

	sd_data[0] = 0xa0;
	sd_data[1] = mode_run_stop;
	sd_data[2] = mode_auto_manual;
	sd_data[3] = capt_ovrflow_flag;
	
	pv_rpm = pid_pv_rpm;
	sv_rpm = pid_sv_rpm;
	
	sd_data[4] = pv_rpm;
	sd_data[5] = pv_rpm >> 8;
	
	sd_data[6] = sv_rpm;
	sd_data[7] = sv_rpm >> 8;
	
	percent_mv = (pid_mv * 100.0);
	
	sd_data[8] = percent_mv;
	
	sd_data[9] = (pid_kp * 10.0);
	sd_data[10] = (pid_i * 10.0);
	
	sd_data[11] = 0x00;
	
	sd_data[12] = ad_cnt[0];
	sd_data[13] = ( ad_cnt[0] >> 8 );
	
	sd_data[14] = ad_cnt[1];
	sd_data[15] = ( ad_cnt[1] >> 8 );
	
	sd_data[16] = ad_cnt[2];
	sd_data[17] = ( ad_cnt[2] >> 8 );
	
	sd_data[18] = ch0_open_err;
	sd_data[19] = ch1_open_err;
	
	slg_ch0 = slg_ch0_temp * 10.0;
	slg_ch1 = slg_ch1_temp * 10.0;
	slg_ch2 = 0;
	slg_ch3 = slg_ch3_temp * 10.0;
		
	sd_data[20] = slg_ch0;
	sd_data[21] = slg_ch0 >> 8;
	
	sd_data[22] = slg_ch1;
	sd_data[23] = slg_ch1 >> 8;
	
	sd_data[24] = slg_ch2;
	sd_data[25] = slg_ch2 >> 8;
	
	sd_data[26] = slg_ch3;
	sd_data[27] = slg_ch3 >> 8;
	
	crc_cd = cal_crc_sd_data( send_cnt - 2 );   // CRCの計算
	
	sd_data[28] = crc_cd >> 8;	// CRC上位バイト
	sd_data[29] = crc_cd;		// CRC下位バイト
	
}



//
//  CTSUセンサカウント値読み出しコマンドのレスポンス作成
//
// 受信データ:
//  rcv_data[0];　0x10 (コマンド)
//  rcv_data[1]:  dummy 0
//  rcv_data[2]:  dummy 0
//  rcv_data[3]:  dummy 0
//  rcv_data[4]:  dummy 0
//  rcv_data[5]:  dummy 0
//  rcv_data[6]: CRC(上位バイト側)
//  rcv_data[7]: CRC(下位バイト側)
//
// 送信データ :

//     sd_data[0];　0x90 (コマンドに対するレスポンス)
//     sd_data[1]:  dummy 0
//     sd_data[2] : TS14用 CTSUステータスレジスタ ctsu_sr(bit0-bit7) 
//     sd_data[3] :             :                        (bit8-bit15)
//     sd_data[4] : TS15用 CTSUステータスレジスタ ctsu_sr(bit0-bit7) 
//     sd_data[5] :             :                        (bit8-bit15)
//     sd_data[6] : TS14用 CTSUセンサオフセットレジスタ ctsu_so (bit0-bit7) 
//     sd_data[7] :             :                                (bit8-bit15)
//     sd_data[8] :             :                                (bit16-bit23)
//     sd_data[9] :             :                                (bit17-bit31)
//     sd_data[10] : TS15用 CTSUセンサオフセットレジスタ ctsu_so(bit0-bit7)
//     sd_data[11] :             :                                (bit8-bit15)
//     sd_data[12] :             :                                (bit16-bit23)
//     sd_data[13] :             :                                (bit17-bit31)
//     sd_data[14] :TS14用 CTSUセンサカウンタ ctsu_scnt (bit0-bit7)
//     sd_data[15] :             :                      (bit8-bit15)
//     sd_data[16] :                                    (bit16-bit23)
//     sd_data[17] :             :                      (bit24-bit31)
//     sd_data[18] :TS15用 CTSUセンサカウンタ ctsu_scnt (bit0-bit7)
//     sd_data[19] :             :                      (bit8-bit15)
//     sd_data[20] :                                    (bit16-bit23)
//     sd_data[21] :             :                      (bit24-bit31)
//     sd_data[22] : CRC 上位バイト 
//     sd_data[23] : CRC 下位バイト


void  ctsu_resp_make(void)
{
	uint16_t crc_cd;
	
	send_cnt = 24;		// 送信バイト数
	
	sd_data[0] = 0x90;	// レスポンス
	sd_data[1] = 0x00;
	
	sd_data[2] = ctsu_sr[0];	// TS14 用
	sd_data[3] = (ctsu_sr[0] >> 8);
	
	sd_data[4] = ctsu_sr[1];	// TS15 用
	sd_data[5] = (ctsu_sr[1] >> 8);
		
	sd_data[6] = ctsu_so[0];
	sd_data[7] = (ctsu_so[0] >> 8);
	sd_data[8] = (ctsu_so[0] >> 16);
	sd_data[9] = (ctsu_so[0] >> 24);

	sd_data[10] = ctsu_so[1];
	sd_data[11] = (ctsu_so[1] >> 8);
	sd_data[12] = (ctsu_so[1] >> 16);
	sd_data[13] = (ctsu_so[1] >> 24);

	sd_data[14] = ctsu_scnt[0];
	sd_data[15] = (ctsu_scnt[0] >> 8);
	sd_data[16] = (ctsu_scnt[0] >> 16);
	sd_data[17] = (ctsu_scnt[0] >> 24);

	sd_data[18] = ctsu_scnt[1];
	sd_data[19] = (ctsu_scnt[1] >> 8);
	sd_data[20] = (ctsu_scnt[1] >> 16);
	sd_data[21] = (ctsu_scnt[1] >> 24);
	
	crc_cd = cal_crc_sd_data( send_cnt - 2 );   // CRCの計算
	
	sd_data[22] = crc_cd >> 8;	// CRC上位バイト
	sd_data[23] = crc_cd;		// CRC下位バイト
	
}


// 　送信データのCRC計算
//   sd_data[0]からnum個のデータのCRCを計算する。
//
uint16_t    cal_crc_sd_data( uint16_t num )
{
	uint16_t  crc;
	
	uint32_t i;

	Init_CRC();			// CRC演算器の初期化
	
	for ( i = 0 ; i < num ; i++ ) {	// CRC計算
	
		CRC.CRCDIR = sd_data[i];
	}
	
	crc = CRC.CRCDOR;        // CRCの演算結果
	
	return crc;
}



// 
// SCI1 初期設定
//  8bit-non parity-1stop
//  PCLKB = 24[MHz]
//  
// 1) BDGM=0,ABCS=0,ABCSE=0 の場合 (SCI1.SEMR.BIT.BGDM = 0 , SCI1.SEMR.BIT.ABCS = 0, SCI1.SEMR.BIT.ABCSE = 0;)      
//  　
//    N = {(24 x 1000000)/((64/2) x B)} - 1
//    N: BRRレジスタの値,　B: ボーレート bps
//
//    N = 0 とすると、
//    B =(24/32) x 1000000 = 0.75[Mbps] = 750[Kbps]
//
//
//  2) BDGM=1,ABCS=0,ABCSE=0の場合、     
//   N = [(24 x 1000000)/((32/2) x B)] - 1
//    
//   N = 0 とすると、B= 1.5 [Mbps]
//
//  3) BDGM=1,ABCS=1,ABCSE=0の場合、     
//   N = [(24 x 1000000)/((16/2) x B)] - 1
//    
//   N = 0 とすると、B= 3 [Mbps]
//
//  4) BDGM=任意,ABCS=任意,ABCSE=1の場合、     
//   N = [(24 x 1000000)/((12/2) x B)] - 1
//    
//   N = 0 とすると、B= 4 [Mbps]
//
//参考:「RX140グループユーザーズマニュアル　ハードウェア編」( R01UH0905JJ0120 Rev.1.20 ) 2024.11.22
// 	表27.11 BRRレジスタの設定値NとビットレートBの関係(SCI6, SCI8, SCI9, SCI12) 
// 
//

void SCI_1_Reg_Set(void)
{
	
	SCI1.SCR.BYTE = 0;	// 内蔵ボーレートジェネレータ、送受信禁止
	
	SCI1.SMR.BYTE = 0;	// PCLKB(=24MHz), 調歩同期,8bit,parity なし,1stop
	
	
//	SCI1.BRR = 0;			// 0.75 [Mbps] = 750[Kbps]
//	SCI1.SEMR.BIT.BGDM = 0;         // 0: ボーレートジェネレータから通常の周波数のクロックを出力
//	SCI1.SEMR.BIT.ABCS = 0;         // 0: 基本クロック16サイクルの期間が1ビット期間の転送レートになります
//	SCI1.SEMR.BIT.ABCSE =0;		// 0:1ビット期間の転送レートはBGDMビットとABCSビットの設定に従う
	
	SCI1.BRR = 0;			// 1.5 [Mbps] 
	SCI1.SEMR.BIT.BGDM = 1;         // 1= ボーレートジェネレータから2倍の周波数のクロックを出力
	SCI1.SEMR.BIT.ABCS = 0;         // 0= 基本クロック16サイクルの期間が1ビット期間の転送レートになります	
	SCI1.SEMR.BIT.ABCSE =0;		// 0:1ビット期間の転送レートはBGDMビットとABCSビットの設定に従う
	
//	SCI1.BRR = 0;			// 3 [Mbps] 
//	SCI1.SEMR.BIT.BGDM = 1;         // 1= ボーレートジェネレータから2倍の周波数のクロックを出力
//	SCI1.SEMR.BIT.ABCS = 1;         // 1= 基本クロック8サイクルの期間が1ビット期間の転送レートになります	
//	SCI1.SEMR.BIT.ABCSE =0;		// 0:1ビット期間の転送レートはBGDMビットとABCSビットの設定に従う
	
//	SCI1.BRR = 0;			// 4 [Mbps] 
//	SCI1.SEMR.BIT.BGDM = 0;         // 任意
//	SCI1.SEMR.BIT.ABCS = 0;         // 任意
//	SCI1.SEMR.BIT.ABCSE =1;		// 1:基本クロック6サイクルの期間が1ビット期間の転送レートになります
	
	
	rcv_cnt = 0;			// 受信バイト数の初期化
	//Init_CRC();			// CRC演算器の初期化
}




//
// SCI1 の割り込み用、割り込みコントローラ、SCIモジュール側の設定
//  
void SCI1_Init_interrupt(void)
{
	IPR(SCI1,RXI1) = 12;		// 受信割り込み 割り込みレベル = 12 （15が最高レベル)
	IR(SCI1,RXI1) = 0;		// 割り込み要求のクリア
	IEN(SCI1,RXI1) = 1;		// 受信割り込み許可
	
	IPR(SCI1,TXI1) = 12;		// 送信割り込みレベル = 12 （15が最高レベル)
	IR(SCI1,TXI1) = 0;		// 割り込み要求のクリア
	IEN(SCI1,TXI1) = 1;		// 送信割り込み許可
	
	IPR(SCI1,TEI1) = 12;		// 送信完了 割り込みレベル = 12 （15が最高レベル)
	IR(SCI1,TEI1) = 0;		// 送信完了割り込み要求のクリア
	IEN(SCI1,TEI1) = 1;		// 送信完了割り込み許可
	
	SCI1.SCR.BIT.TIE = 0;		// TXI割り込み要求を 禁止
	SCI1.SCR.BIT.RIE = 1;		// RXIおよびERI割り込み要求を 許可
	SCI1.SCR.BIT.TE = 0;		// シリアル送信動作を 禁止　
	SCI1.SCR.BIT.RE = 1;		// シリアル受信動作を 許可
	
	SCI1.SCR.BIT.MPIE = 0;         // (調歩同期式モードで、SMR.MPビット= 1のとき有効)
	SCI1.SCR.BIT.TEIE = 0;         // TEI割り込み要求を禁止
	SCI1.SCR.BIT.CKE = 0;          // 内蔵ボーレートジェネレータ
	
}


//    SCI 1 インターフェイス用のポートを設定
//
//      P30 = RXD1  
//   	P26 = TXD1 
//
//  
void SCI_1_Port_Set(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;   // マルチファンクションピンコントローラ　プロテクト解除
	MPC.PWPR.BIT.PFSWE = 1;  // PmnPFS ライトプロテクト解除
	
	MPC.P30PFS.BYTE = 0x0A;  // P30 = RXD1
	MPC.P26PFS.BYTE = 0x0A;  // P26 = TXD1
	
	
	MPC.PWPR.BYTE = 0x80;    //  PmnPFS ライトプロテクト 設定
			
	PORT3.PMR.BIT.B0 = 1;	// P30 周辺モジュールとして使用
	PORT2.PMR.BIT.B6 = 1;   // P26 周辺モジュールとして使用
	
}

