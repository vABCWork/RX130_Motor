
//  送信時のLED用　出力ポート P27　(パソコンとの通信用)
//#define LED_TX_PDR      (PORT2.PDR.BIT.B7)   // 1: 出力ポートに指定
//#define LED_TX_PODR     (PORT2.PODR.BIT.B7)  //   出力データ


////  受信時のLED用　出力ポート P31 　(パソコンとの通信用)
//#define LED_RX_PDR      (PORT3.PDR.BIT.B1)   // 1: 出力ポートに指定
//#define LED_RX_PODR     (PORT3.PODR.BIT.B1)  //   出力データ


extern volatile uint8_t  rcv_data[16];
extern  volatile uint8_t  rcv_cnt;
extern	volatile uint8_t rcv_over;


extern volatile uint8_t sd_data[64];
extern volatile uint8_t  send_cnt;


void control_para_write(void);
void control_resp_make(void);

void  ctsu_resp_make(void);


uint16_t cal_crc_sd_data( uint16_t num );


void SCI_1_Reg_Set(void);
void SCI1_Init_interrupt(void);
void SCI_1_Port_Set(void);



