#include "iodefine.h"
#include  "misratypes.h"
#include  "mtu.h"
#include  "ad.h"
#include "control.h"

uint8_t   mode_run_stop;	// 1: Run 出力有り, 0: Stop(出力 = 0)
uint8_t   mode_auto_manual;	// 1: Auto(PID演算の出力),  0:Manual(ボリュームの出力)

uint8_t   pre_mode_run_stop;	// 1つ前のモード
uint8_t   pre_mode_auto_manual; //


float32_t  f_cnt_avg;		 // カウント平均値
float32_t  msec_per_rotation;    // 1回転に要する時間[msec] 


float32_t  pid_pv_rpm;     	// 現在の回転数 (1分間での回転数 [rpm])
float32_t  pid_sv_rpm;		// 目標 回転数　0～4000 [rpm] 

float32_t  pid_pv;		// 現在の回転数 (0～1) pid_pv_rpm/4000
float32_t  pid_sv;		// 目標 回転数 (0～1)  pid_sv_rpm/4000


float32_t  pid_kp;		// 比例ゲイン Kp

float32_t  pid_i;		// 積分時間 

float32_t  pid_ts;		// サンプルタイム(Ts)= 演算周期

float32_t   pid_en;		// 偏差 pv-sp (今回)　
float32_t   pid_en1;		// 偏差 (前回)(1制御周期前)( 1[msec[前)

float32_t   pid_mv;		// PID演算後の出力(今回) (0～1.0)
float32_t   pid_mv1;		// PID演算後の出力(前回)
float32_t   pid_delta_mv;	// 出力の変化分


//  PIDパラメータ初期設定
//
void pid_para_ini(void)
{
	pid_sv_rpm = 2000.0;	// 目標 回転数[rpm]
	
	pid_kp = 0.5;		// 比例ゲイン
	
        //pid_i =  0.01;		// 積分時間 0.01[sec]  制御できない。
	pid_i = 0.1;		//  積分時間 0.1[sec]

	pid_ts = 0.001;		// 1[msec] = 0.001[sec]
		
	pid_en1 = 0.0;
	pid_mv1 = 0.0;
	
	capt_ovrflow_flag = 1;   //  キャプチャ未発生フラグ = 1
}



//
//    制御演算　 ( 演算周期 1[msec] )
//    速度型 PID (PI制御）
//
//    MVn = ΔMVn + MVn-1
//　今回の出力　= 出力変化分 + 1演算周期前の出力 + 
//
//  ΔMVn = Kp*( En - En1 ) + Kp * ( Ts/Ti ) * En
// 出力変化分 = P動作出力変化分 +　I動作出力変化分 
//
//    Kp:比例ゲイン,  Ti:積分時間,  Ts:演算周期
//
void cal_control(void)
{
		
	pid_sv =  pid_sv_rpm / RPM_RANGE;    // 0.0～1.0の範囲(0～100%)にスケーリング
	pid_pv =  pid_pv_rpm / RPM_RANGE;
	
	pid_en = pid_sv - pid_pv;  	// 偏差 En = SV - PV
	
	if ( mode_run_stop == 1 ) {  // Runモードの場合
	  if ( mode_auto_manual == 1 ) {  // Autoモードの場合
	      pid_delta_mv = pid_kp * ( pid_en - pid_en1 ) +  pid_kp * (  pid_ts / pid_i ) * pid_en;
	      pid_mv = pid_mv1 + pid_delta_mv;
	      
	      if ( pid_mv > 1.0 ) {	// 1.0(=100[%])を超える場合
	      	pid_mv = 1.0;		// 100[%]
	      }
	   }
	  else {	// Manualモードの場合
	       pid_mv = ad_cnt[2] / 4095.0;  //  VR1の値 
	  }
	}
	else {		// Stopモードの場合
	  pid_mv = 0;
	}
	 
	
	pid_en1 = pid_en;	// 次回に、現在の値を1つ前の値として使用
	pid_mv1 = pid_mv;
}



//  1回転のカウント値から、回転数[rpm]を計算
//
// RX140　インプットキャプチャー使用
//   PCLKB = 24 MHz
//   24/64 = 0.375[MHz] でカウント。　1カウントの時間は、64/24= 2.67[usec]
//   1回転に要する時間[usec] = カウント値 * 2.67[usec]
//
//    オーバフロー時間: 175[msec]  ( 2.67 x 65535[usec] = 174760[usec] )
//

void cal_rpm(void)
{
	uint32_t  i;
	uint32_t  cnt_sum;
	
	if (( capt_data[0] == 0)|| ( capt_ovrflow_flag == 1 ) ) {
		pid_pv_rpm = 0;
	}
	else{			// キャプチャ有り、キャプチャオーバフローしていない。
	
	  cnt_sum = 0;
	
	  for ( i = 0 ; i < 10 ; i++ ) {
	     cnt_sum = cnt_sum + capt_data[i];
	  }
	
	  f_cnt_avg = cnt_sum / 10.0;	// カウント平均値
	
	  msec_per_rotation = (f_cnt_avg * (64.0/24.0))/ 1000.0;  //  1回転に要する時間[msec] 
	
	  pid_pv_rpm = 60000.0 / msec_per_rotation;   // 回転数 [rpm] [1/min]
	}
}