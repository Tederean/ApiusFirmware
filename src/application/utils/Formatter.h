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
      value *= pow(10.0f, requestedDigits - digitsCountBeforeDecimalSeparator);
    }

    value = roundf(value);
      
    if (digitsCountBeforeDecimalSeparator < requestedDigits)
    {
      value *= pow(0.1f, requestedDigits - digitsCountBeforeDecimalSeparator);
    }
      
    return value;
  }

  void FormatForBinary(float value, const char* unit, char* output, size_t outputLength)
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
    size_t formatStringLength = sizeof(formatString) / sizeof(char);

    strncpy(formatString, "%.", formatStringLength);
    snprintf(formatString +2, formatStringLength -2, "%i", digitsCountAfterDecimalSeparator);
    strncat(formatString, "f", formatStringLength);


    snprintf(output, outputLength, formatString, roundedValue);
    strncat(output, " ", outputLength);

    if (unitCounter == 1)
    {
      strncat(output, "Ki", outputLength);
    }
    if (unitCounter == 2)
    {
      strncat(output, "Mi", outputLength);
    }
    if (unitCounter == 3)
    {
      strncat(output, "Gi", outputLength);
    }
    if (unitCounter == 4)
    {
      strncat(output, "Ti", outputLength);
    }
    if (unitCounter == 5)
    {
      strncat(output, "Pi", outputLength);
    }

    strncat(output, unit, outputLength);
  }

} // namespace Formatter

#endif
