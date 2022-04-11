#ifndef _Formatter_
#define _Formatter_

#include <Arduino.h>

using namespace std;

namespace Formatter
{

  uint8_t GetDigitsCountBeforeDecimalSeparator(float value)
  {
    auto absoluteValue = abs(value);

    return absoluteValue < 1.0f ? 0 : (uint8_t)(log10(absoluteValue) + 1.0f);
  }

  float RoundToNDigits(float value, uint8_t requestedDigits)
  {
    auto digitsCountBeforeDecimalSeparator = GetDigitsCountBeforeDecimalSeparator(value);
      
    if (digitsCountBeforeDecimalSeparator < requestedDigits)
    {
      value *= (requestedDigits - digitsCountBeforeDecimalSeparator) * 10.0f;
    }

    value = roundf(value);
      
    if (digitsCountBeforeDecimalSeparator < requestedDigits)
    {
      value *= (requestedDigits - digitsCountBeforeDecimalSeparator) * 0.1f;
    }
      
    return value;
  }




  
  void FormatForBinary(float value, const char* unit, char* output)
  {
    auto unitCounter = (uint8_t)0;
    
    while (value >= 1000.0f)
    {
        value /= 1024.0f;
        unitCounter++;
    }
    
    auto roundedValue = RoundToNDigits(value, 3);
    
    if (roundedValue >= 1000.0f)
    {
        value /= 1024.0f;
        unitCounter++;
        
        roundedValue = RoundToNDigits(value, 3);
    }
    
    auto digitsCountAfterDecimalSeparator = 3 - GetDigitsCountBeforeDecimalSeparator(roundedValue);


    char formatString[10];

    strcpy(formatString, "%.");
    sprintf(formatString + 2, "%i", digitsCountAfterDecimalSeparator);
    strcat(formatString, "f");


    sprintf(output, formatString, roundedValue);
    strcat(output, " ");

    if (unitCounter == 1)
    {
      strcat(output, "Ki");
    }
    if (unitCounter == 2)
    {
      strcat(output, "Mi");
    }
    if (unitCounter == 3)
    {
      strcat(output, "Gi");
    }
    if (unitCounter == 4)
    {
      strcat(output, "Ti");
    }

    strcat(output, unit);
  }

} // namespace Formatter

#endif
