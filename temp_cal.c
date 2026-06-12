
#include <math.h>
#include "iodefine.h"
#include "misratypes.h"

#include "riic_slg47011.h"
#include "temp_cal.h"

#include "debug.h"

 
 uint8_t  ch0_open_err;		// ch0 熱電対 断線時 = 1
 uint8_t  ch1_open_err;		// ch1 熱電対 断線時 = 1

 float32_t slg_ch0_temp;	// SLG ch0 温度 [℃]
 float32_t slg_ch1_temp;	// SLG ch1 温度 [℃]
 float32_t slg_ch3_temp;	//　サーミスタによる測定温度
 
 float32_t slg_ch0_thermo_volt;   // SLG ch0 熱起電力 [mV]　
 float32_t slg_ch1_thermo_volt;	  // SLG ch1 熱起電力 [mV] 
 float32_t slg_ch3_thermo_volt;   // サーミスタ測定温度からの熱起電力 K-type [mV]
 
 
 //  SLG データによる温度計算  
 //  計算時間 = 80[usec]
 //
 void slg_temp_cal(void)
 {
	//PC3_PODR = 1;	// debug PC3 = 1
	
 	thermistor_temp_cal();	// サーミスタの温度計算
	
 	thermistor_thermo_volt();	// サーミスタ測定温度の熱起電力
	
	thermo_couple_volt();		// 熱電対の熱起電力を得る  
	
	if ( slg_buffer_0 < 4000 ) {
	    slg_ch0_temp = thermo_couple_temp( slg_ch0_thermo_volt + slg_ch3_thermo_volt); //  SLG ch0 温度 [℃]
	    ch0_open_err = 0;
	}
	else{
		slg_ch0_temp = 500.0;
		ch0_open_err = 1;	// ch0 熱電対の断線発生
	}
	if ( slg_buffer_1 < 4000 ) {	
	    slg_ch1_temp = thermo_couple_temp( slg_ch1_thermo_volt + slg_ch3_thermo_volt); //  SLG ch1 温度 [℃]
	    ch1_open_err = 0;
	}
	else{
		slg_ch1_temp = 500.0;
		ch1_open_err = 1;	// ch1 熱電対の断線発生
	}
	
	
	//PC3_PODR = 0;	// debug PC3 = 0
 
 }

//
// サーミスタの温度計算
//  使用サーミスタ:  NCP15XH103F03RC (村田製作所)
//                   10K[Ω](at 25℃), B = 3380
// SimSurfing NTサーミスタ動作シュミレーターで求めた、近似式(3次)を使用.
//
//   T = 101.61739* x^3 - 217.83684* x^2 + 206.25610* x - 54.752738
//  
//   T:温度[℃], x:測定電圧[V}
//
void thermistor_temp_cal(void){

    float32_t v_out;
 
   
    v_out = ( slg_buffer_3 / 4095.0 ) * 1.62;       //  測定電圧 [V]
    

    slg_ch3_temp = 101.61739 * powf(v_out, 3) - 217.83684 * powf(v_out, 2) + 206.25610 * v_out - 54.752738; // サーミスタによる測定温度


}


//
// サーミスタ測定温度から、熱起電力を得る
//
//  熱起電力は、熱電対 Kタイプ ( 0～1372[℃])用
//
//　多項式: JIS C1602 ( kikakurui.com/c1/C1602-2015-01.html )
//
//   E = b0 + b1*t^1 + b2*t^2 + b3*t^3 + b4*t^4 + b5*t^5 + b6*t^6 + b7*t^7 + b8*t^8 + b9*t^9 + c0*Exp(c1* (t - 126.9686)^2) 
//   t: 温度 [℃]  
//   E: 熱起電力 [uV]
//
//   b0 = -1.76004137E+01
//   b1 = 3.89212050E+01
//   b2 = 1.85587700E-02
//   b3 = -9.94575929E-05
//   b4 = 3.18409457E-07
//   b5 = -5.60728449E-10
//   b6 = 5.60750591E-13
//   b7 = -3.20207200E-16
//   b8 = 9.71511472E-20
//   b9 = -1.21047213E-23
//
//   c0 = 1.185976E+02
//   c1 = -1.183432E-04
//

const float32_t b_const[10] = { -1.76004137E+01, 3.89212050E+01, 1.85587700E-02, -9.94575929E-05, 3.18409457E-07,
                             -5.60728449E-10, 5.60750591E-13, -3.20207200E-16, 9.71511472E-20,-1.21047213E-23};

const float32_t c0 = 1.185976E+02;
const float32_t c1 = -1.183432E-04;


void thermistor_thermo_volt(void)
{
    uint32_t i;
    
    float32_t a;
    float32_t t;
    float32_t E;

    t = slg_ch3_temp;           // サーミスタによる測定温度

    E = 0;

    for (i = 0; i < 10; i++)
    {
        E = E + b_const[i] * powf( t , i);
    }

    a = c1 * powf( t - 126.9686, 2 );

    E = E + c0 * expf(a);

    slg_ch3_thermo_volt = E * 0.001;    // サーミスタ測定温度からの熱起電力 K-type [mV]

}

//
// 熱電対の熱起電力を得る
// SLG47011の A/Dコンバータ 12bit 設定
// Gain = 32
// Vref = 1.62[V], Vref/2 = 0.81[V]
// 0.81[V]/32 = 0.0253125 [V]
//
// A/Dコンバータ 12bitの場合:
// AD値             測定電圧       温度
//    0              -25.3 [mV]
//  2047 (0x7ff)        0 [mV]     0[℃]
//  4095 (0xfff)     25.3 [mV]　 610[℃]　
//
//
// 
void thermo_couple_volt(void)
{
    slg_ch0_thermo_volt = ( (slg_buffer_0 - 2047) / 2047.0 ) * 25.3125;   // [mV]

    slg_ch1_thermo_volt = ( (slg_buffer_1 - 2047) / 2047.0) * 25.3125;

}

//  熱起電力から温度を得る
//   入力: 熱起電力[mV] 
//   出力: 温度[℃]    
// 多項式:     
//   T = c0 + c1*E^1 + c2*E^2 + c3*E^3 + c4*E^4 + c5*E^5 + c6*E^6 + c7*E^7 + c8*E^8 + c9*E^9 
//   T: 温度 [℃]  
//   E: 熱起電力 [uV]
//
//  定数は0から500℃用 
//   c0 = 0.0 
//   c1 = 2.5083550E-02
//   c2 = 7.8601060E-08
//   c3 = -2.5031310E-10
//   c4 = 8.3152700E-14
//   c5 = -1.2280340E-17
//   c6 = 9.8040360E-22
//   c7 =-4.4130300E-26
//   c8 =  1.0577340E-30
//   c9 = -1.0527550E-35
//


const float32_t c_const[10] =  {   0.0, 2.508355E-02, 7.860106E-08, -2.503131E-10, 8.315270E-14,
                              -1.228034E-17, 9.804036E-22,-4.413030E-26, 1.057734E-30, -1.052755E-35};
			      
float32_t  thermo_couple_temp( float32_t e_mv) 
{
    uint32_t i;
    
    float32_t t;
    float32_t e;

  
    t = 0;
    e = e_mv  * 1000;    // 熱起電力[uV]

    for ( i = 0; i < 10; i++)
    {
        t = t + c_const[i] * powf(e, i);
    }

    return t;
}
