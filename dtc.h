
// 転送情報用　構造体 (フルアドレスモード)
struct dtcreg_full {
	uint8_t   wk;
	uint8_t   MRC;
	uint8_t   MRB;
	uint8_t   MRA;
	void 	*SAR;
	void 	*DAR;
	uint16_t  CRB;
	uint16_t  CRA;
};

extern struct dtcreg_full SCI1_RCV_DTC; 
extern struct dtcreg_full SCI5_RCV_DTC;

extern struct dtcreg_full AD_CNT_DTC;	

void  initDTC(void);


void  set_dtc_sci1_rcv(void);
void  set_dtc_sci1_snd(void);

void  set_dtc_sci5_rcv(void);
void  set_dtc_sci5_snd(void);

void  set_dtc_ad(void);
void  set_dtc_ctsu(void);

