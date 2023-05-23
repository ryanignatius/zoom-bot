#include "stdafx.h"
#include <iostream>
#include "raw_data_manager.h"

GetRawData::GetRawData()
{
	recordingController = NULL;
	livestreamController = NULL;
	zoomRenderer = NULL;
	rawAudioHelper = NULL;
	isInited = false;
	canGetRawData = false;
	isGetRawData = false;
	meetingID = -1;
}
GetRawData::~GetRawData()
{
	OutputDebugStringW(L"Destroy Manager\n");
	stopRawData();
	recordingController = NULL;
	livestreamController = NULL;
	zoomRenderer = NULL;
	rawAudioHelper = NULL;
	isInited = false;
	canGetRawData = false;
	isGetRawData = false;
	meetingID = -1;
}

void GetRawData::initController()
{
	OutputDebugStringW(_T("INIT RAW\n"));
	if (isInited)
	{
		return;
	}

	recordingController = SDKInterfaceWrap::GetInst().GetMeetingRecordingController();
	recordingController->SetEvent(this);
	ZOOM_SDK_NAMESPACE::IMeetingParticipantsController* participantCtrl =  SDKInterfaceWrap::GetInst().GetMeetingParticipantsController();

	livestreamController = SDKInterfaceWrap::GetInst().GetMeetingLivestreamController();
	livestreamController->SetEvent(this);
	
	auto meetingTopic = SDKInterfaceWrap::GetInst().GetMeetingService()->GetMeetingInfo()->GetMeetingTopic();
	wstring ws(meetingTopic);
	myselfID = participantCtrl->GetMySelfUser()->GetUserID();
	
	isInited = true;
}

void GetRawData::startRawData()
{
	OutputDebugStringW(_T("START RAW\n"));
	
	wchar_t s[256];
	wsprintf(s, _T("Myself ID IS %d %d\n"), myselfID, meetingID);
	OutputDebugStringW(s);

	canGetRawData = recordingController->IsSupportLocalRecording(myselfID);
	myManager.start();

	if (!canGetRawData)
	{
		OutputDebugStringW(_T("Can't Get Record\n"));
		ZOOM_SDK_NAMESPACE::SDKError err = recordingController->RequestLocalRecordingPrivilege();

		cout << "result request " << err << endl;
	}
	else {
		recordingController->StartRawRecording();
	}
}

void GetRawData::stopRawData()
{
	myManager.close();
	if (recordingController && isGetRawData) {
		recordingController->StopRawRecording();
		recordingController->SetEvent(NULL);
	}
}
void GetRawData::startLiveStreamRaw()
{
	OutputDebugStringW(_T("Start live stream raw\n"));

	if (!canGetRawData)
	{
		OutputDebugStringW(_T("Can't Get Record\n"));
		ZOOM_SDK_NAMESPACE::SDKError err = livestreamController->RequestRawLiveStream(L"https://zoom.us");
	}
	else
	{
		livestreamController->StartRawLiveStream(L"https://zoom.us");
	}


}
void GetRawData::stopLiveStreamRaw()
{
	if (livestreamController && isGetRawData)
	{
		livestreamController->StopRawLiveStream();
		livestreamController->SetEvent(NULL);
	}
}

void GetRawData::onRecording2MP4Done(bool bsuccess, int iResult, const wchar_t* szPath){}
void GetRawData::onRecording2MP4Processing(int iPercentage){}
void GetRawData::onRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status){
	if (ZOOM_SDK_NAMESPACE::Recording_Start == status) {
		isGetRawData = true;
		OutputDebugStringW(_T("Recording Started\n"));
		if (NULL == zoomRenderer)
		{
			ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::createRenderer(&zoomRenderer, this);

			if (ZOOM_SDK_NAMESPACE::SDKERR_SUCCESS == err) 
				OutputDebugStringW(_T("SUCCESS CREATE RENDERER\n"));
			else
				OutputDebugStringW(_T("FAIL CREATE RENDERER\n"));
		}

		if (NULL == rawAudioHelper)
		{
			rawAudioHelper = ZOOM_SDK_NAMESPACE::GetAudioRawdataHelper();
			rawAudioHelper->subscribe(this);
		}
	}
	else {
		if (ZOOM_SDK_NAMESPACE::Recording_Stop == status) {
			OutputDebugStringW(L"Stop Recording\n");
			stopRawData();
		}
		OutputDebugStringW(_T("Recording Paused/Stopped\n"));
		isGetRawData = false;
		if (NULL != zoomRenderer)
		{
			ZOOM_SDK_NAMESPACE::SDKError err = ZOOM_SDK_NAMESPACE::destroyRenderer(zoomRenderer);
			zoomRenderer = NULL;
		}

		if (NULL != rawAudioHelper)
		{
			rawAudioHelper->unSubscribe();
		}
	}
}
void GetRawData::onCloudRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status){}
void GetRawData::onRecordPriviligeChanged(bool bCanRec)
{
	canGetRawData = bCanRec;
	if (canGetRawData) {
		OutputDebugStringW(_T("Record Changed\n"));
		recordingController->StartRawRecording();
	}
	else {
		OutputDebugStringW(_T("Record Not Changed\n"));
		recordingController->StopRawRecording();
	}
}
void GetRawData::onLocalRecordingPrivilegeRequestStatus(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus status){
	if (ZOOM_SDK_NAMESPACE::RequestLocalRecording_Granted == status) {
		OutputDebugStringW(_T("Request Granted\n"));
	}
	else {
		OutputDebugStringW(_T("Request NOT Granted\n"));
	}
}
void GetRawData::onCustomizedLocalRecordingSourceNotification(ZOOM_SDK_NAMESPACE::ICustomizedLocalRecordingLayoutHelper* layout_helper){}
void GetRawData::onLocalRecordingPrivilegeRequested(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler* handler){}

void GetRawData::onLiveStreamStatusChange(ZOOM_SDK_NAMESPACE::LiveStreamStatus status)
{
	if (ZOOM_SDK_NAMESPACE::LiveStreamStatus_InProgress == status)
	{
		OutputDebugStringW(_T("Live stream In Progress\n"));
		isGetRawData = true;

		if (NULL == rawAudioHelper)
		{
			rawAudioHelper = ZOOM_SDK_NAMESPACE::GetAudioRawdataHelper();
			rawAudioHelper->subscribe(this);
		}
	}
	else {
		OutputDebugStringW(_T("Live stream not progressing\n"));
		isGetRawData = false;

		if (NULL != rawAudioHelper) {
			rawAudioHelper->unSubscribe();
		}
	}
}
void GetRawData::onRawLiveStreamPrivilegeChanged(bool bHasPrivilege) {
	canGetRawData = bHasPrivilege;
	if (canGetRawData)
	{
		OutputDebugStringW(_T("Have live stream privilege\n"));
		livestreamController->StartRawLiveStream(L"https:://zoom.us");
	}
	else {
		OutputDebugStringW(_T("Not Have live stream privilege\n"));
		livestreamController->StopRawLiveStream();
	}
}

void GetRawData::onRendererBeDestroyed(){}
void GetRawData::onRawDataFrameReceived(YUVRawDataI420* data){}
void GetRawData::onRawDataStatusChanged(RawDataStatus status){}

void GetRawData::onMixedAudioRawDataReceived(AudioRawData* data_) {
	//OutputDebugStringW(L"Sent Multiple\n");
	myManager.send(data_);
}
void GetRawData::onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) {
	//OutputDebugStringW(_T("OneWayAudioData\n"));
}
