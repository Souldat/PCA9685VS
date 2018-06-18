#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <string>
#include <wiringPiI2C.h>


using namespace std;

 uint16_t prepareDshotPacket(const uint16_t _value)
{
	uint16_t packet = (_value << 1); //| (motor->requestTelemetry ? 1 : 0);

	//motor->requestTelemetry = false; // reset telemetry request to make sure it's triggered only once in a row

	// compute checksum
	int csum = 0;
	int csum_data = packet;

	for (int i = 0; i < 3; i++) 
	{
		csum ^= csum_data; // xor data by nibbles
		csum_data >>= 4;
	}

	csum &= 0xf;

	// append checksum
	packet = (packet << 4) | csum;

	return packet;
 }

 struct MeasureTime
 {
	 using precision = std::chrono::nanoseconds;
	 std::vector<std::chrono::steady_clock::time_point> times;
	 std::chrono::steady_clock::time_point oneLast;

	 double p()
	 {
		 //std::cout<< "Mark "<< times.size() / 2 << ": "<< std::chrono::duration_cast<precision>(times.back() - oneLast).count()<< std::endl;
		 return std::chrono::duration_cast<precision>(times.back() - oneLast).count();

		 m();
	 }

	 void m()
	 {
		 oneLast = times.back();
		 times.push_back(std::chrono::steady_clock::now());
	 }

	 double t()
	 {
		 m();
		 return p(); 		 
	 }

	 MeasureTime()
	 {
		 times.push_back(std::chrono::steady_clock::now());
	 }
 };

 struct clock
 {
	 typedef unsigned long long                 rep;
	 typedef std::ratio<1, 1400000000>       period; // My machine is 2.8 GHz
	 typedef std::chrono::duration<rep, period> duration;
	 typedef std::chrono::time_point<clock>     time_point;
	 static const bool is_steady = true;

	 static time_point now() noexcept
	 {
		 unsigned lo, hi;
		 asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
		 return time_point(duration(static_cast<rep>(hi) << 32 | lo));
	 }
 };

 int main(void)
 {	
	 int k = wiringPiI2CSetup(0x40);

	 cout << wiringPiI2CReadReg8(k,0x04) << endl;

	 

	 int m;
	 cin >> m;
	 
	 return 0;
 }
#pragma region OLD
 /* wiringPiSetup();
 pinMode(1, OUTPUT);

 while (true)
 {
 digitalWrite(1, 1);
 digitalWrite(1, 0);
 }*/

 //uint16_t _bitCheck;

 // while (true)
 // {
 //	 digitalWrite(1, 0);
 //	 delayMicroseconds(2);

 //	 p_send = _packet;	 

 //	 //Transmit Packet
 //	 for (int i = 0; i < 15; i++)
 //	 {
 //		 //_bitCheck = (p_send & 0x01);

 //		 digitalWrite(1, p_send & 0x01);

 //		 //delayMicroseconds(1);

 //		 digitalWrite(1, 0);

 //		 delayMicroseconds(1.67);

 //		 p_send = p_send >> 0x01;

 //		 
 //	 }

 //	 //break;
 //}

 /* MeasureTime m;

 int k = 0;

 while (true)
 {

 digitalWrite(1, 0);
 delayMicroseconds(2);

 p_send = _packet;

 for (int i = 0; i < 15; i++)
 {

 if (m.t() > 0.00000001)
 {
 if (k == 0)
 {
 digitalWrite(1, p_send & 0x01);
 p_send = p_send >> 0x01;
 k = 1;
 }
 else if (k == 1)
 {
 digitalWrite(1, 0);

 k = 0;
 }
 check = false;
 }
 }
 }*/

#pragma endregion

//int i;
////int delayTime = delayMicroseconds(10);
//
//wiringPiSetup();
//	
//pinMode(1, OUTPUT);

//for (;;)
//{
//	//printf("LED #0 on\n");
//	digitalWrite(1, 1);
//	//delay(delayTime);
//	delayMicroseconds(3);

//	//printf("LED #0 off\n\n");
//	digitalWrite(1, 0);
//	delayMicroseconds(3);
//	//delay(delayTime);	
//	
//}


// use the wPi column for pin identification in this program.
// In this case, wPi pin 0 is located at physical pin #11 (GND at pin #9)
// wPi pin 1 is located at physical pin #12 (GND at pin #6)
// note that these pin numbers are based on your hardware device.  I'm using a Raspberry Pi, version B, first generation.

//pi@raspberrypi:~ $ gpio readall  <--- Hint, Hint !!
// +-----+-----+---------+------+---+-Model B1-+---+------+---------+-----+-----+
// | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
// +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
// |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
// |   2 |   8 |   SDA.1 |  OUT | 0 |  3 || 4  |   |      | 5v      |     |     |
// |   3 |   9 |   SCL.1 |  OUT | 0 |  5 || 6  |   |      | 0v      |     |     |
// |   4 |   7 | GPIO. 7 |  OUT | 0 |  7 || 8  | 0 | OUT  | TxD     | 15  | 14  |
// |     |     |      0v |      |   |  9 || 10 | 1 | OUT  | RxD     | 16  | 15  |
// |  17 |   0 | GPIO. 0 |  OUT | 0 | 11 || 12 | 0 | OUT  | GPIO. 1 | 1   | 18  |
// |  27 |   2 | GPIO. 2 |  OUT | 1 | 13 || 14 |   |      | 0v      |     |     |
// |  22 |   3 | GPIO. 3 |  OUT | 0 | 15 || 16 | 0 | OUT  | GPIO. 4 | 4   | 23  |
// |     |     |    3.3v |      |   | 17 || 18 | 0 | OUT  | GPIO. 5 | 5   | 24  |
// |  10 |  12 |    MOSI |  OUT | 0 | 19 || 20 |   |      | 0v      |     |     |
// |   9 |  13 |    MISO |  OUT | 0 | 21 || 22 | 0 | OUT  | GPIO. 6 | 6   | 25  |
// |  11 |  14 |    SCLK |  OUT | 0 | 23 || 24 | 0 | OUT  | CE0     | 10  | 8   |
// |     |     |      0v |      |   | 25 || 26 | 0 | OUT  | CE1     | 11  | 7   |
// +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
// | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
// +-----+-----+---------+------+---+-Model B1-+---+------+---------+-----+-----+
//
