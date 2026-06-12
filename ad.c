#include <machine.h>
#include "iodefine.h"
#include "misratypes.h"
#include "ad.h"

#include  "debug.h"

volatile uint16_t ad_cnt[3];

// 
// AD スキャン終了割り込み (S12ADI0)
// 
#pragma interrupt (Excep_S12AD_S12ADI0(vect=102))
void Excep_S12AD_S12ADI0(void){

		
	ad_cnt[0] = S12AD.ADDR0;
	ad_cnt[1] = S12AD.ADDR1;
	ad_cnt[2] = S12AD.ADDR2;
	
	
}






//   A/D コンバータ(12 bit ) の設定
//
// S12AD.ADSSTR0:  アナログ入力のサンプリング時間の設定を行います。
// 1 ステート = 1 ADCLK (A/D 変換クロック) 幅でADCLK クロックが48 MHz であれば1 ステート = 20.8 ns
//になります。初期値は13 ステートです。アナログ入力信号源のインピーダンスが高くサンプリング時間が
// 不足する場合や、ADCLK クロックが低速な場合に、サンプリング時間を調整
// 
void initAD(void)
{
	ad_port_set();			//  A/D コンバータへの入力用ポート設定
	
	S12AD.ADANSA0.BIT.ANSA000 = 1;	// AN000 を変換対象とする。
	S12AD.ADANSA0.BIT.ANSA001 = 1;	// AN001 を変換対象とする。
	S12AD.ADANSA0.BIT.ANSA002 = 1;	// AN002 を変換対象とする。
	
	S12AD.ADCSR.BIT.EXTRG = 0;	// 同期トリガによるA/D変換の開始を選択
	S12AD.ADCSR.BIT.TRGE = 1;	// 同期、非同期トリガによるA/D変換の開始を許可
	S12AD.ADSTRGR.BIT.TRSA = 9;     // 同期トリガ = ELC 

	S12AD.ADCSR.BIT.ADIE = 1;	// スキャン終了後のS12ADI0割り込み発生の許可
	S12AD.ADCSR.BIT.ADCS = 0;	// シングルスキャンモード
			
	IPR(S12AD,S12ADI0) = 6;		// 割り込みレベル = 6　　（15が最高レベル)
	IEN(S12AD,S12ADI0) = 1;		// 割込み処理許可
	
}


// A/D コンバータへの入力用ポート設定
//  AN000:P40    モータ電流測定用抵抗
//  AN001:P41    Motor A 速度調整用用ボリューム
//  An002:P42　　Motor B 速度調整用用ボリューム
//
void ad_port_set(void)
{	
	MPC.PWPR.BIT.B0WI = 0;  	 // マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
	
	MPC.P40PFS.BIT.ASEL = 1;	// P40 アナログ端子として使用する
	MPC.P41PFS.BIT.ASEL = 1;	// P41 アナログ端子として使用する
	MPC.P42PFS.BIT.ASEL = 1;	// P42 アナログ端子として使用する
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定
	
	PORT4.PMR.BIT.B0 = 1;		// P40 周辺機能ポートとして使用
	PORT4.PMR.BIT.B1 = 1;		// P41 周辺機能ポートとして使用
	PORT4.PMR.BIT.B2 = 1;		// P42 周辺機能ポートとして使用
}