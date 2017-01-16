#include "main.h"

#define TRACE_GROUP "AT_BOARD_MAIN"

typedef struct __GPS_Data{
    double latitude;
    double longitude;
    double altitude;
    double speed;
} GPS_DATA;

Serial          pc(PC_6, PC_7);

// Create the GPS object
GPSI2C gps;
// Create the modem object
MDMSerial mdm; 

static void trace_printer(const char *str)
{
    pc.printf("%s\r\n", str);
}

int dprint(void* param, char const * format, ...)
{
    pc.printf(format);
}

void modem_TCPTest()
{
    int ret;
    char buf[512] = "";
    int socket; 

    wait(1);
    // join the internet connection 
    MDMParser::IP ip = mdm.join(APN,USERNAME,PASSWORD);

    if (ip == NOIP)
    {
	pc.printf("Not able to join network\r\n");
    }
    else
    {
	int port = 7;
	const char* host = "echo.u-blox.com";
	MDMParser::IP ip = mdm.gethostbyname(host);
	char data[] =	"\r\nxxx Socket Hello World\r\n"
			"End\r\n";
	mdm.dumpIp(ip);

	pc.printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
	pc.printf("Testing TCP sockets with ECHO server\r\n");
	socket = mdm.socketSocket(MDMParser::IPPROTO_TCP);
	if (socket >= 0)
	{
	    mdm.socketSetBlocking(socket, 10000);
	    if (mdm.socketConnect(socket, host, port)) 
	    {
		memcpy(data, "\r\nTCP", 5); 
		ret = mdm.socketSend(socket, data, sizeof(data)-1);
		if (ret == sizeof(data)-1) 
		{
		    pc.printf("Socket Send %d \"%s\"\r\n", ret, data);
		}
		ret = mdm.socketRecv(socket, buf, sizeof(buf)-1);
		if (ret >= 0) 
		{
		    pc.printf("Socket Recv %d \"%.*s\"\r\n", ret, ret, buf);
		}
		mdm.socketClose(socket);
	    }
	    mdm.socketFree(socket);
	}
	pc.printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    }
}

bool modem_intt()
{
    bool mdmOk;

    // initialize the modem 
    MDMParser::DevStatus devStatus = {};
    MDMParser::NetStatus netStatus = {};
    mdmOk = mdm.init(SIMPIN, &devStatus);
    mdm.dumpDevStatus(&devStatus, dprint);
    if (mdmOk) 
    {
	// wait until we are connected
	mdmOk = mdm.registerNet(&netStatus);
	mdm.dumpNetStatus(&netStatus, dprint);
    }

    return mdmOk;
}

void get_GPSData(GPS_DATA* data)
{
#define _CHECK_TALKER(s) ((buf[3] == s[0]) && (buf[4] == s[1]) && (buf[5] == s[2]))

    double la = 0, lo = 0, a = 0, s = 0; 
    int ret;
    char buf[512] = "";

    if((ret = gps.getMessage(buf, sizeof(buf))) > 0)	// 적당한 NMEA값을 가져오기 위한 바복필요
    {
	int len = LENGTH(ret);
	//pc.printf("NMEA: %.*s\r\n", len-2, msg); 
	if ((PROTOCOL(ret) == GPSParser::NMEA) && (len > 6))
	{
	    // talker is $GA=Galileo $GB=Beidou $GL=Glonass $GN=Combined $GP=GPS
	    if ((buf[0] == '$') || buf[1] == 'G') 
	    {
		if (_CHECK_TALKER("GLL")) 
		{
		    char ch;
		    if ( gps.getNmeaAngle(1,buf,len,la) && 
			    gps.getNmeaAngle(3,buf,len,lo) && 
			    gps.getNmeaItem(6,buf,len,ch) && ch == 'A')
		    {
			pc.printf("GPS Location: %.5f %.5f\r\n", la, lo); 

			data->latitude = la;
			data->longitude = lo;
		    }
		} 
		else if (_CHECK_TALKER("GGA") || _CHECK_TALKER("GNS") ) 
		{
		    if (gps.getNmeaItem(9, buf, len, a)) // altitude msl [m]
		    {
			pc.printf("GPS Altitude: %.1f\r\n", a); 
			data->altitude = a;
		    }
		} 
		else if (_CHECK_TALKER("VTG")) 
		{
		    if (gps.getNmeaItem(7,buf,len,s)) // speed [km/h]
		    {
			pc.printf("GPS Speed: %.1f\r\n", s); 
			data->speed = s;
		    }
		}
	    }
	}

    }
    else
    {
	pc.printf("Not get the PGS data.\r\n");
    }
}

bool system_init()
{
    bool ret=true;
    pc.baud(115200);

    // set up the tracing library
    mbed_trace_init(); 
    mbed_trace_print_function_set(trace_printer);
    mbed_trace_config_set( TRACE_ACTIVE_LEVEL_ALL|TRACE_MODE_COLOR );
    
    pc.printf("system init\r\n");

    if( !(ret = modem_intt()) )
    {
	pc.printf("Fail modem init\r\n");
    }

    return ret;
}

int main()
{
    char buffer[32];
    float value;

    AxesValues	axes;
    GPS_DATA	gpsdata;

    if(!system_init())
    {
	pc.printf("Fail system initalization\r\n");
	return 0;
    }

    pc.printf("START~~~!!\r\n");

    modem_TCPTest();

    pc.printf("Humidity id is 0x%X\r\n", get_SensorID(HUMIDITY) );
    pc.printf("Pressure id is 0x%X\r\n", get_SensorID(PRESSURE) );
    pc.printf("Magnetometer id is 0x%X\r\n", get_SensorID(MAGNETOMETER) );
    pc.printf("Gyroscope id is 0x%X\r\n", get_SensorID(GYROSCOPE) );

    while(1)
    {
	wait(1);

	value = get_Temperature();
	printDouble(buffer, value);
	pc.printf("[temp] %7s°C\r\n", buffer);
	value = get_Humidity();
	printDouble(buffer, value);
	pc.printf("[hum] %s%%\r\n", buffer);
	value = get_Pressure();
	printDouble(buffer, value);
	pc.printf("[press] %smbar\r\n", buffer);

	get_Magnet(&axes);
	pc.printf("[mag/mgauss]:  %6d, %6d, %6d\r\n", axes.x, axes.y, axes.z);
	get_Accel(&axes);
	pc.printf("[acc/mg]:      %6d, %6d, %6d\r\n", axes.x, axes.y, axes.z);
	get_Gyro(&axes);
	pc.printf("[gyro/mdps]:   %6d, %6d, %6d\r\n", axes.x, axes.y, axes.z);

	get_GPSData(&gpsdata);
    }
}
