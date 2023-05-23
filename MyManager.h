#pragma once
#include <Windows.h>
#include "zoom_sdk_raw_data_def.h"
#include <iostream>
#include <fstream>

using namespace std;

class MyManager 
{
public:
  	MyManager();
	  ~MyManager();
    virtual bool meetingReady();
    virtual void setMeetingId(uint32_t meetingID);
    virtual void start();
    virtual void close();
    virtual void send(AudioRawData* rawData);

private:
    ofstream target;
};
