/*-
 * Copyright (c) <2010,2013>, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#if defined __baremetal__
extern "C" void kprintf(...);
#elif defined __linux__
#include <math.h>
extern "C" void printf(...);
#endif

const float PI = 3.14159265358979f;
const float PI2 = 6.28318530717959f;
extern "C" long int getticks(void);
float sinus(float x) {
    float y;
    y=x*x;
    y=((((2.136548709317364E-6f*y+-0.0001924245224273794f)*y+0.008307513519274192f)*y+-0.166620906578438f)*y+0.999977239959734f)*x;
    return (y);
}

inline float _fabsf(float x) { return (x < 0) ? (-x):x; }
void memset(void *ptr, unsigned char s, int size)
{
for (int i = 0; i < size; i++) *((unsigned char*)ptr + size) = s;
}
class MyCtrl;

class DriverObj {
  friend class MyCtrl;
  public:
    DriverObj()
    {
      memset (this, 0, sizeof(DriverObj));
      time = getticks();
      return;
    }
    DriverObj(float min, float max, float i, float p, float d, float speed, float position) : min(min), max(max), kp(p), ki(i), kd(d), speed(speed), position(position), last_position(position)
    {
      memset (this, 0, sizeof(DriverObj));
      time = getticks();
      return;
    }

    float pid_update(float set)
    {
  float tmp, e, p, ret, tmpi;
	e = this->e;
	this->e = set - speed;
	tmpi = this->i + this->e;

        ret = kp + this->e + ki * tmpi + kd*(e - this->e);
	if ((ret < max) && (ret > min)) {
		i = tmpi;
	} else if (ret > max) {
		ret = max;
	} else if (ret < min) {
		ret = min;
	}

	return ret;
    };

    float speed;
    long long int time;
    float last_position;
    float position;

  private:
    float min;
    float max;
    float e;
    float i;
    float kp;
    float ki;
    float kd;
};

class MyCtrl
{
  public:
    MyCtrl();
    ~MyCtrl(void);

  void ctrl(void);

  private:
    const static int N_DRIVES = 16;
    DriverObj myDrives[N_DRIVES];
};


MyCtrl::MyCtrl() {
    memset(this, 0, sizeof(MyCtrl));
    for (int i = 0; i <  N_DRIVES; i++)
    {
      myDrives[i].min = -100.0f;
      myDrives[i].max = 100.0f;
      myDrives[i].kp = 0.01f;
      myDrives[i].ki = 0.05f;
      myDrives[i].kd = 0.00001f;
      myDrives[i].speed = 10.0f + (i % 10);
      myDrives[i].position = 20.0f;
      myDrives[i].last_position = 20.0f;
    }

}

MyCtrl::~MyCtrl()
{
    for (int i = 0; i <  N_DRIVES; i++)
    {
	//delete myDrives[0];
    }
}

// kinda Singleton
static MyCtrl myCtrl;

void MyCtrl::ctrl (void)
{
    unsigned long delta_us, cur_us;
    for(int i = 0; i < N_DRIVES; i++)
    {
	cur_us = getticks();
	delta_us = ((unsigned int)(cur_us - myDrives[i].time))/1000;
	myDrives[i].time = cur_us;
#if defined __baremetal__
// In the "real" baremetal mode, do a PCIe read (not for the sake of getting some data, just to test the jitter)
//#define AHCI_BAR2 0x5263b000
//#define AHCI_BAR2 0xB1240000
//   volatile unsigned char tmp = *((volatile unsigned char *) (SOC_BAR2 + 0x014));
#endif
	// read from fake data

	// calculate

	myDrives[i].last_position = myDrives[i].position;
	myDrives[i].position = myDrives[i].position + delta_us*myDrives[i].speed * 0.00003f / sinus(myDrives[i].last_position) ;

	float speed = (myDrives[i].position - myDrives[i].last_position)*(1000*60/8000)/delta_us;// 8000 RPM :)
	float change = myDrives[i].pid_update(speed + 2.0f);

	myDrives[i].speed += change;
#if defined __baremetal__
	// In the "read" baremetal mode, do a PCIe write (not for the sake of control, just to test the jitter)
	// write
//   *((volatile unsigned char *) (SOC_BAR2 + 0x014)) = 0x1;
#endif
    }

}


unsigned int minmax[2];
unsigned int test_ctrl()
{
    unsigned int delta_tsc;
    unsigned long long int start_tsc, stop_tsc; 

    start_tsc = getticks();
    myCtrl.ctrl();
    delta_tsc = getticks() - start_tsc;
    if ( delta_tsc > minmax[1] )
    {
        minmax[1] = delta_tsc;
    }
    if ( delta_tsc < minmax[0] )
    {
        minmax[0] = delta_tsc;
    }
    return delta_tsc;
}
extern "C" {
unsigned int test_ctrl2()
{
    return test_ctrl();
}
}
