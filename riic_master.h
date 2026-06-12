
extern	uint8_t iic_slave_adrs; 
extern volatile uint8_t iic_rcv_data[16];
extern volatile uint8_t iic_sd_data[32];

extern volatile uint8_t iic_sd_pt;
extern volatile uint8_t iic_rcv_pt;

extern volatile uint8_t  iic_com_over_fg;

void RIIC0_Init(void);
void RIIC0_Port_Set(void);

void RIIC0_Init_interrupt(void);