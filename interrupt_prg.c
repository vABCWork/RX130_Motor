#include <machine.h>

//
//    　割り込み処理プログラム
//   (割り込みベクタテーブルに登録されているアドレスの分岐先のプログラム本体)
//
// ・　resrvedとして定義されていない箇所は、リンクオプションの「可変ベクタの空き領域のアドレス」に _Except_BRKとすれば、
//    ベクタ番号 0 と同じアドレスが登録される。
//
// ・割り込みベクタテーブルの先頭アドレスは、レジスタINTBで示される。  
// ・スタートアップ時 (restprg.c)で、INTB に C$VECT の値を格納している。
//
//      brk() : 無条件トラップ (割り込みベクタテーブルのベクタ番号0のアドレスに分岐) 
// 　　wait() : プログラムの実行停止  (ノンマスカブル割り込み、割り込み、またはリセットが発生するとプログラムの実行を開始)
//

#pragma section IntPRG

#pragma interrupt (Excep_BRK(vect=0))  
void Excep_BRK(void){ 
	wait(); 
}

// vector  1 reserved
// vector  2 reserved
// vector  3 reserved
// vector  4 reserved
// vector  5 reserved
// vector  6 reserved
// vector  7 reserved
// vector  8 reserved
// vector  9 reserved
// vector 10 reserved
// vector 11 reserved
// vector 12 reserved
// vector 13 reserved
// vector 14 reserved
// vector 15 reserved

// BSC BUSERR
#pragma interrupt (Excep_BSC_BUSERR(vect=16))

void Excep_BSC_BUSERR(void){ 
	brk();
}

// vector 17 reserved
// vector 18 reserved
// vector 19 reserved
// vector 20 reserved
// vector 21 reserved
// vector 22 reserved

// FCU FRDYI
#pragma interrupt (Excep_FCU_FRDYI(vect=23))
void Excep_FCU_FRDYI(void){
	brk();
}

// vector 24 reserved
// vector 25 reserved
// vector 26 reserved

// ICU SWINT
#pragma interrupt (Excep_ICU_SWINT(vect=27))
void Excep_ICU_SWINT(void){
	brk();
}

// cmt.cで定義
// CMT0 CMI0
//#pragma interrupt (Excep_CMT0_CMI0(vect=28)) 
//void Excep_CMT0_CMI0(void){
//	brk();
//}

// cmt.cで定義
// // CMT1 CMI1
//#pragma interrupt (Excep_CMT1_CMI1(vect=29))
//void Excep_CMT1_CMI1(void){
//	brk();
//}


// CAC FERRF
#pragma interrupt (Excep_CAC_FERRF(vect=32))
void Excep_CAC_FERRF(void){
	brk();
}

// CAC MENDF
#pragma interrupt (Excep_CAC_MENDF(vect=33))
void Excep_CAC_MENDF(void){
	brk();
}

// CAC OVFF
#pragma interrupt (Excep_CAC_OVFF(vect=34))
void Excep_CAC_OVFF(void){
	brk();
}

// vector 35 reserved
// vector 36 reserved
// vector 37 reserved
// vector 38 reserved
// vector 39 reserved
// vector 40 reserved
// vector 41 reserved
// vector 42 reserved
// vector 43 reserved

// RSPI0 SPEI0
#pragma interrupt (Excep_RSPI0_SPEI0(vect=44))
void Excep_RSPI0_SPEI0(void){
	brk();
}

// RSPI0 SPRI0
#pragma interrupt (Excep_RSPI0_SPRI0(vect=45))
void Excep_RSPI0_SPRI0(void){
	brk();
}

// RSPI0 SPTI0
#pragma interrupt (Excep_RSPI0_SPTI0(vect=46))
void Excep_RSPI0_SPTI0(void){
	brk();
}


// RSPI0 SPII0
#pragma interrupt (Excep_RSPI0_SPII0(vect=47))
void Excep_RSPI0_SPII0(void){
	brk();
}

// vector 48 reserved
// vector 49 reserved
// vector 50 reserved
// vector 51 reserved

// RSCAN COMFRXINT
#pragma interrupt (Excep_RSCAN_COMFRXINT(vect=52))
void Excep_RSCAN_COMFRXINT(void){
	brk();
}

// RSCAN RXFINT
#pragma interrupt (Excep_RSCAN_RXFINT(vect=53))
void Excep_RSCAN_RXFINT(void){
	brk();
}

// RSCAN TXINT
#pragma interrupt (Excep_RSCAN_TXINT(vect=54))
void Excep_RSCAN_TXINT(void){
	brk();
}

// RSCAN CHERRINT
#pragma interrupt (Excep_RSCAN_CHERRINT(vect=55))
void Excep_RSCAN_CHERRINT(void){
	brk();
}

// RSCAN GLERRINT
#pragma interrupt (Excep_RSCAN_GLERRINT(vect=56))
void Excep_RSCAN_GLERRINT(void){
	brk();
}

// DOC DOPCF
#pragma interrupt (Excep_DOC_DOPCF(vect=57))
void Excep_DOC_DOPCF(void){
	brk();
}


// CMPB CMPB0

#pragma interrupt (Excep_CMPB_CMPB0(vect=58))
void Excep_CMPB_CMPB0(void){
	brk();
}

// CMPB CMPB1
#pragma interrupt (Excep_CMPB_CMPB1(vect=59))
void Excep_CMPB_CMPB1(void){
	brk();
}

// CTSU CTSUWR
// ctsu.cで定義
//#pragma interrupt (Excep_CTSU_CTSUWR(vect=60))
//void Excep_CTSU_CTSUWR(void){
//	brk();
//}

// CTSU CTSURD
// ctsu.cで定義
//#pragma interrupt (Excep_CTSU_CTSURD(vect=61))
//void Excep_CTSU_CTSURD(void){
//	brk();
//}

// CTSU CTSUFN
//// ctsu.cで定義
//#pragma interrupt (Excep_CTSU_CTSUFN(vect=62))
//void Excep_CTSU_CTSUFN(void){
//	brk();
//}

// RTC CUP
#pragma interrupt (Excep_RTC_CUP(vect=63))
void Excep_RTC_CUP(void){
	brk();
}

// ICU IRQ0
#pragma interrupt (Excep_ICU_IRQ0(vect=64))
void Excep_ICU_IRQ0(void){
	brk();
}

// ICU IRQ1
#pragma interrupt (Excep_ICU_IRQ1(vect=65))
void Excep_ICU_IRQ1(void){
	brk();
}

// ICU IRQ2
#pragma interrupt (Excep_ICU_IRQ2(vect=66))
void Excep_ICU_IRQ2(void){
	brk();
}

// ICU IRQ3
#pragma interrupt (Excep_ICU_IRQ3(vect=67))
void Excep_ICU_IRQ3(void){
	brk();
}

// ICU IRQ4
#pragma interrupt (Excep_ICU_IRQ4(vect=68))
void Excep_ICU_IRQ4(void){
	brk();
}

// ICU IRQ5
#pragma interrupt (Excep_ICU_IRQ5(vect=69))
void Excep_ICU_IRQ5(void){
	brk();
}

// ICU IRQ6
#pragma interrupt (Excep_ICU_IRQ6(vect=70))
void Excep_ICU_IRQ6(void){
	brk();
}

// ICU IRQ7
#pragma interrupt (Excep_ICU_IRQ7(vect=71))
void Excep_ICU_IRQ7(void){
	brk();
}

// vector 72 reserved
// vector 73 reserved
// vector 74 reserved
// vector 75 reserved
// vector 76 reserved
// vector 77 reserved
// vector 78 reserved
// vector 79 reserved

// ELC ELSR8I
#pragma interrupt (Excep_ELC_ELSR8I(vect=80))
void Excep_ELC_ELSR8I(void){
	brk();
}


// SYSTEM SNZI
#pragma interrupt (Excep_SYSTEM_SNZI(vect=81))
void Excep_SYSTEM_SNZI(void){
	brk();
}

// vector 82 reserved
// vector 83 reserved
// vector 84 reserved
// vector 85 reserved
// vector 86 reserved
// vector 87 reserved

// LVD LVD1
#pragma interrupt (Excep_LVD_LVD1(vect=88))
void Excep_LVD_LVD1(void){
	brk();
}

// LVD LVD2
#pragma interrupt (Excep_LVD_LVD2(vect=89))
void Excep_LVD_LVD2(void){
	brk();
}

// vector 90 reserved
// vector 91 reserved

// RTC ALM
#pragma interrupt (Excep_RTC_ALM(vect=92))
void Excep_RTC_ALM(void){
	brk();
}

// RTC PRD
#pragma interrupt (Excep_RTC_PRD(vect=93))
void Excep_RTC_PRD(void){
	brk();
}

// vector 94 reserved
// vector 95 reserved
// vector 96 reserved
// vector 97 reserved
// vector 98 reserved
// vector 99 reserved
// vector 100 reserved
// vector 101 reserved

// ad.c で定義
// S12AD S12ADI0
//#pragma interrupt (Excep_S12AD_S12ADI0(vect=102))
//void Excep_S12AD_S12ADI0(void){
//	brk();
//}

// S12AD GBADI
#pragma interrupt (Excep_S12AD_GBADI(vect=103))
void Excep_S12AD_GBADI(void){
	brk();
}

// vector 104 reserved
// vector 105 reserved

// ELC ELSR18I
#pragma interrupt (Excep_ELC_ELSR18I(vect=106))
void Excep_ELC_ELSR18I(void){
	brk();
}


// vector 107 reserved
// vector 108 reserved
// vector 109 reserved
// vector 110 reserved


// AES AESWRI
#pragma interrupt (Excep_AES_AESWRI(vect=111))
void Excep_AES_AESWRI(void){
	brk();
}

// AES AESRDI
#pragma interrupt (Excep_AES_AESRDI(vect=112))
void Excep_AES_AESRDI(void){
	brk();
}

// TRNG TRNGRDI
#pragma interrupt (Excep_TRNG_TRNGRDI(vect=113))
void Excep_TRNG_TRNGRDI(void){
	brk();
}


// MTU0 TGIA0
#pragma interrupt (Excep_MTU0_TGIA0(vect=114))
void Excep_MTU0_TGIA0(void){
	brk();
}

// mtu.cで定義
// MTU0 TGIB0
//#pragma interrupt (Excep_MTU0_TGIB0(vect=115))
//void Excep_MTU0_TGIB0(void){
//	brk();
//}

// MTU0 TGIC0
#pragma interrupt (Excep_MTU0_TGIC0(vect=116))
void Excep_MTU0_TGIC0(void){
	brk();
}

// MTU0 TGID0
#pragma interrupt (Excep_MTU0_TGID0(vect=117))
void Excep_MTU0_TGID0(void){
	brk();
}

// MTU0 TCIV0
#pragma interrupt (Excep_MTU0_TCIV0(vect=118))
void Excep_MTU0_TCIV0(void){
	brk();
}

// MTU0 TGIE0
#pragma interrupt (Excep_MTU0_TGIE0(vect=119))
void Excep_MTU0_TGIE0(void){
	brk();
}

// MTU0 TGIF0
#pragma interrupt (Excep_MTU0_TGIF0(vect=120))
void Excep_MTU0_TGIF0(void){
	brk();
}

//  MTU1 TGIA1
#pragma interrupt (Excep_MTU1_TGIA1(vect=121))
void Excep_MTU1_TGIA1(void){
	brk();
}

// MTU1 TGIB1
#pragma interrupt (Excep_MTU1_TGIB1(vect=122))
void Excep_MTU1_TGIB1(void){
	brk();
}

// MTU1 TCIV1
#pragma interrupt (Excep_MTU1_TCIV1(vect=123))
void Excep_MTU1_TCIV1(void){
	brk();	
}

// MTU1 TCIU1
#pragma interrupt (Excep_MTU1_TCIU1(vect=124))
void Excep_MTU1_TCIU1(void){
	brk();
}

// mtu.cで定義
//// MTU2 TGIA2
//#pragma interrupt (Excep_MTU2_TGIA2(vect=125))
//void Excep_MTU2_TGIA2(void){
//	brk();
//}

// MTU2 TGIB2
#pragma interrupt (Excep_MTU2_TGIB2(vect=126))
void Excep_MTU2_TGIB2(void){
	brk();
}

// MTU2 TCIV2
#pragma interrupt (Excep_MTU2_TCIV2(vect=127))
void Excep_MTU2_TCIV2(void){
	brk();
}

// MTU2 TCIU2
#pragma interrupt (Excep_MTU2_TCIU2(vect=128))
void Excep_MTU2_TCIU2(void){
	brk();
}

// MTU3 TGIA3
#pragma interrupt (Excep_MTU3_TGIA3(vect=129))
void Excep_MTU3_TGIA3(void){
	brk();
}

// MTU3 TGIB3
#pragma interrupt (Excep_MTU3_TGIB3(vect=130))
void Excep_MTU3_TGIB3(void){
	brk();
}

// MTU3 TGIC3
#pragma interrupt (Excep_MTU3_TGIC3(vect=131))
void Excep_MTU3_TGIC3(void){
	brk();
}

// MTU3 TGID3
#pragma interrupt (Excep_MTU3_TGID3(vect=132))
void Excep_MTU3_TGID3(void){
	brk();
}

// MTU3 TCIV3
#pragma interrupt (Excep_MTU3_TCIV3(vect=133))
void Excep_MTU3_TCIV3(void){
	brk();
}

// MTU4 TGIA4
#pragma interrupt (Excep_MTU4_TGIA4(vect=134))
void Excep_MTU4_TGIA4(void){
	brk();
}

// MTU4 TGIB4
#pragma interrupt (Excep_MTU4_TGIB4(vect=135))
void Excep_MTU4_TGIB4(void){
	brk();
}

// MTU4 TGIC4
#pragma interrupt (Excep_MTU4_TGIC4(vect=136))
void Excep_MTU4_TGIC4(void){
	brk();
}

// MTU4 TGID4
#pragma interrupt (Excep_MTU4_TGID4(vect=137))
void Excep_MTU4_TGID4(void){
	brk();
}

// MTU4 TCIV4
#pragma interrupt (Excep_MTU4_TCIV4(vect=138))
void Excep_MTU4_TCIV4(void){
	brk();
}

// MTU5 TGIU5
#pragma interrupt (Excep_MTU5_TGIU5(vect=139))
void Excep_MTU5_TGIU5(void){
	brk();
}

// MTU5 TGIV5
#pragma interrupt (Excep_MTU5_TGIV5(vect=140))
void Excep_MTU5_TGIV5(void){
	brk();
}

// MTU5 TGIW5
#pragma interrupt (Excep_MTU5_TGIW5(vect=141))
void Excep_MTU5_TGIW5(void){
	brk();
}

// vector 142 reserved
// vector 143 reserved
// vector 144 reserved
// vector 145 reserved
// vector 146 reserved
// vector 147 reserved
// vector 148 reserved
// vector 149 reserved
// vector 150 reserved
// vector 151 reserved
// vector 152 reserved
// vector 153 reserved
// vector 154 reserved
// vector 155 reserved
// vector 156 reserved
// vector 157 reserved
// vector 158 reserved
// vector 159 reserved
// vector 160 reserved
// vector 161 reserved
// vector 162 reserved
// vector 163 reserved
// vector 164 reserved
// vector 165 reserved
// vector 166 reserved
// vector 167 reserved
// vector 168 reserved
// vector 169 reserved

// POE OEI1
#pragma interrupt (Excep_POE_OEI1(vect=170))
void Excep_POE_OEI1(void){
	brk();
}

// POE OEI2
#pragma interrupt (Excep_POE_OEI2(vect=171))
void Excep_POE_OEI2(void){
	brk();
}

// vector 172 reserved
// vector 173 reserved

// TMR0 CMIA0
#pragma interrupt (Excep_TMR0_CMIA0(vect=174))
void Excep_TMR0_CMIA0(void){
	brk();
}

// TMR0 CMIB0
#pragma interrupt (Excep_TMR0_CMIB0(vect=175))
void Excep_TMR0_CMIB0(void){
	brk();
}

// TMR0 OVI0
#pragma interrupt (Excep_TMR0_OVI0(vect=176))
void Excep_TMR0_OVI0(void){
	brk();
}

// TMR1 CMIA1
#pragma interrupt (Excep_TMR1_CMIA1(vect=177))
void Excep_TMR1_CMIA1(void){
	brk();
}

// TMR1 CMIB1
#pragma interrupt (Excep_TMR1_CMIB1(vect=178))
void Excep_TMR1_CMIB1(void){
	brk();
}

// TMR1 OVI1
#pragma interrupt (Excep_TMR1_OVI1(vect=179))
void Excep_TMR1_OVI1(void){
	brk();
}

// TMR2 CMIA2
#pragma interrupt (Excep_TMR2_CMIA2(vect=180))
void Excep_TMR2_CMIA2(void){
	brk();
}

// TMR2 CMIB2
#pragma interrupt (Excep_TMR2_CMIB2(vect=181))
void Excep_TMR2_CMIB2(void){
	brk();
}

// TMR2 OVI2
#pragma interrupt (Excep_TMR2_OVI2(vect=182))
void Excep_TMR2_OVI2(void){
	brk();
}

// TMR3 CMIA3
#pragma interrupt (Excep_TMR3_CMIA3(vect=183))
void Excep_TMR3_CMIA3(void){
	brk();
}

// TMR3 CMIB3
#pragma interrupt (Excep_TMR3_CMIB3(vect=184))
void Excep_TMR3_CMIB3(void){
	brk();
}

// TMR3 OVI3
#pragma interrupt (Excep_TMR3_OVI3(vect=185))
void Excep_TMR3_OVI3(void){
	brk();
}


// vector 186 reserved
// vector 187 reserved
// vector 188 reserved
// vector 189 reserved
// vector 190 reserved
// vector 191 reserved
// vector 192 reserved
// vector 193 reserved
// vector 194 reserved
// vector 195 reserved
// vector 196 reserved
// vector 197 reserved
// vector 198 reserved
// vector 199 reserved
// vector 200 reserved
// vector 201 reserved
// vector 202 reserved
// vector 203 reserved
// vector 204 reserved
// vector 205 reserved
// vector 206 reserved
// vector 207 reserved
// vector 208 reserved
// vector 209 reserved
// vector 210 reserved
// vector 211 reserved
// vector 212 reserved
// vector 213 reserved
// vector 214 reserved
// vector 215 reserved
// vector 216 reserved
// vector 217 reserved



// SCI1 ERI1
#pragma interrupt (Excep_SCI1_ERI1(vect=218))
void Excep_SCI1_ERI1(void){
	brk();
}

// SCI1 RXI1
// sci1.c で定義
//#pragma interrupt (Excep_SCI1_RXI1(vect=219))
//void Excep_SCI1_RXI1(void){
//	brk();
//}

// SCI1 TXI1
// sci1.c で定義
//#pragma interrupt (Excep_SCI1_TXI1(vect=220))
//void Excep_SCI1_TXI1(void){
//	brk();
//}

// SCI1 TEI1
// sci1.c で定義
//#pragma interrupt (Excep_SCI1_TEI1(vect=221))
//void Excep_SCI1_TEI1(void){
//	brk();
//}

// SCI5 ERI5
#pragma interrupt (Excep_SCI5_ERI5(vect=222))
void Excep_SCI5_ERI5(void){
	brk();
}


// SCI5 RXI5
// sci5.c で定義
//#pragma interrupt (Excep_SCI5_RXI5(vect=223))
//void Excep_SCI5_RXI5(void){
//	brk();
//}

// SCI5 TXI5
// sci5.c で定義
//#pragma interrupt (Excep_SCI5_TXI5(vect=224))
//void Excep_SCI5_TXI5(void){
//	brk();
//}

// SCI5 TEI5
//// sci5.c で定義
//#pragma interrupt (Excep_SCI5_TEI5(vect=225))
//void Excep_SCI5_TEI5(void){
//	brk();
//}

// SCI6 ERI6
#pragma interrupt (Excep_SCI6_ERI6(vect=226))
void Excep_SCI6_ERI6(void){
	brk();
}

// SCI6 RXI6
#pragma interrupt (Excep_SCI6_RXI6(vect=227))
void Excep_SCI6_RXI6(void){
	brk();
}

// SCI6 TXI6
#pragma interrupt (Excep_SCI6_TXI6(vect=228))
void Excep_SCI6_TXI6(void){
	brk();
}

// SCI6 TEI6
#pragma interrupt (Excep_SCI6_TEI6(vect=229))
void Excep_SCI6_TEI6(void){
	brk();
}


// SCI8 ERI8
#pragma interrupt (Excep_SCI8_ERI8(vect=230))
void Excep_SCI8_ERI8(void){
	brk();
}

// SCI8 RXI8
#pragma interrupt (Excep_SCI8_RXI8(vect=231))
void Excep_SCI8_RXI8(void){
	brk();
}

// SCI8 TXI8
#pragma interrupt (Excep_SCI8_TXI8(vect=232))
void Excep_SCI8_TXI8(void){
	brk();
}

// SCI8 TEI8
#pragma interrupt (Excep_SCI8_TEI8(vect=233))
void Excep_SCI8_TEI8(void){
	brk();
}

// SCI9 ERI9
#pragma interrupt (Excep_SCI9_ERI9(vect=234))
void Excep_SCI9_ERI9(void){
	brk();
}

// SCI9 RXI9
#pragma interrupt (Excep_SCI9_RXI9(vect=235))
void Excep_SCI9_RXI9(void){
	brk();
}

// SCI9 TXI9
#pragma interrupt (Excep_SCI9_TXI9(vect=236))
void Excep_SCI9_TXI9(void){
	brk();
}

// SCI9 TEI9
#pragma interrupt (Excep_SCI9_TEI9(vect=237))
void Excep_SCI9_TEI9(void){
	brk();
}


// SCI12 ERI12
#pragma interrupt (Excep_SCI12_ERI12(vect=238))
void Excep_SCI12_ERI12(void){
	brk();
}

// SCI12 RXI12
#pragma interrupt (Excep_SCI12_RXI12(vect=239))
void Excep_SCI12_RXI12(void){
	brk();
}

// SCI12 TXI12
#pragma interrupt (Excep_SCI12_TXI12(vect=240))
void Excep_SCI12_TXI12(void){
	brk();
}

// SCI12 TEI12
#pragma interrupt (Excep_SCI12_TEI12(vect=241))
void Excep_SCI12_TEI12(void){
	brk();
}

// SCI12 SCIX0
#pragma interrupt (Excep_SCI12_SCIX0(vect=242))
void Excep_SCI12_SCIX0(void){
	brk();
}

// SCI12 SCIX1
#pragma interrupt (Excep_SCI12_SCIX1(vect=243))
void Excep_SCI12_SCIX1(void){
	brk();
}

// SCI12 SCIX2
#pragma interrupt (Excep_SCI12_SCIX2(vect=244))
void Excep_SCI12_SCIX2(void){
	brk();
}

// SCI12 SCIX3
#pragma interrupt (Excep_SCI12_SCIX3(vect=245))
void Excep_SCI12_SCIX3(void){
	brk();
}

// RIIC0 EEI0
// riic.cで定義
//#pragma interrupt (Excep_RIIC0_EEI0(vect=246))
//void Excep_RIIC0_EEI0(void){
//	brk();
//}

// RIIC0 RXI0
// riic.cで定義
//#pragma interrupt (Excep_RIIC0_RXI0(vect=247))
//void Excep_RIIC0_RXI0(void){
//	brk();
//}

// RIIC0 TXI0
//// riic.cで定義
//#pragma interrupt (Excep_RIIC0_TXI0(vect=248))
//void Excep_RIIC0_TXI0(void){
//	brk();
//}

// RIIC0 TEI0
//// riic.cで定義
//#pragma interrupt (Excep_RIIC0_TEI0(vect=249))
//void Excep_RIIC0_TEI0(void){
//	brk();
//}

// vector 250 reserved
// vector 251 reserved
// vector 252 reserved
// vector 253 reserved
// vector 254 reserved

// LPT LPTCMI1
#pragma interrupt (Excep_LPT_LPTCMI1(vect=255))
void Excep_LPT_LPTCMI1(void){
	brk();
}
