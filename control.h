
#define RPM_RANGE 4000.0

extern  uint8_t  mode_run_stop;
extern	uint8_t  mode_auto_manual;
extern  uint8_t   pre_mode_run_stop;	
extern  uint8_t   pre_mode_auto_manual;

extern float32_t  pid_pv_rpm;
extern float32_t  pid_sv_rpm;

extern float32_t  pid_kp;
extern float32_t  pid_i;

extern	float32_t   pid_mv;


void pid_para_ini(void);
void cal_control(void);
void cal_rpm(void);