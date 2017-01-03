// This program  enables two Log software to talk with one Radio.
// It will enable one log to call the radio for some time and than will switch to the second Log software. 
// If during the period that the Radio is controled a Log Softwer, if the second log software 
// have sent an important CAT command, the program will send this commmand
// before letting the second software to talk to the radio for its turn. 
// In this version i will test a concept in which the ARDUINO is listening to the two log software at the same time
// in this version i am adding the ability to listen to the CAT commands that MixW is sending
// to change the Radio mode to digital mode for BPSK and RTTY

#define LogSelect_PIN 5 // This pin will control the SPDT chip to whitch log software to listen
#define LineToRadioOnOff_PIN 6	//This pin will contril an SPDT chip that will swtich On or OFF the communication
								// line to the radio

byte CatByteSW2;
byte CatByteSW1;
byte CatFromRadio;

byte LogSW; // marker. will be 1 for SW2 and 2 for SW1

byte CatCommandSW2[10][20]; // Array that holds the important CAT commmands from Mixw
byte CatCommandSW1[10][20]; // Array that holds the important CAT commmands from SW1

byte nSW2Commands = 0; // number of important CAT commands in the CatCommandSW2[][] array
byte nSW1Commands = 0; // number of important CAT commands in the CatCommandSW1[][] array

byte iSW2 = 0;
byte iSW1 = 0;

boolean FullAnswerToSW2 = false;
boolean FullAnswerToSW1 = false;

boolean ThereIsSW2 = false;
boolean ThereIsSW1 = false;

long t0, LastTimeSawSW2, LastTimeSawSW1;  // will be used as time holding parameters
int SniffTime; // How many milli seconds a SW will be sniffing before letting the othe software to sniff.

//======================================================= Setup ==========================================================

void setup() {

	Serial.begin(115200);
	Serial1.begin(19200); // Will listen to Log SW 1
	Serial2.begin(19200); // Will listen to Log SW 2
	Serial3.begin(19200); // sniffing port for develop stage

	pinMode(LogSelect_PIN, OUTPUT);   // This pin controls an electronic switch that control which LOG SW is connected to 
	// the line that goes to the Radio
	pinMode(LineToRadioOnOff_PIN, OUTPUT);

	digitalWrite(LogSelect_PIN, 0);	//  1  connect the Radio to LOG SW that is sniffed on Serial 1 (SW2 during dev)
									//  0  connect the Radio to LOG SW that is sniffed on Serial 2 (SW1 during dev)
	LogSW = 1; // Flag that we are starting with SW2;
	digitalWrite(LineToRadioOnOff_PIN, 1);  // When 1 the line to the radio is connected to a Log Softwares. 
									    	// when 0 it is connected to Arduino Serial3

	t0 = millis();
	LastTimeSawSW2 = millis();
	LastTimeSawSW1 = millis();

	
	SniffTime = 1200; // how much time the program will let each log software to talk to the radio
	
	delay(1000);
}
//======================================================= Main Loop ======================================================
void loop()
{
	//do // This is a test loop that conect each one of the Log SW for 1 second to the radio.
	//{// it is good for testing the electronics.
	//	digitalWrite(LogSelect_PIN, 1); // Conecting SW2 to the Radio. (This will disconnect SW1 from the Radio)
	//	digitalWrite(LineToRadioOnOff_PIN, 1); // When 0 the line to the radio is disconnected. when 1 it is connected.
	//	delay(1000);
	//	digitalWrite(LineToRadioOnOff_PIN, 0); // When 0 the line to the radio is disconnected. when 1 it is connected.
	//	digitalWrite(LogSelect_PIN, 0); // Conecting SW2 to the Radio. (This will disconnect SW1 from the Radio)
	//	digitalWrite(LineToRadioOnOff_PIN, 1); // When 0 the line to the radio is disconnected. when 1 it is connected.
	//	delay(1000);
	//	digitalWrite(LineToRadioOnOff_PIN, 0); // When 0 the line to the radio is disconnected. when 1 it is connected.
	//} while (true);


//------------------ reading the data on the line that serial 1 is listening on -----------------

	if (Serial1.available() > 0) // Read the bytes SW2 is sending and the Radio is answering
	{
		CatByteSW2 = Serial1.read();
		ThereIsSW2 = true;
		LastTimeSawSW2 = millis();
		//Serial.println("There is SW2");
		CatCommandSW2[nSW2Commands][iSW2] = CatByteSW2;
		iSW2++;
		if (CatByteSW2 == 253) // the end byte of CAT command may have been sent from the log software to the Radio
		{						// or that have been sent by the radio to the log software as an answer.
			//for (int i = 0; i < iSW2; i++)
			//{
			//	Serial.print(CatCommandSW2[nSW2Commands][i]); Serial.print("M ");
			//}
			//Serial.println();

			if (IsImportantCommad(CatCommandSW2[nSW2Commands][4]) == true && LogSW == 2)
			{

				//for (int i = 0; i < iSW2; i++)
				//{
				//	Serial.print(CatCommandSW2[nSW2Commands][i]); Serial.print("E ");
				//}
				//Serial.println();


				CatCommandSW2[nSW2Commands][19] = iSW2;
				nSW2Commands++;
				//Serial.print("There are "); Serial.print(nSW2Commands); Serial.print(" important Mixw commands");

			}
			else if (CatCommandSW2[nSW2Commands][2] == 224) // Check if the CAT command is an answer from the Radio to the Log SW
			{
				FullAnswerToSW2 = true; //Serial.println("Full answer to SW2");
			}
			else if (CatCommandSW2[nSW2Commands][2] != 224)
			{
				FullAnswerToSW2 = false;
			}

			iSW2 = 0;
		}
	}
	else if (millis() - LastTimeSawSW2>2000)
	{
		ThereIsSW2 = false;
		FullAnswerToSW2 = false;
		//Serial.println("No SW2");
	}

//------------------ reading the data on the line that serial 2 is listening on -----------------

	if (Serial2.available() > 0) //Read the bytes SW1 is sending and the Radio is answering
	{
		CatByteSW1 = Serial2.read();
		ThereIsSW1 = true;
		LastTimeSawSW1 = millis();
		CatCommandSW1[nSW1Commands][iSW1] = CatByteSW1;
		iSW1++;
		if (CatByteSW1 == 253)
		{
			
			//for (int j = 0; j < iSW1; j++)
			//{
			//	Serial.print(CatCommandSW1[nSW1Commands][j]); Serial.print(" ");
			//}
			//Serial.println();
			

			if (IsImportantCommad(CatCommandSW1[nSW1Commands][4]) == true && LogSW == 1)
			{
				Serial.println("SW1 Manual command");
				CatCommandSW1[nSW1Commands][19] = iSW1;
				nSW1Commands++;
				Serial.print("There are "); Serial.print(nSW1Commands); Serial.println(" important SW1 commands");
			}
			else if (CatCommandSW1[nSW1Commands][2] == 224) // Check if the CAT command is an answer from the Radio to the Log SW
			{
				FullAnswerToSW1 = true; //Serial.println("Full answer to SW1");
			}
			else if (CatCommandSW1[nSW1Commands][2] != 224)
			{
				FullAnswerToSW1 = false;
			}
			iSW1 = 0;
		}
	}
	else if (millis() - LastTimeSawSW1>2000)
	{
		ThereIsSW1 = false;
		FullAnswerToSW1 = false;
		//Serial.println("No SW1");
	}

//-------------------------- checing that SW1 and SW2 are present --------------------------------

	if (ThereIsSW1 == true && ThereIsSW2 == false)
	{
		digitalWrite(LogSelect_PIN, 0);
	}
	else if (ThereIsSW2 == true && ThereIsSW1 == false)
	{
		digitalWrite(LogSelect_PIN, 1);
	}

	//delay(5);
	//Serial.println(millis());
	//Serial.print("ThereIsSW2= "); Serial.print(ThereIsSW2);
	//Serial.print("  FullAnswerToSW2= "); Serial.println(FullAnswerToSW2);

	//	Serial.print("ThereIsSW1= "); Serial.print(ThereIsSW1);
	//	Serial.print(" FullAnswerToSW1= "); Serial.println(FullAnswerToSW1);

//----------------------------------------------------------------------------------------------------------------------

	//Serial.print("Loop 2 is OK");
	//if (LogSW == 1 && ((millis() - t0) > 1000) && FullAnswerToSW2==true && ThereIsSW1==true) // if SW2 is talking to the Radio at least 1000ms
	if (LogSW == 1 && ((millis() - t0) > SniffTime) && ThereIsSW1 == true) // if SW2 is talking to the 
																			// Radio at least 1000ms
	{
		if (nSW1Commands > 0) // if there are important SW1 CAT commands that need to be sent to the Radio
		{
			digitalWrite(LineToRadioOnOff_PIN, 0); // Connect the Radio to Serial3
			delay(1);
			
			//ClearRadioBuffer();
			//delay(400);
			//Serial.println("Sending SW1 CAT from Serial3");
	
				for (byte i = 0; i < nSW1Commands; i++)
				{
					for (byte j = 0; j < CatCommandSW1[i][19]; j++)
					{
						Serial.print(CatCommandSW1[i][j]); Serial.print("-");
						Serial3.write(CatCommandSW1[i][j]);
						delay(1);
					}
					//Serial.println("-------------");
					delay(100);
					Serial.println();
				}

			nSW1Commands = 0;
			delay(100);
			digitalWrite(LineToRadioOnOff_PIN, 1); // Connect the Radio to Log Softwares line
		}
		
		digitalWrite(LogSelect_PIN, 0); // Connect SW1 to the line that goes to the Radio
		//ClearRadioBuffer();
		//Serial.println("Connected SW1");
		//digitalWrite(LineToRadioOnOff_PIN, 1); // Connect the Radio to Log Softwares line
		//Serial.println("Connect the Radio");
		LogSW = 2; // A flag telling us that SW1 is the log software that is talking to the Radio from now.
		//Serial.println("SW1 is talking to the Radio now");
		t0 = millis();
	}



	//----------------------------------------------------------------------------------------------------------------------
	//Serial.print("Loop 3 is OK");
	//if (LogSW == 2 && ((millis() - t0)> 1000) && FullAnswerToSW1==true && ThereIsSW2==true) // if SW1 is talking to the Radio at least 1000ms
	if (LogSW == 2 && ((millis() - t0)> SniffTime) && ThereIsSW2 == true) // if Log
	{
		if (nSW2Commands > 0) // if there are important SW2 CAT commands that need to be sent to the Radio
		{
			digitalWrite(LineToRadioOnOff_PIN, 0); // Connect the Radio to Serial3
			//ClearRadioBuffer();
			//delay(400);
			//Serial.println("Sending SW2 CAT from Serial3");
			for (byte i = 0; i < nSW2Commands; i++)
			{
				for (byte j = 0; j < CatCommandSW2[i][19]; j++)
				{
					Serial.print(CatCommandSW2[i][j]); Serial.print("+");
					Serial3.write(CatCommandSW2[i][j]);
				}
				delay(10);
				//Serial.println("+++++++++++++");
			}
			nSW2Commands = 0;
			delay(100);
			digitalWrite(LineToRadioOnOff_PIN, 1); // Connect the Radio to Log Softwares line
		}
		//ClearRadioBuffer();
		digitalWrite(LogSelect_PIN, 1); // Connect SW2 to the line that goes to the Radio
		//Serial.println("Connected SW2");
		//digitalWrite(LineToRadioOnOff_PIN, 1); // Connect the Radio to Log Software
		//Serial.println("Connect the Radio");
		LogSW = 1; // A flag telling us that SW2 is the log software that is talking to the Radio.
		//Serial.println("SW2 is talking to the Radio now");
		t0 = millis();
	}

}

boolean IsImportantCommad(byte cmd) // This function checks if the CAT command is an important one
{
	//Serial.print("Loop 5 is OK");
	if (cmd == 0 || cmd == 1 || cmd == 5 || cmd == 6 || cmd == 7 || cmd==26 )
	{
		//Serial.print("Important CAT cmd= "); Serial.println(cmd);
		return true;
	}
	else
	{
		return false;
	}
}


void ClearRadioBuffer()
{
	Serial.println("This is the Radio buffer ===============================");
	delay(100);
	long tt = millis();
	do
	{
		if (Serial3.available() > 0)
		{
			CatFromRadio = Serial3.read();
			Serial.print(CatFromRadio); Serial.print("* ");
		}

	} while (((millis() - tt)<100));
	//Serial.println();
	//Serial.println(millis()-tt);
	Serial.println("End of Radio buffer ===============================");
	delay(100);
}
