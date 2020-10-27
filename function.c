/*
 * function.c
 *
 * Created: 14-Jan-20 16:09:51
 * Authors : Facundo Busano & Fernando Ribero
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include "function.h"
#include "usart.h"

//VARs AND CONSTANTS DEFINITION
unsigned char cAZ_PosArray[3];
unsigned char cEL_PosArray[2];
unsigned int iAZ_PosNum = 0;
unsigned int iEL_PosNum = 0;
unsigned char cAZ_DegreeArray[3];
unsigned char cEL_DegreeArray[2];
unsigned int iAZ_DegreeNum = 0;
unsigned int iEL_DegreeNum = 0;
unsigned int iAZ_Pulses = 0;
unsigned int iEL_Pulses = 0;
unsigned int iAZ_PosPulses = 0;
unsigned int iEL_PosPulses = 0;
unsigned int i = 0;
unsigned int j = 0;
unsigned int iZ_Char = 0;
unsigned int iE_Char = 0;
unsigned int iU_Char = 0;
unsigned char cAZ_RotorDir = 'R';
unsigned char cEL_RotorDir = 'U';
unsigned int iAZ_OriginFlag = 1;
unsigned int iEL_OriginFlag = 1;
unsigned int iRotatorFlag = 1;
unsigned int iAZ_PulseFlag = 0;
unsigned int iEL_PulseFlag = 0;
unsigned int iSpeedFlag = 0;

/*********************************************
	READ INPUT SERIAL COMMAND FUNCTION
 *********************************************/
void read_command(){                                                                                                                                                                             
	if (cCommand_Array[0] == 'F' && cCommand_Array[1] == 'I' && cCommand_Array[2] == 'R' && cCommand_Array[3] == 'M' && cCommand_Array[4] == 'v'){	//verifies "FIRMv" command
		PORTB |= (1<<ReDePin);																														//sets MAX485 as emitter (EMITTER = HIGH)
		USART0_TX_STRING(FIRMWARE_V);																												//prints Firmware version
		USART0_TX_CHAR(10);																															//sends LF+CR
		_delay_ms(15);
		PORTB &= ~(1<<ReDePin);																														//sets MAX485 as receiver (RECEIVER = LOW)
	}
	if (cCommand_Array[0] == 'S' && cCommand_Array[1] == 'P' && cCommand_Array[2] == 'E' && cCommand_Array[3] == 'E' && cCommand_Array[4] == 'D'){	//verifies "SPEED" command
		PORTB |= (1<<ReDePin);																														//sets MAX485 as emitter (EMITTER = HIGH)
		if(iSpeedFlag){																																//prints rotator speed value
			USART0_TX_STRING("Speed = SET");
			USART0_TX_CHAR(10);																														//sends LF+CR
			_delay_ms(15);
		}
		else{
			USART0_TX_STRING("Speed = FOLLOW");	
			USART0_TX_CHAR(10);																														//sends LF+CR
			_delay_ms(15);
		}
		PORTB &= ~(1<<ReDePin);																														//sets MAX485 as receiver (RECEIVER = LOW)
	}
	if (cCommand_Array[0] == 'F' && cCommand_Array[1] == 'A' && cCommand_Array[2] == 'S' && cCommand_Array[3] == 'T' && cCommand_Array[4] == 's'){	//verifies "FASTs" command
		iSpeedFlag = 1;																																//enable fast speed rotator movement
		PORTB |= (1<<ReDePin);																														//sets MAX485 as emitter (EMITTER = HIGH)
		USART0_TX_STRING("Rotator speed received");
		USART0_TX_CHAR(10);																															//sends LF+CR
		_delay_ms(15);
		PORTB &= ~(1<<ReDePin);																														//sets MAX485 as receiver (RECEIVER = LOW)
	}
	if (cCommand_Array[0] == 'S' && cCommand_Array[1] == 'L' && cCommand_Array[2] == 'O' && cCommand_Array[3] == 'W' && cCommand_Array[4] == 's'){	//verifies "SLOWs" command
		iSpeedFlag = 0;																																//enable slow speed rotator movement
		PORTB |= (1<<ReDePin);																														//sets MAX485 as emitter (EMITTER = HIGH)
		USART0_TX_STRING("Rotator speed received");
		USART0_TX_CHAR(10);																															//sends LF+CR
		_delay_ms(15);
		PORTB &= ~(1<<ReDePin);																														//sets MAX485 as receiver (RECEIVER = LOW)
	}
	if (cCommand_Array[0] == 'S' && cCommand_Array[1] == 'A' && cCommand_Array[3] == 'S' && cCommand_Array[4] == 'E'){								//checks "stop rotors" command (SA_SE_.)
		iRotatorFlag = 1;																															//disables rotator motion (STOP = HIGH)
		memset(cCommand_Array, 0, sizeof(cCommand_Array));																							//cleans command vector
	}
	if (cCommand_Array[0] == 'A' && cCommand_Array[1] == 'Z' && cCommand_Array[2] == ' ' && cCommand_Array[3] == 'E' && cCommand_Array[4] == 'L'){  //checks "get position" command (AZ EL U...)
		unsigned int u = 0;
		convert_to_degree();
		convert_to_char();																															//converts actual AZ and EL pulses to degrees
		PORTB |= (1<<ReDePin);																														//sets MAX485 as emitter (EMITTER = HIGH)
		USART0_TX_CHAR('A');																														//prints AZ and EL degrees (AZ### EL##)
		USART0_TX_CHAR('Z');
		while (u<3 && cAZ_DegreeArray[u] != '#')
		{	
			USART0_TX_CHAR(cAZ_DegreeArray[u]);
			u++;
		}
		u = 0;
		USART0_TX_CHAR(' ');
		USART0_TX_CHAR('E');
		USART0_TX_CHAR('L');
		while (u<2 && cEL_DegreeArray[u] != '#')
		{
			USART0_TX_CHAR(cEL_DegreeArray[u]);
			u++;
		}		
		USART0_TX_CHAR(10);																															//sends LF+CR																																
		_delay_ms(15);
		PORTB &= ~(1<<ReDePin);																														//sets MAX485 as receiver (RECEIVER = LOW)
		memset(cCommand_Array, 0, sizeof(cCommand_Array));																							//cleans command vector
	}
	if (cCommand_Array[0] == 'A' && cCommand_Array[1] == 'Z'){																						//checks "set position" command (AZ###.# EL##.# U...)
		iRotatorFlag= 0;																															//enables rotator motion (GO = LOW)
		memset(cAZ_PosArray, '#', sizeof(cAZ_PosArray));																							//fills AZ_position vector with # (logical technique to detect 1, 2 or 3 characters)
		memset(cEL_PosArray, '#', sizeof(cEL_PosArray));																							//fills EL_position vector with # (logical technique to detect 1, 2 or 3 characters)
		for (i=0 ; i<35 ; i++){																														//command vector scanning to identify reference characters. (A'Z'###.# 'E'L##.# 'U')
			if(cCommand_Array[i] == 'Z'){																											//'Z' character identification
				iZ_Char = i;
			}
			if(cCommand_Array[i] == 'E'){																											//'E' character identification
				iE_Char = i;																														
			}
			if(cCommand_Array[i] == 'U'){																											//'U' character identification
				iU_Char = i;
				i = 0;
				break;
			}
		}
		for (i=iZ_Char+1 ; i<iE_Char-3 ; i++){																										//gets AZ position characters with +-1 degree resolution (from Z+1 to E-3 character)
			cAZ_PosArray[j] = char_to_num(cCommand_Array[i]);																						//fills AZ_position vector with the extracted characters
			j++;
		}
		j = 0;
		for (i=iE_Char+2 ; i<iU_Char-3 ; i++){																										//gets EL position characters with +-1 degree resolution (from E+2 to U-3 character)
			cEL_PosArray[j] = char_to_num(cCommand_Array[i]);																						//fills EL_position vector with the extracted characters
			j++;
		}
		j = 0;
		convert_to_num();																															//converts AZ_position and EL_position vectors to numbers (AZ_num and EL_num)
		convert_to_pulses();																														//converts AZ_num and EL_num numbers to pulses
		memset(cCommand_Array, 0, sizeof(cCommand_Array));																							//cleans command vector
	}
}
/*********************************************
  CONVERT INPUT CHARACTERS TO NUMBERS FUNCTION
 *********************************************/
int char_to_num (char ch){
  int num = 0;
  switch (ch){																																		//compares characters with ASCII table values and sets the equivalent number 
    case 48:
    num = 0;
    break;
    case 49:
    num = 1;
    break;
    case 50:
    num = 2;
    break;
    case 51:
    num = 3;
    break;
    case 52:
    num = 4;
    break;
    case 53:
    num = 5;
    break;
    case 54:
    num = 6;
    break;
    case 55:
    num = 7;
    break;
    case 56:
    num = 8;
    break;
    case 57:
    num = 9;
    break;
  }
  return num;
}

/*********************************************
  CONVERT NUMBER TO CHARACTER FUNCTION
 *********************************************/
char num_to_char (int number){
  char chtr = 0;
  switch (number){																																//compares characters with ASCII table values and sets the equivalent char
    case 0:
    chtr = 48;
    break;
    case 1:
    chtr = 49;
    break;
    case 2:
    chtr = 50;
    break;
    case 3:
    chtr = 51;
    break;
    case 4:
    chtr = 52;
    break;
    case 5:
    chtr = 53;
    break;
    case 6:
    chtr = 54;
    break;
    case 7:
    chtr = 55;
    break;
    case 8:
    chtr = 56;
    break;
    case 9:
    chtr = 57;
    break;
  }
  return chtr;
}

/*********************************
 CONVERT ARRAY TO NUMBER FUNCTION
 *********************************/
void convert_to_num(){																															//AZ_pos_arr = [#][#][#] (degrees)   EL_pos_arr = [#][#] (degrees)
  if(cAZ_PosArray[2] == '#'){																													//checks if there is not a HUNDRED FACTOR number 
    if(cAZ_PosArray[1] == '#'){																													//checks if there is not a TEN FACTOR number
      iAZ_PosNum = (cAZ_PosArray[0]*1);                                                                                                         //sets the AZ_num with the ONE FACTOR number
    }
    else{																																		//checks if there is a TEN FACTOR number
      iAZ_PosNum = (cAZ_PosArray[0]*10) + (cAZ_PosArray[1]*1);                                                                                  //sets the AZ_num with the TEN FACTOR number + ONE FACTOR number
    }
  }
  else{																																			//checks if there is a HUNDRED FACTOR number
    iAZ_PosNum = (cAZ_PosArray[0]*100) + (cAZ_PosArray[1]*10) + (cAZ_PosArray[2]*1);                                                            //sets the AZ_num with the HUNDRED FACTOR number + TEN FACTOR number + ONE FACTOR number
  }
                                                                                                                  
  if(cEL_PosArray[1] == '#'){																													//checks if there is not a TEN FACTOR number 
    iEL_PosNum = (cEL_PosArray[0]*1);																											//sets the EL_num with the ONE FACTOR number
  }
  else{																																			//checks if there is a TEN FACTOR number
    iEL_PosNum = (cEL_PosArray[0]*10) + (cEL_PosArray[1]*1);                                                                                    //sets the EL_num with the TEN FACTOR number + ONE FACTOR number
  }  
}

/******************************************
  CONVERT DEGREES NUMBER TO PULSES FUNCTION
 ******************************************/
void convert_to_pulses(){ 
	iAZ_Pulses = iAZ_PosNum / 0.0225;																											//converts AZ_number (degrees) to AZ_pulses (1 pulse = 0,0225 degrees)
	iEL_Pulses = iEL_PosNum / 0.0225;                                                                                                           //converts EL_number (degrees) to EL_pulses (1 pulse = 0,0225 degrees)
}

/***************************************************
  CONVERT ACTUAL POSITION PULSES TO DEGREES FUNCTION
 ***************************************************/
void convert_to_degree(){
	float num = 0; 
	float num_2 = 0;
	unsigned int dec = 0;
	                                                                                                                  
	num = iAZ_PosPulses * 0.0225;																												//converts the actual AZ_pos_pulses to AZ degrees (1 pulse = 0,0225 degrees)
	iAZ_DegreeNum = num;                                                                                                                        //takes the integer part of the AZ degrees
	num_2 = num - iAZ_DegreeNum;																												//calculates the decimal part of the AZ degrees
	dec = num_2 * 10;																															//multiplies the decimal part of AZ degrees by 10
	if(dec > 5){																																//checks if decimal part of AZ degrees is greater than 5 
		iAZ_DegreeNum = iAZ_DegreeNum + 1;																										//rounds up AZ degrees
	}
	
	num = iEL_PosPulses * 0.0225;																												//converts the actual EL_pos_pulses to EL degrees (1 pulse = 0,0225 degrees)
	iEL_DegreeNum = num;                                                                                                                        //takes the integer part of the EL degrees
	num_2 = num - iEL_DegreeNum;																												//calculates the decimal part of the EL degrees
	dec = num_2 * 10;																															//multiplies the decimal part of EL degrees by 10
	if(dec > 5){																																//checks if decimal part of EL degrees is greater than 5 
		iEL_DegreeNum = iEL_DegreeNum + 1;                                                                                                      //rounds up EL degrees
	}

}

/***************************************************
  CONVERT ACTUAL DEGREES TO CHAR FUNCTION
 ***************************************************/
void convert_to_char(){
	unsigned int centena = 0;
	unsigned int decena = 0;
	unsigned int unidad = 0;
	unsigned int num = 0;
	
	memset(cAZ_DegreeArray, '#', sizeof(cAZ_DegreeArray));
	memset(cEL_DegreeArray, '#', sizeof(cEL_DegreeArray));
	
	centena = iAZ_DegreeNum/100;																												
	
	if (centena > 0)
	{
		num = iAZ_DegreeNum - (centena*100);
		decena = num/10;
		unidad = num - (decena*10);
		cAZ_DegreeArray[0] = num_to_char(centena);
		cAZ_DegreeArray[1] = num_to_char(decena);
		cAZ_DegreeArray[2] = num_to_char(unidad);
	}
	else{
		decena = iAZ_DegreeNum/10;
		if (decena > 0)
		{
			unidad = iAZ_DegreeNum - (decena*10);
			cAZ_DegreeArray[0] = num_to_char(decena);
			cAZ_DegreeArray[1] = num_to_char(unidad);	
		}
		else{
			cAZ_DegreeArray[0] = num_to_char(iAZ_DegreeNum);
		}
	}
	
	decena = iEL_DegreeNum/10;
	if (decena > 0)
	{
		unidad = iEL_DegreeNum - (decena*10);
		cEL_DegreeArray[0] = num_to_char(decena);
		cEL_DegreeArray[1] = num_to_char(unidad);
	}
	else{
		cEL_DegreeArray[0] = num_to_char(iEL_DegreeNum);
	}	                                                                                                                         
}

void move_rotators(){
	if ( (iAZ_PosPulses != iAZ_Pulses) || (iEL_Pulses != iEL_PosPulses) ){                                                                      //checks if AZ rotor and EL rotor are not in the position
		if (iAZ_PosPulses != iAZ_Pulses){                                                                                                       //checks if AZ rotor is not in the position
			if( !(iAZ_PosPulses == 0 && iAZ_Pulses == 15999) && !(iAZ_PosPulses == 15999 && iAZ_Pulses == 0) ){									//checks if the position is 0 or 360 and the new position must be 0 or 360.
				PORTD &= ~(1<<AZ_enable);																										//enables AZ rotator (ENABLE = LOW)
				if (iAZ_Pulses > iAZ_PosPulses){                                                                                                //checks if the AZ pulses to execute are greater than AZ position
					if(11999 <= iAZ_Pulses && iAZ_Pulses <= 15999 && 0 <= iAZ_PosPulses && iAZ_PosPulses <= 3999){                              //checks if the AZ pulses to execute are in fourth quadrant and AZ position is in first quadrant
						PORTD &= ~(1<<DIR_AZ);                                                                                                  //Azimuth rotor direction set to LEFT 
						cAZ_RotorDir = 'L';                                                                                                     //Azimuth turn flag set to LEFT
					}
					else{
						PORTD |= (1<<DIR_AZ);                                                                                                   //Azimuth rotor direction set to RIGHT
						cAZ_RotorDir = 'R';                                                                                                     //Azimuth turn flag set to RIGHT
					}
				}
				else if(iAZ_Pulses < iAZ_PosPulses){																							//checks if the AZ pulses to execute are less than AZ position
					if(0 <= iAZ_Pulses && iAZ_Pulses <= 3999 && 11999 <= iAZ_PosPulses && iAZ_PosPulses <= 15999){                              //checks if the AZ pulses to execute are in first quadrant and AZ position is in fourth quadrant
						PORTD |= (1<<DIR_AZ);                                                                                                   //Azimuth rotor turning right
						cAZ_RotorDir = 'R';                                                                                                     //Azimuth turn flag set to RIGHT
					}
					else{
						PORTD &= ~(1<<DIR_AZ);                                                                                                  //Azimuth rotor direction set to LEFT
						cAZ_RotorDir = 'L';                                                                                                     //Azimuth turn flag set to LEFT
					}
				}
				PORTD |= (1<<AZ_output);																										//sets a HIGH level on AZ rotator pulses
				iAZ_PulseFlag = 1;																												//AZ current pulse flag
			}
			else{
				if(iAZ_PosPulses == 0 && iAZ_Pulses == 15999){																					//checks if AZ position is 0 degrees and new position is 360 360 degrees
					iAZ_PosPulses = 15999;																										//sets AZ position in 360 degrees
				}
				else{
					iAZ_PosPulses = 0;																											//sets AZ position in 0 degrees
				}
			}
		}
		if (iEL_PosPulses != iEL_Pulses){                                                                                                       //checks if EL rotor is not in the position
			PORTD &= ~(1<<EL_enable);																											//enables EL rotator (ENABLE = LOW)
			if (iEL_Pulses > iEL_PosPulses){                                                                                                    //checks if the EL pulses to execute are greater than EL position
				PORTD &= ~(1<<DIR_EL);                                                                                                          //Elevation rotor direction set to UP
				cEL_RotorDir = 'U';                                                                                                             //Elevation turn flag set to UP				}
			}
			else if(iEL_Pulses < iEL_PosPulses){                                                                                                //checks if the EL pulses to execute are less than EL position
				PORTD |= (1<<DIR_EL);                                                                                                           //Elevation rotor direction set to DOWN
				cEL_RotorDir = 'D';                                                                                                             //Elevation turn flag set to DOWN
			}
			PORTB |= (1<<EL_output);																											//sets a HIGH level on EL rotator pulses
			iEL_PulseFlag = 1;
		}
		if (iSpeedFlag){																														//checks speed (FOLLOW = SLOW - SET = HIGH)
			_delay_ms(T_ON_SET);
		}
		else{
			_delay_ms(T_ON_FOLLOW);
		}
		PORTD |= (1<<AZ_enable);																												//disables AZ rotator (DISABLE = HIGH)
		PORTD |= (1<<EL_enable);																												//disables EL rotator (DISABLE = HIGH)
		PORTD &= ~(1<<AZ_output);																												//sets a LOW level on AZ rotator pulses
		PORTB &= ~(1<<EL_output);																												//sets a LOW level on EL rotator pulses
		if (iSpeedFlag){																														//checks speed (FOLLOW = SLOW - SET = HIGH)
			_delay_ms(T_OFF_SET);
		}
		else{
			_delay_ms(T_OFF_FOLLOW);
		}
			
		if(iAZ_PulseFlag == 1){																													//checks if there is a current pulse (high level) on AZ rotor
			if(cAZ_RotorDir == 'R'){                                                                                                            //checks if AZ rotor is turning RIGHT
				if (iAZ_PosPulses == 15999){                                                                                                    //checks if AZ rotor has turned 360 degrees
					iAZ_PosPulses = 0;                                                                                                          //resets AZ_pos_pulses to 0 degrees
				}
				else{
					iAZ_PosPulses++;                                                                                                            //increases AZ_pos_pulses
				}
			}
			else if(cAZ_RotorDir == 'L'){																										//checks if AZ rotor is turning LEFT
				if (iAZ_PosPulses == 0){                                                                                                        //checks if AZ rotor has turned 360 degrees
					iAZ_PosPulses = 15999;                                                                                                      //resets AZ_pos_pulses to 360 degrees
				}
				else{
					  iAZ_PosPulses--;                                                                                                          //decreases AZ_pos_pulses
				}
			}
			iAZ_PulseFlag = 0;
		}
			
		if(iEL_PulseFlag == 1){																													//checks if there is a current pulse (high level) on EL rotor
			if(cEL_RotorDir == 'U'){                                                                                                            //checks if EL rotor is turning UP
				iEL_PosPulses++;                                                                                                                //increases EL_pos_pulses
			}
			else if(cEL_RotorDir == 'D'){                                                                                                       //checks if EL rotor is turning DOWN
				iEL_PosPulses--;                                                                                                                //increases EL_pos_pulses
			}
			iEL_PulseFlag = 0;
		}
	
	}
	else {
		iRotatorFlag = 1;																														//if rotors are in position, sets stop flag (STOP = HIGH)
	}    
}	