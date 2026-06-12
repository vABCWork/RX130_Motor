

//   N [msec] 待つ関数 (ICLK = 48MHz)
//   命令語は、 RX V2 アーキテクチャ
//    コンパイル時　最適化レベル = 2   (-optimize = 2 )
//   n_msec=  1:  1msec
//      
//  注: プログラム領域 (セクション P)のアライメント数は、1byte
//	instalign4 でアライメント数を 4 としている。
//

#pragma instalign4 delay_msec

void  delay_msec( unsigned long  n_msec )
{
	unsigned long delay_cnt;

	while( n_msec-- ) {

	   delay_cnt = 16026;
		
 	   while(delay_cnt --) 
	   { 			 
	   }

	}
}

