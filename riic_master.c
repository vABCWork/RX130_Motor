#include <machine.h>
#include "iodefine.h"
#include "misratypes.h"
#include "riic_master.h"



uint8_t iic_slave_adrs;  // IIC スレーブアドレス  00: 7bitアドレス( 例:100 0000 = 0x40 )

volatile uint8_t iic_rcv_data[16];   // IIC受信データ
volatile uint8_t iic_sd_data[32];    // 送信データ

volatile uint8_t iic_sd_pt;	    // 送信データ位置
volatile uint8_t iic_rcv_pt;         // 受信データ位置

volatile uint8_t  iic_com_over_fg;  // 1:STOPコンディションの検出時

volatile uint8_t  dummy_read_fg;    // 受信割り込みで、ダミーリードするためフラグ



// RIIC0 EEI0
// 通信エラー/通信イベント発生
//( アービトレーションロスト検出、NACK 検出、タイムアウト検出、スタートコンディション検出、ストップコンディション検出)
//
//   アービトレーションロスト検出、とタイムアウト検出は、使用していない。
//

#pragma interrupt (Excep_RIIC0_EEI0(vect=246))
void Excep_RIIC0_EEI0(void){
	
	if( RIIC0.ICSR2.BIT.START == 1 ) {      // スタート(リスタート)コンディション検出
		RIIC0.ICSR2.BIT.START = 0;	// スタートコンディション検出フラグのクリア
		
		if ( iic_sd_pt == 3) {         //  読み出しアドレス(b7-b0))送信完了後の、リスタートコンディション発行
			
			RIIC0.ICDRT = iic_sd_data[iic_sd_pt];  // 送信 ( スレーブアドレス(読み出し用)の送信 )
			
			iic_sd_pt++;
			
			// スレーブアドレス(読み出し用)の送信後に、ICCR2.TRS = 0(受信モード)となり、
			// ICSR2.RDRF (受信データフルフラグ)は自動的に“1”(ICDRRレジスタに受信データあり)になる。
			// スレーブアドレス(読み出し用)送信後の、受信割り込みで、ダミーリードするためのフラグを設定
			 
			 dummy_read_fg = 1;    // ダミーリード有効
		}	
	}
	
	else if ( RIIC0.ICSR2.BIT.STOP == 1 ) {      // STOP 検出
	
	      RIIC0.ICSR2.BIT.STOP = 0;	 //  STOP 検出フラグのクリア
    	      
	      iic_com_over_fg = 1;		// 通信完了
	      
	}
	
	else if ( RIIC0.ICSR2.BIT.NACKF == 1 ) {      // NACK 検出
	        
		RIIC0.ICSR2.BIT.NACKF = 0;	  // NACK 検出フラグのクリア
	        
		RIIC0.ICCR2.BIT.SP = 1;		   // ストップコンディションの発行要求の設定
	}
	
}

// RIIC0 RXI0
// 受信データフル　割り込み
// ICDRRレジスタに受信データあり
#pragma interrupt (Excep_RIIC0_RXI0(vect=247))
void Excep_RIIC0_RXI0(void){
	
	uint8_t dummy;
	
	if ( dummy_read_fg == 1 ) {		// スレーブアドレス(読み出し用)送信後のダミーリード
	
		dummy = RIIC0.ICDRR;		// ダミーリード　(SCLクロックを出力して、受信動作開始)
		dummy_read_fg = 0;
	}
	else { 
		
		iic_rcv_data[iic_rcv_pt] = RIIC0.ICDRR;    // 受信データ読み出し
		
		iic_rcv_pt++;

		if( iic_rcv_pt == 1 ) {			// 最初のデータ受信
		     RIIC0.ICMR3.BIT.ACKBT = 0;		// ACK 送信
		}
		
		else if ( iic_rcv_pt == 2 ) {		// 最終データ受信
		     RIIC0.ICMR3.BIT.ACKBT = 1;		// NACK 送信	
		      
		     RIIC0.ICCR2.BIT.SP = 1;		// ストップコンディションの発行要求の設定
		}
	}
	
}

// RIIC0 TXI0
// 送信データエンプティ	割り込み
// ICDRTレジスタに送信データなしの時に、発生
//
// iic_sd_pt:  送信内容
//     0    : スレーブアドレス(Write)
//     1    : 読み出しアドレス (b15-b8)
//     2    : 読み出しアドレス (b7-b0)
//    

#pragma interrupt (Excep_RIIC0_TXI0(vect=248))
void Excep_RIIC0_TXI0(void){
	
	RIIC0.ICDRT = iic_sd_data[iic_sd_pt];  // 送信
		
	if ( iic_sd_pt == 2 ) {      // 読み出しアドレス(b7-b0) 送信開始後
		
		RIIC0.ICIER.BIT.TIE = 0;	// 送信データエンプティ割り込み(TXI)要求の禁止
		
		while(RIIC0.ICIER.BIT.TIE) {	// クリアの確認
		}
	}
	
	iic_sd_pt++;		// 送信位置の更新
	
}

// RIIC0 TEI0
// 送信終了割り込み
//  ICSR2.BIT.TEND = 1で発生 ( ICSR2.BIT.TDRE = 1 の状態で、SCL クロックの9 クロック目の立ち上がりで発生)
#pragma interrupt (Excep_RIIC0_TEI0(vect=249))
void Excep_RIIC0_TEI0(void){
	
	 RIIC0.ICSR2.BIT.TEND = 0;		//  送信完了フラグのクリア
	 
	 RIIC0.ICCR2.BIT.RS = 1;		// リスタートコンディションの発行 
	
}



//  RIIC レジスタ設定 ( I2C マスタとし使用 )
//      PCLKB = 24[MHz]
//

void RIIC0_Init(void)
{
	RIIC0.ICCR1.BIT.ICE = 0;    // RIICは機能停止(SCL,SDA端子非駆動状態)
	RIIC0.ICCR1.BIT.IICRST = 1; // RIICリセット、
	RIIC0.ICCR1.BIT.ICE = 1;    // 内部リセット状態 、SCL0、SDA0端子駆動状態
	
				    //  通信速度 = 400 kbps (実測 401 kbps  )プルアップ抵抗 1[KΩ]
	RIIC0.ICMR1.BIT.CKS = 1;    // RIICの内部基準クロック = 24/4 = 6 MHz　
	RIIC0.ICBRH.BIT.BRH = 0xE7; 
	RIIC0.ICBRL.BIT.BRL = 0xF1;

	
	RIIC0.ICMR3.BIT.ACKWP = 1;	// ACKBTビットへの書き込み許可		
					
	RIIC0.ICMR3.BIT.RDRFS = 1;	// RDRFフラグ(受信データフル)セットタイミング
					// 1：RDRF フラグは8 クロック目の立ち上がりで“1” にし、8 クロック目の立ち下がりでSCL0 ラインをLow にホールドします。
					// このSCL0 ラインのLow ホールドはACKBT ビットへの書き込みにより解除されます。
					//この設定のとき、データ受信後アクノリッジビット送出前にSCL0 ラインを自動的にLow にホールドするため、
					// 受信データの内容に応じてACK (ACKBT ビットが“0”) またはNACK (ACKBT ビットが“1”) を送出する処理が可能です。
			
					
	RIIC0.ICMR3.BIT.WAIT = 0;	// WAITなし (9クロック目と1クロック目の間をLowにホールドしない)	
					// RDRFS ビットとWAIT ビットがともに“0” のとき、ダブルバッファによる連続受信動作が可能です。

	RIIC0.ICMR3.BIT.SMBS = 0;       // I2Cバス選択 
	
	RIIC0.ICCR1.BIT.IICRST = 0;	 // RIICリセット解除
	
}

//
//  RIICインターフェイス用のポートを設定
//  RIIC用: 
//   	P16 = SCL0   
//      P17 = SDA0  
//

void RIIC0_Port_Set(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;      // マルチファンクションピンコントローラ　プロテクト解除
    	MPC.PWPR.BIT.PFSWE = 1;     // PmnPFS ライトプロテクト解除
    
    	MPC.P16PFS.BYTE = 0x0f;     // PORT16 = SCL0
    	MPC.P17PFS.BYTE = 0x0f;     // PORT17 = SDA0
          
    	MPC.PWPR.BYTE = 0x80;      //  PmnPFS ライトプロテクト 設定
  
    	PORT1.PMR.BIT.B6 = 1;     // PORT16:周辺モジュールとして使用
    	PORT1.PMR.BIT.B7 = 1;     // PORT17:      :
	
}


// RIIC の割り込み用、割り込みコントローラ、RIICモジュール側の設定
// 以下を、割り込み処理で行う
//   EEI: 通信エラー/通信イベント (NACK 検出、スタートコンディション検出、ストップコンディション検出)
//　 RXI:　受信データフル
//   TXI:  送信データエンプティ
//   TEI:  送信終了

void RIIC0_Init_interrupt(void)
{
					// 割り込みコントローラ側（受け入れ側)
					// 通信エラー/通信イベント 割り込み
	IPR(RIIC0,EEI0) = 0x04;		// 割り込みレベル = 4　　（15が最高レベル)
	IR(RIIC0,EEI0) = 0;		// 割り込み要求のクリア
	IEN(RIIC0,EEI0) = 1;		// 割り込み許可	
	
					// 受信データフル
	IPR(RIIC0,RXI0) = 0x04;		// 割り込みレベル = 4　　（15が最高レベル)
	IR(RIIC0,RXI0) = 0;		// 割り込み要求のクリア
	IEN(RIIC0,RXI0) = 1;		// 割り込み許可	
	
					// 送信データエンプティ
	IPR(RIIC0,TXI0) = 0x04;		// 割り込みレベル = 4　　（15が最高レベル)
	IR(RIIC0,TXI0) = 0;		// 割り込み要求のクリア
	IEN(RIIC0,TXI0) = 1;		// 割り込み許可	
	
					// 送信終了
	IPR(RIIC0,TEI0) = 0x04;		// 割り込みレベル = 4　　（15が最高レベル)
	IR(RIIC0,TEI0) = 0;		// 割り込み要求のクリア
	IEN(RIIC0,TEI0) = 1;		// 割り込み許可	
	
	
					// RIICモジュール側の設定 (要求元)
	RIIC0.ICIER.BIT.TMOIE = 0;	// タイムアウト割り込み(TMOI)要求の禁止
	RIIC0.ICIER.BIT.ALIE  = 0;   	// アービトレーションロスト割り込み(ALI)要求の禁止
	
	RIIC0.ICIER.BIT.STIE  = 1;	// スタートコンディション検出割り込み(STI)要求の許可
	RIIC0.ICIER.BIT.SPIE  = 1;      // ストップコンディション検出割り込み(SPI)要求の許可
	RIIC0.ICIER.BIT.NAKIE  = 1;	// NACK受信割り込み(NAKI)要求の許可

	RIIC0.ICIER.BIT.RIE = 1;	// 受信データフル割り込み(RXI)要求の許可
	RIIC0.ICIER.BIT.TIE = 0;	// 送信データエンプティ割り込み(TXI)要求の禁止
	RIIC0.ICIER.BIT.TEIE = 1;	// 送信終了割り込み(TEI)要求の許可
	
}

