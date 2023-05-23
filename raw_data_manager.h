#pragma once
#include "sdk_util.h"
#include "MyManager.h"
#include "rawdata/rawdata_renderer_interface.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "rawdata/zoom_rawdata_api.h"
#include "zoom_sdk_raw_data_def.h"

class GetRawData: public ZOOM_SDK_NAMESPACE::IMeetingRecordingCtrlEvent
	,public ZOOM_SDK_NAMESPACE::IMeetingLiveStreamCtrlEvent
	,public ZOOM_SDK_NAMESPACE::IZoomSDKRendererDelegate
	,public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate
{
public:
	GetRawData();
	virtual ~GetRawData();
	virtual void initController();
	virtual void startRawData();
	virtual void stopRawData();
	virtual void startLiveStreamRaw();
	virtual void stopLiveStreamRaw();

	//MeetingRecordingCtrlEvent
	virtual void onRecording2MP4Done(bool bsuccess, int iResult, const wchar_t* szPath);
	virtual void onRecording2MP4Processing(int iPercentage);
	virtual void onRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status);
	virtual void onCloudRecordingStatus(ZOOM_SDK_NAMESPACE::RecordingStatus status);
	virtual void onRecordPriviligeChanged(bool bCanRec);
	virtual void onLocalRecordingPrivilegeRequestStatus(ZOOM_SDK_NAMESPACE::RequestLocalRecordingStatus status);
	virtual void onCustomizedLocalRecordingSourceNotification(ZOOM_SDK_NAMESPACE::ICustomizedLocalRecordingLayoutHelper* layout_helper);
	virtual void onLocalRecordingPrivilegeRequested(ZOOM_SDK_NAMESPACE::IRequestLocalRecordingPrivilegeHandler* handler);

	//MeetingLiveStreamCtrlEvent
	virtual void onLiveStreamStatusChange(ZOOM_SDK_NAMESPACE::LiveStreamStatus status);
	virtual void onRawLiveStreamPrivilegeChanged(bool bHasPrivilege);
	virtual void onRawLiveStreamPrivilegeRequestTimeout() {}
	virtual void onUserRawLiveStreamPrivilegeChanged(unsigned int userid, bool bHasPrivilege){}
	virtual void onRawLiveStreamPrivilegeRequested(ZOOM_SDK_NAMESPACE::IRequestRawLiveStreamPrivilegeHandler* handler) {}
	virtual void onUserRawLiveStreamingStatusChanged(ZOOM_SDK_NAMESPACE::IList< ZOOM_SDK_NAMESPACE::RawLiveStreamInfo >* liveStreamList){}

	//RenderDelegate
	virtual void onRendererBeDestroyed();
	virtual void onRawDataFrameReceived(YUVRawDataI420* data);
	virtual void onRawDataStatusChanged(RawDataStatus status);

	//AudioDelegate
	virtual void onMixedAudioRawDataReceived(AudioRawData* data_);
	virtual void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id);

protected:
	ZOOM_SDK_NAMESPACE::IMeetingRecordingController* recordingController;
	ZOOM_SDK_NAMESPACE::IMeetingLiveStreamController* livestreamController;
	ZOOM_SDK_NAMESPACE::IZoomSDKRenderer* zoomRenderer;
	ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataHelper* rawAudioHelper;
	MyManager myManager;
	uint32_t meetingID;
	bool isInited;
	bool canGetRawData;
	bool isGetRawData;
	unsigned int myselfID;
};
