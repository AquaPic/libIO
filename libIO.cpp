/*
  libDAC - An I2C Extended IO Library for chipKit.
  Texas Instrurments PCF8574 over I2C serial communications.

  Created by Skyler Brandt on May 2013 for chipKit.

  Copyright 2013 Skyler Brandt
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#include <inttypes.h>
#include <WProgram.h>
#include <Wire.h>
#include <libIO.h>

/******************************************************************************
 * Class Variables
 ******************************************************************************/
int libIO::address = 0;
uint8_t libIO::outReg = 0;  //Register sent to device
bool libIO::out[8] = {true, true, true, true, true, true, true, true};
uint8_t libIO::inReg = 0;

/******************************************************************************
 * Constructors
 ******************************************************************************/
libIO::libIO(int initAddress)
{
  address = initAddress;
}

/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**********************************************************
 *setup the device to use as an output (0) or input (1)
 **********************************************************/
void libIO::init(char inOut)
{
    if (inOut == 1)
	{
		allOn();
	}
	else if (inOut == 0)
	{
		allOff();
	}
}

/**********************************************************
 *turn single bit output on
 **********************************************************/
void libIO::setOutOn(char bit)
{
  //add binary bit to byte being sent
  switch(bit)
  {
  case 0:
    if (!out[0])
    {
      outReg += 1;
      out[0] = true;
    }
    break;
  case 1:
    if (!out[1])
    {
      outReg += 2;
      out[1] = true;
    }
    break;
  case 2:
    if (!out[2])
    {
      outReg += 4;
      out[2] = true;
    }
    break;
  case 3:
    if (!out[3])
    {
      outReg += 8;
      out[3] = true;
    }
    break;
  case 4:
    if (!out[4])
    {
      outReg += 16;
      out[4] = true;
    }
    break;
  case 5:
    if (!out[5])
    {
      outReg += 32;
      out[5] = true;
    }
    break;
  case 6:
    if (!out[6])
    {
      outReg += 64;
      out[6] = true;
    }
    break;
  case 7:
    if (!out[7])
    {
      outReg += 128;
      out[7] = true;
    }
    break;
  default:
    break;
  }

  //send byte
  setIO(outReg);
}

/**********************************************************
 *turn single bit output off
 **********************************************************/
void libIO::setOutOff(char bit)
{
  //subtract binary bit from byte being sent
  switch(bit)
  {
  case 0:
    if (out[0])
    {
      outReg -= 1;
      out[0] = false;
    }
    break;
  case 1:
    if (out[1])
    {
      outReg -= 2;
      out[1] = false;
    }
    break;
  case 2:
    if (out[2])
    {
      outReg -= 4;
      out[2] = false;
    }
    break;
  case 3:
    if (out[3])
    {
      outReg -= 8;
      out[3] = false;
    }
    break;
  case 4:
    if (out[4])
    {
      outReg -= 16;
      out[4] = false;
    }
    break;
  case 5:
    if (out[5])
    {
      outReg -= 32;
      out[5] = false;
    }
    break;
  case 6:
    if (out[6])
    {
      outReg -= 64;
      out[6] = false;
    }
    break;
  case 7:
    if (out[7])
    {
      outReg -= 128;
      out[7] = false;
    }
    break;
  default:
    break;
  }

  //send byte
  setIO(outReg);
}

/**********************************************************
 *turn all outputs off
 **********************************************************/
void libIO::allOff()
{
  outReg = 0x00;
  setIO(outReg);
  for (char i=0 ; i<8 ; i++)
  {
      out[i] = false;
  }
}

/**********************************************************
 *turn all outputs on
 **********************************************************/
void libIO::allOn()
{
  outReg = 0xFF;
  setIO(outReg);
  for (char i=0 ; i<8 ; i++)
  {
      out[i] = true;
  }
}

/**********************************************************
 *check specific bit status
 **********************************************************/
bool libIO::bitStat(char bit)
{
  uint8_t inByte = inputReg(); //get input register from device
  inReg = inByte;  //save to class variable
  inByte >>= bit;  //shift off bits
  inByte = inByte & 0x01;
  if (inByte == 0x01)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool libIO::bitStat(char bit, bool update)
{
  uint8_t byte = 0;

  if (update)
  {
    byte = inputReg();  //get input register from device
    inReg = byte;  //save to class variable
  }
  else
  {
    byte = inReg;  //used if already updated input register on that scan and want to check the status of another bit
  }
  byte >>= bit;  //shift off bits
  byte = byte & 0x01;
  if (byte == 0x01)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**********************************************************
 *returns pointer to an array of booleans on the status of the input byte
 *  use only if entire device is used for inputs
 **********************************************************/
bool* libIO::inputArray()
{
  static bool in[] = {false, false, false, false, false, false, false, false};
  uint8_t byte = inputReg(); //get input register from device
  uint8_t tempByte = 0;
  for (char i=0 ; i<0 ; i++)
  {
    tempByte = byte;
    tempByte >>= i;
    tempByte = byte & 0x01;
    if (tempByte == 0x01)
    {
      in[i] = true;
    }
  }
  return in;
}

/******************************************************************************
 *Private Functions
 ******************************************************************************/

/**********************************************************
 *send output Registers via I2C to device
 **********************************************************/
void libIO::setIO(uint8_t outputReg)
{
  uint8_t sendByte[2] = {0, outputReg};
  Wire.beginTransmission(address);
  Wire.send(sendByte, 2);
  Wire.endTransmission();
}

/**********************************************************
 *read input register
 **********************************************************/
uint8_t libIO::inputReg()
{
  uint8_t registerByte = 0;
  Wire.requestFrom(address,2);
  while(Wire.available())
  {
    registerByte = Wire.receive();
    registerByte = Wire.receive();
  }

  return registerByte;
}
