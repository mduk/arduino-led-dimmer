#include <EEPROM.h>
#include <Tlc5940.h>
#include <tlc_config.h>
#include <tlc_shifts.h>
#include <tlc_fades.h>

int mode = 1;

void setup()
{
  Serial.begin( 9600 );
  loadMode();
  attachInterrupt( 0, changeMode, FALLING );
  Tlc.init();
}

void loop()
{
  if ( Serial.available() )
  {
    mode = Serial.read() - 48;
    saveMode();
    reset();
  }
  
  switch ( mode )
  {
    case 0: staticLoop(); break;
    case 1: rampLoop(); break;
    case 2: wavesLoop(); break;
    case 3: pulseLoop(); break;
    case 4: larsonLoop(); break;
    case 5: randomfadesLoop(); break;
  }
}

void changeMode()
{
  if ( mode == 5 )
  {
    mode = 0 ;
  }
  else
  {
    mode++;
  }
  
  saveMode();
  reset();
}

void saveMode()
{
  EEPROM.write( 0, mode );
}

int loadMode()
{
  mode = EEPROM.read( 0 );
}

void reset()
{
  Tlc.setAll( 0 );
  Tlc.update();
}

void staticLoop()
{
  Tlc.setAll( map( analogRead( A0 ), 0, 1023, 0, 4095 ) );
  Tlc.update();
}

int ramp_brightnessIncrement = 128;
uint16_t ramp_prevBrightness = 1;
int ramp_direction = 0; // 0: Up 1: Down
int ramp_lowCount = 0;

void rampLoop()
{
  uint16_t brightness = 1;
  
  switch ( ramp_direction )
  {
    case 0: 
      brightness = ramp_prevBrightness + ramp_brightnessIncrement;
      
      if ( brightness == 4096 )
      {
        ramp_direction = 1;
      }
      break;
    
    case 1:
      brightness = ramp_prevBrightness - ramp_brightnessIncrement;
      
      if ( brightness == 1 )
      {
        ramp_direction = 0;
      }
  }
  
  tlc_shiftUp();
  Tlc.set( 0, brightness - 1 );
  Tlc.update();
  ramp_prevBrightness = brightness;
  delay( 50 );
}

int waves_increment = 0;
int waves_direction = 0; // 0 or 1, it's all relative

void wavesLoop()
{
  uint16_t brightness = waves_increment * 256;
  
  if ( waves_direction == 0 )
  {
    tlc_shiftUp();
    Tlc.set( 0, brightness );
  }
  else
  {
    tlc_shiftDown();
    Tlc.set( 15, brightness );
  }
  
  Tlc.update();
  
  if ( waves_increment == 16 )
  {
    waves_increment = 0;
  }
  else
  {
    waves_increment++;
  }
  
  delay( 20 );
}

int pulse_increment = 0;
int pulse_direction = 0; // 0 or 1, it's all relative
int pulse_length = 4;

void pulseLoop()
{
  uint16_t brightness = 0;
  
  if ( pulse_increment < pulse_length )
  {
    brightness = pulse_increment * ( 4096 / pulse_length );
  }
  else
  {
    brightness = 0;
  }
  
  if ( pulse_direction == 0 )
  {
    tlc_shiftUp();
    Tlc.set( 0, brightness );
  }
  else
  {
    tlc_shiftDown();
    Tlc.set( 15, brightness );
  }
  
  Tlc.update();
  
  if ( pulse_increment == pulse_length + 16 )
  {
    pulse_increment = 0;
  }
  else
  {
    pulse_increment++;
  }
  
  delay( 40 );
}

TLC_CHANNEL_TYPE larson_channel = 0;
int larson_direction = 0;
int larson_period = 750;
int larson_ledperiod = larson_period / 16;

void larsonLoop()
{
  uint32_t lastMillis = millis();
  tlc_addFade( larson_channel, 4095, 0, lastMillis + 2, lastMillis + (uint16_t) larson_period / 2.8 );
             
  if ( larson_direction == 0 )
  {
    larson_channel++;
    
    if ( larson_channel == 15 )
    {
      larson_direction = 1;
    }
  }
  else if ( larson_direction == 1 )
  {
    larson_channel--;
    
    if ( larson_channel == 0 )
    {
      larson_direction = 0;
    }
  }
  
  uint32_t currentMillis;
  do {
    currentMillis = millis();
    tlc_updateFades( currentMillis );
  } while ( currentMillis - lastMillis <= larson_ledperiod );
}

TLC_CHANNEL_TYPE randomfades_channel = 0;

void randomfadesLoop()
{
  if ( tlc_fadeBufferSize < TLC_FADE_BUFFER_LENGTH - 2 )
  {
    if ( !tlc_isFading( randomfades_channel ) )
    {
      uint16_t duration = random( 100, 1000 ) * 4;
      int maxValue = random( 0, 1024 ) * 4;
      uint32_t startMillis = millis() + 50;
      uint32_t endMillis = startMillis + duration;
      tlc_addFade( randomfades_channel, 0, maxValue, startMillis, endMillis );
      tlc_addFade( randomfades_channel, maxValue, 0, endMillis, endMillis + duration );
    }
    
    if ( randomfades_channel++ == NUM_TLCS * 16 )
    {
      randomfades_channel = 0;
    }
  }
  
  tlc_updateFades();
}
