/*  SBUS serial (100000,8,e,2) protocol connected to usart RX line.
 	The packet will be send every ???ms.
 	Between packet is a 4ms gap.
 
 I am using one, out of 3 uarts build into the Teensy 3.1.
 UART rx line is definable by the user.
 
 This receiver code also utilizes a more advanced failsafe sequence.
 In case of receiver malfunction / signal cable damage, RX Failasefe will kicks in
 which will start auto-descent sequence.
 
 */

#define SBUS_START 0x0F
#define SBUS_END 0x00
#define SBUS_FRAME_SIZE 25  // (START 16Channelx11Bits Status Stop), https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/
#define SBUS_FLAG_CHANNEL_17        (1 << 0)
#define SBUS_FLAG_CHANNEL_18        (1 << 1)
#define SBUS_FLAG_SIGNAL_LOSS       (1 << 2)
#define SBUS_FLAG_FAILSAFE_ACTIVE   (1 << 3)

static uint8_t sbusbytes[SBUS_FRAME_SIZE];
uint16_t ui16RxCnt=0;
uint16_t ui16FailCnt=0;

volatile uint16_t RX_failsafeStatus;
volatile uint8_t RX_signalReceived = 0;

bool failsafeEnabled = false;

// raw data: 8800 12000 15200

// ISR
void UART_RECEIVER_ISR( void )
{
}

void SBus_UnrollChannels( uint8_t *s, volatile uint16_t *d)
{
#define FR(v,s) (((v) >> (s)) & 0x7ff)

	/* unroll channels 1-8 */
	*d++ = FR(s[0] | s[1] << 8, 0);
	*d++ = FR(s[1] | s[2] << 8, 3);
	*d++ = FR(s[2] | s[3] << 8 | s[4] << 16, 6);
	*d++ = FR(s[4] | s[5] << 8, 1);
	*d++ = FR(s[5] | s[6] << 8, 4);
	*d++ = FR(s[6] | s[7] << 8 | s[8] << 16, 7);
	*d++ = FR(s[8] | s[9] << 8, 2);
	*d++ = FR(s[9] | s[10] << 8, 5);

	/* unroll channels 9-16 */
	*d++ = FR(s[11] | s[12] << 8, 0);
	*d++ = FR(s[12] | s[13] << 8, 3);
	*d++ = FR(s[13] | s[14] << 8 | s[15] << 16, 6);
	*d++ = FR(s[15] | s[16] << 8, 1);
	*d++ = FR(s[16] | s[17] << 8, 4);
	*d++ = FR(s[17] | s[18] << 8 | s[19] << 16, 7);
	*d++ = FR(s[19] | s[20] << 8, 2);
	*d++ = FR(s[20] | s[21] << 8, 5);

	/* unroll discrete channels 17 and 18 */
	//*d++ = (s[22] & SBUS_FLAG_DC1) ? SBUS_VALUE_MAX : SBUS_VALUE_MIN;
	//*d++ = (s[22] & SBUS_FLAG_DC2) ? SBUS_VALUE_MAX : SBUS_VALUE_MIN;
}
void ReceiverReadPacket( void )
{

  uint8_t	    data;
  static uint8_t    state=0;

  while (Serial1.available()) 
  {
    data = Serial1.read();

    if( 0 == state) 
    {
      if (data == SBUS_START)
      {
    	  sbusbytes[state++] = data;
      }
    }
    else
    {
    	sbusbytes[state++] = data;
    }

    if(state >= SBUS_FRAME_SIZE )
    {
    	if( data != SBUS_END )
    	{
    		ui16FailCnt++;
    	}
    	else
    	{
    		RX_failsafeStatus = 0;
    		SBus_UnrollChannels( &sbusbytes[1], &RX[0]);
            ui16RxCnt++;
            if( sbusbytes[24] & SBUS_FLAG_FAILSAFE_ACTIVE )
            {
              RX_failsafeStatus = 1;
            }
            if( sbusbytes[24] & SBUS_FLAG_SIGNAL_LOSS )
            {
              RX_failsafeStatus = 1;
            }
    	}

    	for( state=0; state<16; state++ )
    	{
    	  RX[state] = (0.625f * RX[state]) + 880;
    	  //Serial.print( RX[state] );
    	  //Serial.print( " " );
    	}
    	//Serial.println( sbusFrame.frame.flags );
    	state = 0;
    }
  }        
}


void initializeReceiver( void )
{
  Serial1.begin(100000, SERIAL_8E1_RXINV_TXINV);	// only 1 stopbit, sbus is defined for 2 stopbits
}

void RX_failSafe( void )
{
  if (RX_failsafeStatus > 0)
  {
    RX_signalReceived++;
  } 
  else
  {
    // Raise the FLAGS
    RX_failsafeStatus |= 0xffff;

    // Reset the counter
    RX_signalReceived = 0;
  }

  if (RX_signalReceived >= 10)
  {
    RX_signalReceived = 10; // don't let the variable overflow
    failsafeEnabled = true; //
  } 
  else
  {
    failsafeEnabled = false;
  }
}



