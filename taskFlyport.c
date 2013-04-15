#include "taskFlyport.h"
#include "RS232Helper.h"

#define		TX_232		p4
#define		RX_232		p6

const int port232 = 	3;

#define buffsSize	5

char buff1[buffsSize];
char Msg[50];

void FlyportTask()
{
	// Initialize the RS232
	RS232Off(port232);
	RS232Remap(port232, TX_232, RX_232, 0, 0);
	RS232Init(port232, 9600);
	RS232SetParam(port232, RS232_STOP_BITS, RS232_ONE_STOP);
	RS232SetParam(port232, RS232_DATA_PARITY, RS232_8BITS_PARITY_NONE);
	RS232On(port232);
	vTaskDelay(50);
	
	WFConnect(WF_DEFAULT);
	while (WFStatus != CONNECTED);
	UARTWrite(1,"Flyport connected... hello world!\r\n");
	
	vTaskDelay(100);
	
	while(1)
	{
		if( RS232BufferSize(port232) > 0 )
		{
			int len232 = 4;		// Output of TX pin is string of four characters
			
			RS232Flush(port232);	// Flush buffer to get latest value
			
			DWORD tim1 = TickGetDiv256();
			while((RS232BufferSize(port232) < buffsSize) && ((TickGetDiv256() - tim1) < 100))
			{
				len232 = RS232BufferSize(port232);
			}
			
			RS232Read(port232, buff1, len232);
			buff1[len232] = '\0';		// assigning null terminator to last position
			
			if(buff1[0] == 'R')
				buff1[0] = '+';			// replace R character with + for atoi() function below
			
			unsigned int inches = atoi(buff1);	// convert char array to int
			double cm = (double)inches * 2.54;	// convert inches to cm
			
			sprintf(Msg, "Distance is %u inches (%.2f cm). \r\n", inches, cm);
			UARTWrite(1, Msg);
			vTaskDelay(100);
		}		
	}
}
