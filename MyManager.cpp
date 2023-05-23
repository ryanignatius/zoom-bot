#include "MyManager.h"

MyManager::MyManager() 
{
}

MyManager::~MyManager(){
}

bool MyManager::meetingReady()
{
	return meetingID > 0;
}

void MyManager::setMeetingId(uint32_t m_id)
{
  	meetingID = m_id;
    wchar_t s[64];
    wsprintf(s, (L"Meeting ID: %d\n"), m_id);
}

void MyManager::start()
{
    target.open("audio.pcm", ios::out || ios::binary || ios::app);
    if (!target) {
        target.open("audio.pcm", ios::trunc);
        target.close();
        target.open("audio.pcm", ios::out || ios::binary || ios::app);
    }
}

void MyManager::close()
{
    target.close();
}

void MyManager::send(AudioRawData* data)
{
    wchar_t s[256];
    wsprintf(s, (L"Sent Multiple Channel: %d. Sample: %d\n"), data->GetChannelNum(), data->GetSampleRate());
    OutputDebugStringW(s);
    target.write(data->GetBuffer(), data->GetBufferLen());
    target.flush();
}
