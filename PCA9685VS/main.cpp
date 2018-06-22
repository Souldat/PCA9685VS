#include <iostream>
#include <wiringPiI2C.h>
#include <cmath>
#include <unistd.h>

using namespace std;
const int fd = wiringPiI2CSetup(0x40);

//Write to single registers
int WriteReg8(int _reg, int _val)
{
	try 
	{
		cout << "Writing: " << _val << " to register: " << _reg << " :: ";
		if (wiringPiI2CWriteReg8(fd, _reg, _val) == -1)
		{
			cout << -1 << endl;
			return -1;
		}
		else
		{
			cout << 0 << endl;
			return 0;
		}
	}
	catch (const std::exception &exc)
	{
		cerr << exc.what();
		return -1;
	}	
}

//Read from single 8 bit register
int ReadReg8(int _reg)
{
	try 
	{
		int _k = wiringPiI2CReadReg8(fd, _reg);
		cout << "Read: " << _k << " from: " << _reg << endl;
	}
	catch (const std::exception &exc)
	{
		cerr << exc.what();
		return -1;
	}	
}

 void PCA9685Init()
 {
	 cout << endl << "Initializing PCA9685" << endl;
	 cout << "-----------------------------" << endl;
	 WriteReg8(0xFA, 0x00);
	 WriteReg8(0xFB, 0x00);
	 WriteReg8(0xFC, 0x00);
	 WriteReg8(0xFD, 0x00);
	 WriteReg8(0x01, 0x04);
	 WriteReg8(0x00, 0x01);
	 ReadReg8(0x00);	 
	 WriteReg8(0x00, 0x01);
	 cout << "-----------------------------" << endl;
	 cout << endl << endl;	 
 } 
 
 void SetPWM(int _onVal, int _offVal)
 {	 
	 //Set the time in a single pulse which the output should be HIGH
	 WriteReg8(0x06, _onVal & 0xFF);
	 WriteReg8(0x07, _onVal >> 8);

	 //Set the time in a single pulse which the output should be LOW
	 WriteReg8(0x08, _offVal & 0xFF);
	 WriteReg8(0x09, _offVal >> 8);
 } 

 void SetFrequency(float _freq)
 {
	 cout << "Setting frequency to:" << _freq << "Hz" << endl;
	 int scale = 25000000 / 4096;
	 scale /= _freq;
	 scale -= 1;
	 scale = floor(scale + .05);
	 cout << "Value after scaling to clock (25MHz):" << scale << endl;

	 WriteReg8(0x00, 0x11);
	 WriteReg8(0xFE, scale);
	 WriteReg8(0x00, 0x01);
	 usleep(500);
	 WriteReg8(0x00, 0x81);

	 cout << "Frequency set." << endl << endl;
 }

 int main(void)
 {	
	 //Initialize PCA9685
	 PCA9685Init();

	int input;

	while (true)
	{
		cout << "Enter Frequency: ";
		cin >> input;
		SetFrequency(input);

		cout << "Enter speed:";
		cin >> input;
		SetPWM(0, input);
	}

	 return 0;
 }