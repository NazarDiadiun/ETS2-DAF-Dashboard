#define lo8(x) ((int)(x)&0xff)
#define hi8(x) ((int)(x)>>8)

void Dashboard_Main_Callback();
void Right_Display_Callback();
void Icons_Callback();
void Speedometer_Callback();
void RPM_Callback();
void Distances_Callback();
void Arrows_Callback();
void Time_Callback();

void Mileage_Read_Callback();
