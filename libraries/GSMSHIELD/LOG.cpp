/*
  LOG.cpp - Library for standard logging convention.
  Created by Meir Michanie, June 9, 2010.
  Released into the public domain.
  Version 0.1
*/

#include "LOG.h"

LOG::LOG(int level)
{
  setLevel(level);
}

void LOG::DATA(const char* string)
{
 if (_level > 4){
  DEBUG_SERIAL.print(string);
 }
}

void LOG::DATA(int number)
{
 if (_level > 4){
  DEBUG_SERIAL.print(number);
 }
}

void LOG::DEBUG(const char* string)
{
 if (_level > 3){
  DEBUG_SERIAL.print("\n[DEBUG]: ");
  DEBUG_SERIAL.println(string);
 }
}

void LOG::INFO(const char* string)
{
 if (_level > 2){
   DEBUG_SERIAL.print("\n[INFO]: ");
   DEBUG_SERIAL.println(string);
 }
}

void LOG::WARNING(const char* string)
{
 if (_level > 1){
  DEBUG_SERIAL.print("\n[WARNING]: ");
  DEBUG_SERIAL.println(string);
 }
}

void LOG::CRITICAL(const char* string)
{
 if (_level > 0){
  DEBUG_SERIAL.print("\n[CRITICAL]: ");
  DEBUG_SERIAL.println(string);
 }
}
