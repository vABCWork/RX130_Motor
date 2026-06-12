
#include "iodefine.h"
#include "misratypes.h"
#include "ad.h"
#include "ctsu.h"
#include "sci1.h"
#include "dtc.h"

struct dtcreg_full SCI1_RCV_DTC;    // 転送情報 (SCI1 受信)
struct dtcreg_full SCI1_SND_DTC;    // 転送情報 (SCI1 送信)




//
//  DTC 初期設定
//  ROM: 64KB : 0xffff 0000 ～ 0xffff ffff
//  
//  DTCベクタ 先頭アドレス : 0xffff fb00
//
void initDTC(void)
{
	DTC.DTCVBR = ( void *)0xfffff800;    // DTCベクタ 先頭アドレス
	
	DTC.DTCADMOD.BIT.SHORT = 0;	// フルアドレスモード
	
	set_dtc_sci1_rcv();		// SCI1 受信用 DTC設定
	

	
	DTC.DTCST.BIT.DTCST = 1;	// DTC転送許可
	
}




//
//  SCI1 送信用 DTC設定  (送信データ作成後に実行)
//  「送信データエンプティ割り込み(TXI1)(vect=220)」により、DTC起動。
//    sd_data[] を SCI1.TDR へ書き込み
//
//  転送モード: ノーマル転送
//  転送元(アドレスインクリメント) :  SCI1　送信バッファ sd_data             
//  転送先(アドレス固定) :  SCI1.TDR   
//  転送size   :バイト転送(8bit)
//  転送回数: 送信バイト数　
//            転送回数終了後、CPU割り込み(送信データエンプティ割り込み(TXI1)(vect=220))発生用に設定
//
//  ノーマル転送: 1 回の転送要求で、1 バイト、1 ワードまたは1 ロングワードの転送を行います。転送回数は1 ～ 65536です。
//                転送元アドレスと転送先アドレスは、インクリメント、デクリメント、または固定にそれぞれ設定できます。
//                
//

void  set_dtc_sci1_snd(void)
{
	SCI1_SND_DTC.MRA = 0x08;	// ノーマル転送、バイト(8 bit)転送、SAR転送後インクリメント、 データ転送終了時、転送情報をライトバックする
				
	SCI1_SND_DTC.MRB = 0x00;	// DARレジスタ固定、指定した回数のデータ転送が終了したとき、CPUへの割り込み要求発生
	
	SCI1_SND_DTC.SAR = &sd_data[0];	 // ソースアドレス 
	SCI1_SND_DTC.DAR = &SCI1.TDR;	 // デスティネーションアドレス
	SCI1_SND_DTC.CRA = send_cnt;      // 転送回数 設定  
	
	ICU.DTCER[VECT(SCI1,TXI1)].BIT.DTCE = 1; //  SCI1 送信データエンプティ割り込み TXI1(vect=220)A発生時、DTC起動
}


//
//  SCI1 受信用 DTC設定
//  「受信データフル割り込み(RXI1)(vect=219)」により、DTC起動。
//    SCI1.RDR を rcv_data[]へ転送
//
//  転送モード: ノーマル転送
//  転送元(アドレス固定) :  SCI1のレシーブデータレジスタ SCI1.RDR
//  転送先(アドレスインクリメント) :  SCI1 受信バッファ rcv_data
//  転送size   :バイト転送(8bit)
//  転送回数: 8回　
//            8byte受信終了後、CPU割り込み(受信データフル割り込み(RXI1)(vect=219))発生用に設定
//            コマンド長は、8byte固定
//
//  ノーマル転送: 1 回の転送要求で、1 バイト、1 ワードまたは1 ロングワードの転送を行います。転送回数は1 ～ 65536です。
//                転送元アドレスと転送先アドレスは、インクリメント、デクリメント、または固定にそれぞれ設定できます。
//                
//

void  set_dtc_sci1_rcv(void)
{
	SCI1_RCV_DTC.MRA = 0x04;	// ノーマル転送、バイト(8 bit)転送、SAR固定、 データ転送終了時、転送情報をライトバックする
				
	SCI1_RCV_DTC.MRB = 0x08;	// 転送後DARレジスタをインクリメント、指定した回数のデータ転送が終了したとき、CPUへの割り込み要求発生
	
	SCI1_RCV_DTC.SAR = &SCI1.RDR;	 // ソースアドレス 
	SCI1_RCV_DTC.DAR = &rcv_data[0];	 // デスティネーションアドレス
	SCI1_RCV_DTC.CRA = 0x0008;	 // 転送回数 8回  
	
	ICU.DTCER[VECT(SCI1,RXI1)].BIT.DTCE = 1; //  SCI 1 受信データフル割り込み(RXI1(vect=219)発生時、DTC起動
}






//  DTCベクタテーブルへ、転送情報の格納アドレスを登録する処理
// 
//・ DTCベクタの先頭アドレスは、0xffff f800  (DTC.DTCVBR)
//
//
// 1) SCI1 受信データフル割り込み(RXI1)(vect=219)」
//  アドレス:  0xffff fb6c  ( 0xffff f800 + 219*4(=876,0x36c) )
//   転送情報: 転送情報 SCI1_RCV_DTCのアドレス
//
// 2)  SCI1送信データエンプティ割り込み(TXI1)(vect=220)
//  アドレス:  0xffff fb70  ( 0xffff f800 + 220*4(=880,0x370) )
//   転送情報: 転送情報 SCI1_SND_DTCのアドレス
//
//
// ・リンカオプションのセクション設定で転送情報を DTCベクタテーブルに設定する。
//　SCI1 受信データフル割り込みの場合、
//    0xffff fb6c に DTCTBL_SCI1_RCV　を割りるける。
//
// 　section D で　アライメント数=4 , 初期化データ領域(ROM)



// 0xffff fb6c に配置
#pragma section D DTCTBL_SCI1_RCV
void *DTC_Vector_SCI1_RCV = { &SCI1_RCV_DTC };

// 0xffff fb70 に配置
#pragma section D DTCTBL_SCI1_SND
void *DTC_Vector_SCI1_SND = { &SCI1_SND_DTC };




