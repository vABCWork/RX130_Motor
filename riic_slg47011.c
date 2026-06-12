#include "iodefine.h"
#include "misratypes.h"
#include "riic_slg47011.h"
#include "riic_master.h"



uint16_t slg_buffer_0;      // SLG47011 Buffer0 resultの値
uint16_t slg_buffer_1;      // SLG47011 Buffer1 resultの値

uint16_t slg_buffer_3;      // SLG47011 Buffer3 resultの値





//  
//    SLG4701の Buffer0 result, Buffer1 result, Buffer3 result からのデータ読み出し
//
void slg_47011_buffer_rd( void )
{
	
	riic_rd_slg_47011( 0x2212 );  // Buffer0 result読み出し
	
	while( iic_com_over_fg != 1 ) {    // 送受信完了待ち
	}
	
	slg_buffer_0 = iic_rcv_data[0];	// 受信データをバッファへ格納
	slg_buffer_0 = ( slg_buffer_0 << 8 ) | iic_rcv_data[1];

	
	riic_rd_slg_47011( 0x2224 );  // Buffer1 result読み出し
	while( iic_com_over_fg != 1 ) {    // 送受信完了待ち
	}
	
	slg_buffer_1 = iic_rcv_data[0];	// 受信データをバッファへ格納
	slg_buffer_1 = ( slg_buffer_1 << 8 ) | iic_rcv_data[1];
	
	
	riic_rd_slg_47011( 0x2248 );  // Buffer3 result読み出し
	while( iic_com_over_fg != 1 ) {    // 送受信完了待ち
	}
	
	slg_buffer_3 = iic_rcv_data[0];	// 受信データをバッファへ格納
	slg_buffer_3 = ( slg_buffer_3 << 8 ) | iic_rcv_data[1];
	
	
}




//
//   SLG47011からのデータ読み出し ((割り込み使用)
//
// 入力: rd_adrs 読み出しアドレス　
//               Buffer0 result = 0x2212
//               Buffer1 result = 0x2224
//               Buffer2 result = 0x2236
//               Buffer3 result = 0x2248
//
//   IIC 送信バッファ
//   　iic_sd_data[0] : スレーブアドレス(7bit) + Wrビット(0)
//     iic_sd_data[1] : 読み出しアドレス(b15-b8)
//     iic_sd_data[2] : 読み出しアドレス(b7-b0)
//     iic_sd_data[3] : スレーブアドレス(7bit) + Rdビット(1)
//
void   riic_rd_slg_47011( uint16_t rd_adrs )
{
	iic_sd_data[0] = ( iic_slave_adrs << 1 );    // 書き込み用 スレーブアドレス

	iic_sd_data[1] = ( rd_adrs >> 8 );
	
	iic_sd_data[2] =  rd_adrs;
	
	iic_sd_data[3] = ( iic_sd_data[0] | 0x01);   // 読み出し用　スレーブアドレス 
	
	riic_sd_start();		//  IIC 送信開始
	
}



//
//  RIIC 送信開始
// 異常時、無限ループに入るので、ウオッチドックタイマが必要。
// I2Cバスビジー状態の場合、無限ループ
//
void riic_sd_start(void)
{
	iic_sd_pt = 0;	       	   // 送信データ位置　クリア
	iic_rcv_pt = 0;            // 受信データ位置
	
	iic_com_over_fg = 0;       // 通信完了フラグのクリア
	
	
	
	RIIC0.ICIER.BIT.TIE = 1;		// 送信データエンプティ割り込み(TXI)要求の許可
	
	while(RIIC0.ICCR2.BIT.BBSY == 1){ 	// I2Cバスビジー状態の場合、ループ （Watch dogタイマが必要）
	}
	
	
	RIIC0.ICCR2.BIT.ST = 1;		// スタートコンディションの発行  (マスタ送信の開始)
					// スタートコンディション発行後、ICSR2.TDRE(送信データエンプティフラグ)=1となり、
					//  TXI(送信データエンプティ)割り込み、発生

}