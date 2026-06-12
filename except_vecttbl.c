//
//    例外ベクタテーブル、リセットベクタ、 オプション設定メモリ(OFSM)
//   
//  1) 例外ベクタテーブルで、未定義命令例外、特権命令例外、アクセス例外、浮動小数点例外、ノンマスカブル割り込み処理の開始アドレスを設定。
//  ・例外ベクタテーブルの先頭アドレスは、レジスタ EXTBで示される。  
//　・スタートアップ時 (restprg.c)で、EXTB に section EXCEPTVECTの値を格納している。
//  ・EXCEPTVECTの配置アドレスは、リンクオプションのセクション開始アドレスで指定。（0xffff ff80)
//
// 2) リセットベクタは、0xffff fffc  
//　・ RESETVECTの配置アドレスは、リンクオプションのセクション開始アドレスで指定。（0xffff fffc)
//  
// 3) オプション設定メモリ(OFSM)
//    エンディアン選択レジスタ(MDE),オプション機能選択レジスタ1 (OFS1), (OSF2)を設定。
//
// 4) フラッシュメモリプロテクト機能用のID領域
//
// 参考:
// 「RX140グループ ユーザーズマニュアル　ハードウェア編」( R01UH0905JJ0120 Rev.1.20 2024.11.22 )
//    2.6.1 例外ベクタテーブル
//    13.1 例外事象
//    2.2.2.9 浮動小数点ステータスワード(FPSW)
//    7. オプション設定メモリ(OFSM)
//    41.9 フラッシュメモリプロテクト機能
//

extern void Dummy(void);
extern void Excep_SuperVisorInst(void);
extern void Excep_AccessInst(void);
extern void Excep_UndefinedInst(void);
extern void Excep_FloatingPoint(void);
extern void NonMaskableInterrupt(void);

extern void PowerON_Reset_PC(void);


//
// オプション機能選択レジスタと例外ベクタテーブル (0xffff ff80～ 0xffff fff8)
//   ( reservedには、Dummy()関数の先頭アドレスが入る) 
//
//  アドレス   : 内容
// 0xffff ff80 : エンディアン選択レジスタ(MDE) (リトルエンディアン用)
// 0xffff ff84 : reserved  
// 0xffff ff88 : オプション機能選択レジスタ1 (OFS1)
// 0xffff ff8c : オプション機能選択レジスタ0 (OFS0)
// 0xffff ff90 : reserved  
// 0xffff ff94 : reserved
// 0xffff ff98 : reserved
// 0xffff ff9c : reserved
// 0xffff ffa0 : フラッシュメモリプロテクト用　ID
// 0xffff ffa4 : フラッシュメモリプロテクト用　ID
// 0xffff ffa8 : フラッシュメモリプロテクト用　ID
// 0xffff ffac : フラッシュメモリプロテクト用　ID
// 0xffff ffb0 : reserved  
// 0xffff ffb4 : reserved
// 0xffff ffb8 : reserved
// 0xffff ffbc : reserved
// 0xffff ffc0 : reserved  
// 0xffff ffc4 : reserved
// 0xffff ffc8 : reserved
// 0xffff ffcc : reserved
// 0xffff ffd0 : 特権命令例外
// 0xffff ffd4 : アクセス例外
// 0xffff ffd8 : reserved
// 0xffff ffdc : 未定義命令例外
// 0xffff ffe0 : reserved
// 0xffff ffe4 : 浮動小数点例外
// 0xffff ffe8 : reserved
// 0xffff ffec : reserved
// 0xffff fff0 : reserved
// 0xffff fff4 : reserved
// 0xffff fff8 : ノンマスカブル割り込み
//
// 0xffff fffc : リセットベクタ


#pragma section C EXCEPTVECT

void (*const Except_Vectors[])(void) = {

// 0xffffff80  MDE register
    (void (*)(void))0xffffffff,	 // リトルエンディアン

// 0xffffff84  Reserved
    Dummy,
    
// 0xffffff88  OFS1 register
// 
// 注:「リンケージエディタとCRC演算器のCRC-CCITT結果の違い」 (https://ja-support.renesas.com/knowledgeBase/17796715)
//  FINE接続でマイコンと接続している場合(JTAG接続の場合は該当しません)、E1またはE20エミュレータ起動時に
//  エミュレータ制御用のコードとしてオプション機能選択レジスタ1(OFS1：アドレスFFFFFF88h-FFFFFF8Bh)のbit24に0が書き込まれます。
// 
   
//
// オプション機能選択レジスタ0(OFS1)
// b1-b0: 電圧検出0レベル選択ビット
//        	0 0：3.85 Vを選択
//        	0 1：2.85 Vを選択
//		1 0：2.53 Vを選択
//		1 1：1.90 Vを選択
//  b2: 電圧検出0回路起動ビット 
//                0：リセット後、電圧監視0リセット有効
//                1：リセット後、電圧監視0リセット無効
//  b3:電源立ち上げ時起動時間短縮ビット
//		  0：電源立ち上げ時起動時間短縮
//		  1：通常起動
// b7-b4:  1111
// b8: HOCO発振有効ビット
//		0：リセット後、HOCO発振が有効
//		1：リセット後、HOCO発振が無効  
// b11-b9: 111
// b13-b12:HOCO周波数選択ビット
//  		0 0：48 MHzを選択
//		0 1：48 MHzを選択
//		1 0：24 MHzを選択
//		1 1：32 MHzを選択
// b31-b14: all 1
//


    (void (*)(void))0xffffcfff, // OFS1 HOCO = 48[MHz]

   
//
// オプション機能選択レジスタ0(OFS0)
//  b0: 予約ビット b0=1
//  b1: IWDTスタートモード選択ビット b1 
//				     0:オートスタートモード IWDT自動起動,
//				     1:リセット後、IWDTは停止状態)
//  b3-b2: IWDTタイムアウト期間選択ビット b3 b2
//                                        0  0: 128サイクル(007Fh)
//                                        0  1: 512サイクル(01FFh)
//                                        1  0:1024サイクル(03FFh)
//                                        1  1:2048サイクル(07FFh)
//  b7-b4: IWDTクロック分周比選択ビット  b7 b6 b5 b4 
//                                       0  0  0  0  : 分周なし
//                                       0  0  1  0  : 16分周
//                                       0  0  1  1  : 32分周
//                                       0  1  0  0  : 64分周
//                                       1  1  1  1  :128分周
//                                       0  1  0  1  :256分周
//  b9-b8:  IWDTウィンドウ終了位置選択  b9 b8
//                                      0  0 : 75%
//					0  1 : 50%
//					1  0 : 25%
//					1  1 :  0%
// b11-b10: IWDTウィンドウ開始位置選択  b11 b10
//                                      0  0 : 25%
//					0  1 : 50%
//					1  0 : 75%
//					1  1 :  0%
// b12: IWDTリセット割り込み要求選択ビット b12
//					    0: ノンマスカブル割り込み要求を許可
//					    1: リセットを許可
// b13: 予約ビット b13=1
// b14: IWDTスリープモードカウント停止制御ビット b14
//						  0: カウント停止無効
//						  1: スリープモード、ソフトウェアスタンバイモード、
// 	             	                             ディープスリープモード移行時のカウント停止有効
//b31-b15: 予約ビット b31-b15 = 1
//
//
// 設定-1:
//　オートスタートモード
//  タイムアウト期間 : : 2048サイクル(07ffh)
//  クロック分周比 : 1 (分周なし) 
//  ウインドウ終了位置: 0% 
//  ウインドウ開始位置:50%
//
//  リセット出力の許可
//
// タイムアウトする時間:
//  独立ウオッチドックタイマ専用クロック(15[KHz])の1分周、2048サイクル 
//  1/15[msec]x2048 = 136.5[msec] でタイムアウト
//                      
// 1111 1111 1111 1111  1111 0111 0000 1101  
//    f    f    f    f   f    7     0    d
//
// (void (*)(void))0xfffff70d,    // OFS0　
//
// 設定-2:
//　オートスタートモード
//  タイムアウト期間 : : 2048サイクル(07ffh)
//  クロック分周比 : 1 (分周なし) 
//  ウインドウ終了位置: 0% (終了位置なし）
//  ウインドウ開始位置:100%(開始位置なし）
//
//  ノンマスカラブル割り込み(NMI)の許可
//
// タイムアウトする時間:
//  独立ウオッチドックタイマ専用クロック(15[KHz])の1分周、2048サイクル 
//  1/15[msec]x2048 = 136.5[msec] でタイムアウト
//                      
// 1111 1111 1111 1111  1110 1111 0000 1101  
//    f    f    f    f   e    f     0    d
//
// (void (*)(void))0xffffef0d,    // OFS0　

//   (void (*)(void))0xffffff0d,    // OFS0　136.5[msec]でタイムアウトで、リセット

  (void (*)(void))0xffffef0d,    // OFS0　136.5[msec]でタイムアウトでNMI発生

 
//    (void (*)(void))0xffffffff,    // OFS0  IWDT停止　
    
// 0xffffff90  Reserved
    Dummy,
    
// 0xffffff94  Reserved
    Dummy,
    
// 0xffffff98  Reserved
    Dummy,
    
// 0xffffff9c  Reserved
    Dummy,
    
// 0xffffffa0  ID
    (void (*)(void))0xffffffff,

// 0xffffffa4  ID
    (void (*)(void))0xffffffff,
    
// 0xffffffa8  ID
    (void (*)(void))0xffffffff,
    
// 0xffffffac  ID
    (void (*)(void))0xffffffff,
    
// 0xffffffb0  Reserved
    Dummy,
    
// 0xffffffb4  Reserved
    Dummy,
    
// 0xffffffb8  Reserved
    Dummy,
    
// 0xffffffbc  Reserved
    Dummy,
    
// 0xffffffc0  Reserved
    Dummy,

// 0xffffffc4  Reserved
    Dummy,
    
// 0xffffffc8  Reserved
    Dummy,
    
// 0xffffffcc  Reserved
    Dummy,
    
// 0xffffffd0  特権命令例外
    Excep_SuperVisorInst,
    
// 0xffffffd4  アクセス例外
    Excep_AccessInst,
    
// 0xffffffd8  Reserved
    Dummy,
    
// 0xffffffdc   未定義命令例外
    Excep_UndefinedInst,
    
// 0xffffffe0  Reserved
    Dummy,
    
// 0xffffffe4  浮動小数点例外
    Excep_FloatingPoint,
    
// 0xffffffe8  Reserved
    Dummy,
    
// 0xffffffec  Reserved
    Dummy,
    
//v0xfffffff0  Reserved
    Dummy,
    
//v0xfffffff4  Reserved
    Dummy,
    
//v0xfffffff8  ノンマスカブル割り込み　 NMI
    NonMaskableInterrupt,

};


#pragma section C RESETVECT
void (*const Reset_Vectors[])(void) = {
	PowerON_Reset_PC 
};
