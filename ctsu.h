
#define CTSU_CH_NUM   2     // タッチセンサの個数 (TS14,TS15)

extern volatile uint32_t ctsu_sr[CTSU_CH_NUM];
extern volatile uint32_t ctsu_so[CTSU_CH_NUM];
extern volatile uint32_t ctsu_scnt[CTSU_CH_NUM];

extern volatile uint8_t  ctsu_reading_fg;

void ctsu_self_set(void);

void set_ctsuwr_data(void);

void set_ctsu_so(void);
void ctsu_ini(void);

void ctsu_cra_set(void);
void ctsu_crb_set(void);

void ctsu_ch_enable(void);
void tscap_clear(void);
void touch_port_set(void);



