//------------------------------------------------------------------------------------------------
//
// ODROID 16x2 LCD Sheild Printing time Application.
//
// Defined port number is wiringPi port number.
//
// Compile : gcc -o <create excute file name> <source file name> -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt -lgpiod
// Run : sudo ./<created excute file name>
//
//-------------------------------------------------------------------------------------------------
 #include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
 
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <lcd.h>

//------------------------------------------------------------------------------------------------------------
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/sockios.h>

//------------------------------------------------------------------------------------------------------------
//
// struct for ethtool driver
//
//------------------------------------------------------------------------------------------------------------
#define DUPLEX_HALF 0x00
#define DUPLEX_FULL 0x01

#define ETHTOOL_GSET 0x00000001 /* Get settings command for ethtool */

struct ethtool_cmd {
	unsigned int cmd;
	unsigned int supported; /* Features this interface supports */
	unsigned int advertising; /* Features this interface advertises */
	unsigned short speed; /* The forced speed, 10Mb, 100Mb, gigabit */
	unsigned char duplex; /* Duplex, half or full */
	unsigned char port; /* Which connector port */
	unsigned char phy_address;
	unsigned char transceiver; /* Which tranceiver to use */
	unsigned char autoneg; /* Enable or disable autonegotiation */
	unsigned int maxtxpkt; /* Tx pkts before generating tx int */
	unsigned int maxrxpkt; /* Rx pkts before generating rx int */
	unsigned int reserved[4];
};

//------------------------------------------------------------------------------------------------------------
//
// for lcd interface data
//
//------------------------------------------------------------------------------------------------------------
#define LCD_ROW             2   // 16 Char
#define LCD_COL             16  // 2 Line
#define LCD_BUS             4   // Interface 4 Bit mode
#define LCD_UPDATE_PERIOD   1000 // 300ms
 
static unsigned char lcdFb[LCD_ROW][LCD_COL] = {0, };
 
static int lcdHandle  = 0;
 
#define PORT_LCD_RS     7   // GPIOY.BIT3(#83)
#define PORT_LCD_E      0   // GPIOY.BIT8(#88)
#define PORT_LCD_D4     2   // GPIOX.BIT19(#116)
#define PORT_LCD_D5     3   // GPIOX.BIT18(#115)
#define PORT_LCD_D6     1   // GPIOY.BIT7(#87)
#define PORT_LCD_D7     4   // GPIOX.BIT7(#104)
 
//------------------------------------------------------------------------------------------------------------
//
// Button:
//
//------------------------------------------------------------------------------------------------------------
#define PORT_BUTTON1	5	// GPIOX.BIT5(#102)
#define PORT_BUTTON2	6	// GPIOX.BIT6(#103)
 
static int ledPos = 0;

const int ledPorts[] = {
	21,	// GPIOX.BIT4(#101)
	22,	// GPIOX.BIT3(#100)
	23,	// GPIOX.BIT11(#108):PWM_B
	24,	// GPIOX.BIT0(#97)
	11,	// GPIOX.BIT21(#118)
	26,	// GPIOX.BIT2(#99)
	27,	// GPIOX.BIT1(#98)
	13,	// GPIOX.BIT9(#106):MISO
	12,	// GPIOX.BIT10(#107):MOSI/PWM_E
	14,	// GPIOX.BIT8(#105):SCLK
	10,	// GPIOX/BIT20(#117):CE0
};

#define MAX_LED_CNT sizeof(ledPorts) / sizeof(ledPorts[0])


static int dispMode = 1;
//------------------------------------------------------------------------------------------------------------
int getEthInfo(const unsigned char *eth_name)
{
	int fd;
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	unsigned char ipstr[16];

	/* this entire function is almost copied from ethtool source code */
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		fprintf(stdout,"Cannot get control socket");
		return -1;
	}
	strncpy(ifr.ifr_name, eth_name, IFNAMSIZ); 
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) { 
		fprintf(stdout, "SIOCGIFADDR ioctl Error!!\n"); 
		goto out;
	} else { 
		memset(ipstr, 0x00, sizeof(ipstr));
		inet_ntop(AF_INET, ifr.ifr_addr.sa_data+2, ipstr, sizeof(struct sockaddr)); 
		sprintf(&lcdFb[0][0], "IP %s", ipstr);
		fprintf(stdout, "myOwn IP Address is %s\n", ipstr); 
	}

	/* Pass the "get info" command to eth tool driver */
	ecmd.cmd = ETHTOOL_GSET;
	ifr.ifr_data = (caddr_t)&ecmd;

	/* ioctl failed: */
	if (ioctl(fd, SIOCETHTOOL, &ifr))
	{
		fprintf(stdout,"Cannot get device settings");
		goto out;
	}
	close(fd);

	fprintf(stdout, "LinkSpeed = %d MB/s", ecmd.speed);
	switch (ecmd.duplex)
	{
		case DUPLEX_FULL:
			sprintf(&lcdFb[1][0], "Speed=%d, %s", ecmd.speed, "FULL");
			fprintf(stdout," Full Duplex\n");
		break;
		case DUPLEX_HALF:
			sprintf(&lcdFb[1][0], "Speed=%d, %s", ecmd.speed, "HALF");
			fprintf(stdout," Half Duplex\n");
		break;
		default:
			sprintf(&lcdFb[1][0], "Speed=%d, %s", ecmd.speed, "????");
			fprintf(stdout," Duplex reading faulty\n");
		break;
	}
	return 0;
out:
	close(fd);
	return -1;
}

//------------------------------------------------------------------------------------------------------------
static void lcd_update (void)
{
	int i, j;

	memset((void *)&lcdFb, ' ', sizeof(lcdFb));

	if (dispMode) {
		if (getEthInfo("eth0")) {
			lcdFb[0][sprintf(&lcdFb[0][0], "%s", "Network Error!")] = ' ';
			lcdFb[1][sprintf(&lcdFb[1][0], "%s", "Check ETH Cable")] = ' ';
		}
	}
	else {
		time_t t;
		time(&t);

		// Local time offset(KOR + 9 hour)
		t += (9 * 60 * 60);
	
		sprintf(lcdFb[0], "Time %s", ctime(&t));
		lcdFb[0][strlen(lcdFb[0])-1] = ' ';
		lcdFb[0][strlen(lcdFb[0])] = ' ';
	}
 
	for(i = 0; i < LCD_ROW; i++) {
		lcdPosition (lcdHandle, 0, i);
		for(j = 0; j < LCD_COL; j++)
			lcdPutchar(lcdHandle, lcdFb[i][j]);
	}
}

//------------------------------------------------------------------------------------------------------------
int system_init(void)
{
	int i;
 
	// LCD Init   
	lcdHandle = lcdInit(LCD_ROW, LCD_COL, LCD_BUS,
				PORT_LCD_RS, PORT_LCD_E,
				PORT_LCD_D4, PORT_LCD_D5,
				PORT_LCD_D6, PORT_LCD_D7, 0, 0, 0, 0);
 
	if(lcdHandle < 0) {
		fprintf(stderr, "%s : lcdInit failed!\n", __func__);
		return -1;
	}

	// Button Pull Up Enable.
	pinMode (PORT_BUTTON1, INPUT);
	pullUpDnControl (PORT_BUTTON1, PUD_UP);

	pinMode (PORT_BUTTON2, INPUT);
	pullUpDnControl (PORT_BUTTON2, PUD_UP);

	// GPIO Init(LED Port ALL Output)
	for (i = 0; i < MAX_LED_CNT; i++) {
		pinMode (ledPorts[i], OUTPUT);
		digitalWrite(ledPorts[i], 0);
	}

	return  0;
}
 
//------------------------------------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
	int timer = 0;
 
	wiringPiSetup();
 
	if (system_init() < 0) {
		fprintf (stderr, "%s: System Init failed\n", __func__);
		return -1;
	}

	for(;;) {
		usleep(100000);

		if (millis () < timer)
			continue;
		timer = millis () + LCD_UPDATE_PERIOD;
 
		// lcd update
		lcd_update();

		if 		(!digitalRead (PORT_BUTTON1))	dispMode = 0;
		else if (!digitalRead (PORT_BUTTON2))	dispMode = 1;
	}
 
	return 0 ;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
