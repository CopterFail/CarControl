/*  SUMD serial (115200,8,n,1) protocol connected to usart RX line. 
 	The packet will be send every 10ms.
 
 I am using one, out of 3 uarts build into the Teensy 3.1.
 UART rx line is definable by the user.
 
 This receiver code also utilizes a more advanced failsafe sequence.
 In case of receiver malfunction / signal cable damage, RX Failasefe will kicks in
 which will start auto-descent sequence.
 
 */

#define SUMD_SYNC 0xA8
#define RX_CHANNELS 16
#define RX_BUFFER_SIZE 37  // (3 + RX_CHANNELS * 2 + 2)

#define SUMD_IS_ACTIVE

volatile uint16_t RX[RX_CHANNELS] = {
  1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
volatile uint8_t tmpRX[RX_BUFFER_SIZE];
uint16_t ui16RxCnt=0;
uint16_t ui16FailCnt=0;

volatile uint16_t RX_failsafeStatus;
volatile uint8_t RX_signalReceived = 0;

bool failsafeEnabled = false;

// raw data: 8800 12000 15200

// ISR
void UART_RECEIVER_ISR() 
{
}

void ReceiverReadPacket() 
{

  uint8_t	    data, i;
  static uint8_t    channelCnt=RX_CHANNELS;
  static uint8_t    state=0;
  static uint8_t    rxsize=RX_BUFFER_SIZE;

  while (Serial1.available()) 
  {
    data = Serial1.read();

    if( 0 == state) 
    {
      if (data == SUMD_SYNC) 
      {
        tmpRX[state] = data; 
        state++;
      }
    }
    else if( 1 == state ) 
    {
      // 0x01 SUMD live
      // 0x81 Failsave
      tmpRX[state++] = data; 
    }
    else if( 2 == state ) 
    {
      channelCnt = data;
      tmpRX[state++] = data; 
      rxsize = 3 + ( channelCnt << 1 ) + 2;
    }
    else if( state < rxsize )
    {
      tmpRX[state++] = data;
    }

    if( state == rxsize )
    {
      /* SUMD has 16 bit CCITT CRC */
      uint16_t crc = 0;
      for (int n = 0; n < (rxsize-2); n++) 
      {
        crc ^= (uint16_t)tmpRX[n] << 8;
        for ( i = 0; i < 8; i++ )
          crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
      }
      if (crc ^ (((uint16_t)tmpRX[rxsize-2] << 8) | tmpRX[rxsize-1]))
      {
        /* wrong crc checksum found */
        ui16FailCnt++;
      }
      else
      {
        // calculate RX data
        for( i=0; i<channelCnt; i++ )
        {
          uint16_t val = ((uint16_t)tmpRX[4+2*i]) | ((uint16_t)tmpRX[3+2*i] << 8 );
          RX[i] =  (uint16_t)( val / 6.4f - 375 );
        }
        ui16RxCnt++;
        if( tmpRX[1] != 0x81 )
        {
          RX_failsafeStatus = 0;
        }
      }
      state = 0;
    }
  }        
}


void initializeReceiver() {
  // Serial1.setup
  Serial1.begin(115200);

}

void RX_failSafe() {
  if (RX_failsafeStatus > 0) {
    RX_signalReceived++; // if this flag reaches 10, an auto-descent routine will be triggered.
  } 
  else {
    // Raise the FLAGS
    RX_failsafeStatus |= (1 << RX_CHANNELS) - 1;

    // Reset the counter
    RX_signalReceived = 0;
  }

  if (RX_signalReceived >= 10) {
    RX_signalReceived = 10; // don't let the variable overflow
    failsafeEnabled = true; // this ensures that failsafe will operate in attitude mode

      // Bear in mind that this is here just to "slow" the fall, if you have lets say 500m altitude,
    // this probably won't help you much (sorry).
    // This will slowly (-2 every 100ms) bring the throttle to 1000 (still saved in the PPM array)
    // 1000 = 0 throttle;
    // Descending from FULL throttle 2000 (most unlikely) would take about 1 minute and 40 seconds
    // Descending from HALF throttle 1500 (more likely) would take about 50 seconds
    //        RX[CONFIG.data.CHANNEL_ASSIGNMENT[THROTTLE]] -= 2;

    //        if (RX[CONFIG.data.CHANNEL_ASSIGNMENT[THROTTLE]] < 1000) {
    //            RX[CONFIG.data.CHANNEL_ASSIGNMENT[THROTTLE]] = 1000; // don't let the value fall below 1000
    //        }    
  } 
  else {
    failsafeEnabled = false;
  }
}



