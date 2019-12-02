/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
    This file is part of VRGimbal by VirtualRobotix Italia s.c.a.r.l..

    VRGimbal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VRGimbal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VRGimbal.  If not, see <http://www.gnu.org/licenses/>.

    Please refer to http://vrgimbal.wordpress.com for more information
*/

/**
 * SerialCommand - A Wiring/Arduino library to tokenize and parse commands
 * received over a serial port.
 * 
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 * 
 * Version 20120522
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SerialCommand.h"

//compatibilità
#define F(x) x

//For the standard ASCII character set (used by the "C" locale), printing characters are all with an ASCII code greater than 0x1f (US), except 0x7f (DEL).
bool isprint(char c)
{
	if ((c > 0x1f) && ( c != 0x7f))
		return true;
	return false;
}

char tolower(char c)
{
	if ((c >= 'A') && (c <= 'Z'))
	{
		c = c + ('a' - 'A');
	}
	return c;
}


/**
 * Constructor makes sure some things are set.
 */
SerialCommand::SerialCommand(FastSerial * ser_port)
  : commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\n'),           // default terminator for commands, newline character
    last(NULL),
    _ser_port(ser_port)
{
  //strcpy(delim, " "); // strtok_r needs a null-terminated string
	delim[0] = ' ';
	delim[1] = '\0';
  clearBuffer();
}


void SerialCommand::setSerialPort(FastSerial * ser_port)
{
	if (_ser_port != ser_port)
	{
		clearBuffer();
		_ser_port = ser_port;
	}
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void SerialCommand::addCommand(const char *command, void (*function)()) {
  #ifdef SERIALCOMMAND_DEBUG
    _ser_port->print(F("Adding command ("));
    _ser_port->print(commandCount);
    _ser_port->print(F("): "));
    _ser_port->println(command);
  #endif

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function = function;
  commandCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void SerialCommand::setDefaultHandler(void (*function)(const char *)) {
  defaultHandler = function;
}


/**
 * This checks the Serial stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void SerialCommand::readSerial() {

	while (_ser_port->available() > 0) {
	//while (true) {
	  char inChar = 0;
    //char inChar = _ser_port->read();   // Read single available character, there may be more waiting
	  int ic = _ser_port->read();
	  if (ic > 0)
		  inChar = (char) ic;
	  else
		  break;

    #ifdef SERIALCOMMAND_DEBUG
      _ser_port->print(inChar);   // Echo back to serial stream
    #endif

    //if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
     if ((inChar == '\r') || (inChar == '\n')) {
    	 //if (strlen(buffer) > 0)
    	 //{
		  #ifdef SERIALCOMMAND_DEBUG
			_ser_port->print(F("Received: "));
			_ser_port->println(buffer);
		  #endif

		  char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
		  //char *command = strtok_r(buffer, " ", &last);   // Search for command at start of buffer
		  if (command != NULL) {
			boolean matched = false;
			for (int i = 0; i < commandCount; i++) {
			  #ifdef SERIALCOMMAND_DEBUG
				_ser_port->print(F("Comparing ["));
				_ser_port->print(command);
				_ser_port->print(F("] to ["));
				_ser_port->print(commandList[i].command);
				_ser_port->println(F("]"));
			  #endif

			  // Compare the found command against the list of known commands for a match
			  for (int u = 0; command[u] != '\0'; u++)   // as no strnicmp exists ...
				command[u] = (char)tolower(command[u]);

			  if (strncmp(command, commandList[i].command, SERIALCOMMAND_MAXCOMMANDLENGTH) == 0) {
				#ifdef SERIALCOMMAND_DEBUG
				  _ser_port->print(F("Matched Command: "));
				  _ser_port->println(command);
				#endif

				// Execute the stored handler function for the command
				(*commandList[i].function)();
				matched = true;
				break;
			  }
			}
			if (!matched && (defaultHandler != NULL)) {
			  (*defaultHandler)(command);
			}
		  }
    	 //}
		  // _ser_port->println(F("BruGi> ")); // TODO: BruGi prompt string
		  clearBuffer();

    }
    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < SERIALCOMMAND_BUFFER) {
        buffer[bufPos] = inChar;  // Put character into buffer
        bufPos++;
        buffer[bufPos] = '\0';      // Null terminate
      } else {
        #ifdef SERIALCOMMAND_DEBUG
          _ser_port->println(F("Line buffer is full - increase SERIALCOMMAND_BUFFER"));
        #endif
      }
    }
  }
}

/*
 * Clear the input buffer.
 */
void SerialCommand::clearBuffer() {
  //buffer[0] = '\0';
	memset(buffer, 0, SERIALCOMMAND_BUFFER);
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *SerialCommand::next() {
  return strtok_r(NULL, delim, &last);
	//return strtok_r(NULL, " ", &last);
}
