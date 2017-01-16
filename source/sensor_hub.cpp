#include "sensor_hub.h" 

/* Instantiate the expansion board */
static X_NUCLEO_IKS01A1 *mems_expansion_board = X_NUCLEO_IKS01A1::Instance(PB_9, PB_8);

/* Retrieve the composing elements of the expansion board */
static GyroSensor	*gyroscope	    = mems_expansion_board->GetGyroscope();
static MotionSensor	*accelerometer	    = mems_expansion_board->GetAccelerometer();
static MagneticSensor	*magnetometer	    = mems_expansion_board->magnetometer;
static HumiditySensor	*humidity_sensor    = mems_expansion_board->ht_sensor;
static TempSensor	*temp_sensor	    = mems_expansion_board->ht_sensor;
static PressureSensor	*pressure_sensor    = mems_expansion_board->pt_sensor;

/* Helper function for printing floats & doubles */
char *printDouble(char* str, double v, int decimalDigits)
{
    int i = 1;
    int intPart, fractPart;
    int len;
    char *ptr;

    /* prepare decimal digits multiplicator */
    for (;decimalDigits!=0; i*=10, decimalDigits--);

    /* calculate integer & fractinal parts */
    intPart = (int)v;
    fractPart = (int)((v-(double)(int)v)*i);

    /* fill in integer part */
    sprintf(str, "%i.", intPart);

    /* prepare fill in of fractional part */
    len = strlen(str);
    ptr = &str[len];

    /* fill in leading fractional zeros */
    for (i/=10;i>1; i/=10, ptr++) {
	if(fractPart >= i) break;
	*ptr = '0';
    }

    /* fill in (rest of) fractional part */
    sprintf(ptr, "%i", fractPart);

    return str;
}

uint8_t get_SensorID(Senosor_Type type)
{
    uint8_t id;

    switch(type)
    {
	case HUMIDITY:
	    humidity_sensor->ReadID(&id);
	    break;
	case PRESSURE:
	    pressure_sensor->ReadID(&id);
	    break;
	case MAGNETOMETER:
	    magnetometer->ReadID(&id);
	    break;
	case GYROSCOPE:
	    gyroscope->ReadID(&id);
	    break;
	default:
	    id = 0xFF;
	    break;
    }

    return id;
}

float get_Temperature()
{
    float value;

    temp_sensor->GetTemperature(&value);
    return value;
}

float get_Humidity()
{
    float value;

    humidity_sensor->GetHumidity(&value);
    return value;
}

float get_Pressure()
{
    float value;

    pressure_sensor->GetPressure(&value);
    return value;
}

void get_Magnet(AxesValues* axes)
{
    int32_t values[3];

    magnetometer->Get_M_Axes(values);
    axes->x = values[0]; axes->y = values[1]; axes->z = values[2];
    
    return;
}

void get_Accel(AxesValues* axes)
{
    int32_t values[3];

    accelerometer->Get_X_Axes(values);
    axes->x = values[0]; axes->y = values[1]; axes->z = values[2];

    return;
}

void get_Gyro(AxesValues* axes)
{
    int32_t values[3];

    gyroscope->Get_G_Axes(values);
    axes->x = values[0]; axes->y = values[1]; axes->z = values[2];

    return;
}

