#include	<machine.h>
#include	<_h_c_lib.h>

#include	 "iodefine.h"
#include	 "misratypes.h"

#include	"delay.h"

#define PSW_init       0x00010000	// PSW bit pattern
#define FPSW_DPSW_init 0x00000000	// FPSW/DPSW bit base pattern


static void operating_frequency_set (void);

#pragma stacksize si=0x200	// 割り込みスタックの定義　(ユーザスタックサイズは使用しない。)

#define PSW_init       0x00010000	// プロセッサステータスワード (割り込み許可)


#pragma section ResetPRG		// output PowerON_Reset to PResetPRG section

#pragma entry PowerON_Reset_PC

void PowerON_Reset_PC(void)
{
	set_extb(__sectop("EXCEPTVECT")); // CPUがRXV2アーキテクチャの場合、例外ベクタテーブルをEXTB(例外テーブルレジスタ)へセット
	set_intb(__sectop("C$VECT"));     // 可変ベクターテーブルを、INTBレジスタにセット

	operating_frequency_set();    // システムクロック 等の設定
	
	_INITSCT();			// 静的変数の初期化 (初期値なしの静的変数=0, 初期値付きの静的変数=初期値)

	
	set_psw(PSW_init);		// 割り込み許可
	
	main();				// メインの起動

	brk();
}




//  クロックの設定
// 設定内容:
//    システムクロック(ICLK) = 48 MHz
//    周辺モジュールクロック(PCLKB) = 24 MHz (S12AD以外の周辺モジュール用)
//    周辺モジュールクロック(PCLKD) = 48 MHz (S12AD A/Dコンバータ用)
//    フラッシュIFクロック(FCLK) = 48 MHz
//
//　・システムクロックコントロールレジスタ(SCKCR)
//   b31-b28: FlashIFクロック(FCLK)選択ビット : 0000 :1分周 
//   b27-b24: システムクロック(ICLK)選択ビット: 0000 :1分周
//   b23-b12: all 0
//   b11-b8: 周辺モジュールクロックB (PCLKB)選択ビット :0001 :2分周
//   b7-b4:   all 0
//   b3-b0:  周辺モジュールクロックD (PCLKD)選択ビット: 0000 :1分周
//
//　・ オプション機能選択レジスタ1 (OFS1) = 0xfff fcfff 
//       リセット後、HOCO発振が無効
//　　　　HOCO周波数は、48[MHz]
//

static void operating_frequency_set (void)
{
	
	SYSTEM.PRCR.WORD = 0xA50F;	// クロック発生回路関連レジスタの書き込み許可
	
	SYSTEM.HOCOCR.BIT.HCSTP = 0;	// HOCO動作
	while((SYSTEM.OSCOVFSR.BIT.HCOVF) == 0){	//  高速オンチップオシレータ HOCO発振安定待ち
	}
	
					// 11.2.6 動作電力コントロールレジスタ(OPCCR)
	SYSTEM.OPCCR.BIT.OPCM = 0;	// 高速動作モード
	while( SYSTEM.OPCCR.BIT.OPCMTSF == 1 ){  // 遷移待ち
	}
	
					     // 9.2.1 システムクロックコントロールレジスタ(SCKCR)
	SYSTEM.SCKCR.LONG = 0x00000100;	    // システムクロックの設定 (FCLK=48MHz,ICLK=48MHz,  PCLKB=24MHz ,PCLKD=48MHz)					  
	while( SYSTEM.SCKCR.LONG != 0x00000100 ){
	}
	
	
	SYSTEM.SCKCR3.WORD = 0x0100;		// Clock sourceは、HOCO選択
	while( SYSTEM.SCKCR3.WORD != 0x0100){
	}
	
	SYSTEM.PRCR.WORD = 0xA500;	// クロック発生回路関連レジスタへの書き込み禁止
}

