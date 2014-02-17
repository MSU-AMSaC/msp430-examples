
/**
 * General purpose setup to get the board running.
 */
static inline void setup
  (void)
  __attribute__((always_inline));

static inline void setup
  (void)
{
  WDTCTL = WDTPW | WDTHOLD; /* stop watchdog */
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);               /* trap CPU if clock is not calibrated */
  } 
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}

/**
 * Set up the watchdog as an interval timer interrupt.
 */
static inline void timer_setup
  (void)
  __attribute__((always_inline));

static inline void timer_setup
  (void)
{
  WDTCTL  =  WDT_ADLY_16; /* interrupt every ~16 ms */
  IE1    &= ~NMIIE;
  IE1    |=  WDTIE;
}


/**
 * Initial ADC configuration. 
 *
 * @param channels  The number of channels to sample.
 * @param rate      The starting sample rate in Hz.
 */
 static inline void adc_setup 
  (const uint8_t channels, const uint8_t rate)
  __attribute__((always_inline));

 static inline void adc_setup 
  (const uint8_t channels, const uint8_t rate)
{
    /* SET REF VOLTAGES AND CHANNELS */
  ADC10CTL1 = SHS_1 |                  /* use Timer_A0 */
              INCH_5 |
              CONSEQ_3;// |               /* repeat sequence of channels */
//              (((channels-1) & 0xF)*0x1000u); /* sequence through the requested */
                                       /* number of channels */

  ADC10CTL0 = SREF_1 |
              REFON |
              REF2_5V |
              ADC10SHT_3 |             /* sample and hold for 64 clocks */
              ADC10ON |                /* ADC on */
              ADC10IE;                 /* interrupt active */

  /* DELAY WHILE REF VOLTAGE SETTLES */
  TACCR0   = 30;                            // Delay to allow Ref to settle
  TACTL    = TASSEL_2 | MC_1;               // TACLK = SMCLK, Up mode.
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  __bis_SR_register(LPM0_bits | GIE);  /* low power with interrupts enabled */
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();

  ADC10AE0   = 0x38;//(channels-1);            /* analog inputs on */
//  ADC10CTL0 |= ENC;
  
  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  TACCTL1 = OUTMOD_3;                  /* When counter == TACCR1, set output. */
                                       /* When counter == TACCR0, clear output. */
  TACCR0 = 0xf424 / channels;                     /* 62500 * 64 / (16 MHz / 2 / 2) = 1.0 s */
  TACCR1 = 0xf424 / channels;                     /* 50% duty cycle */
  TACTL = TASSEL_2 |                   /* Source from 16 MHz SMCLK */
          MC_1 |                       /* Count up */
          ID_3;                        /* divide clock by 8 */

}

