
extern  uint8_t  ch0_open_err;
extern  uint8_t  ch1_open_err;
extern	float32_t slg_ch0_temp;
extern	float32_t slg_ch1_temp;
extern  float32_t slg_ch3_temp;

void slg_temp_cal(void);

void thermistor_temp_cal(void);

void thermistor_thermo_volt(void);

void thermo_couple_volt(void);

		      
float32_t  thermo_couple_temp( float32_t e_mv); 