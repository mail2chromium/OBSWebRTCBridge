#include <obs-module.h>
#include <webrtc/api/peer_connection_interface.h>
#include <webrtc/sdk/include/api/video_frame.h>
#include <webrtc/sdk/include/api/video/i420_buffer.h>
#include <webrtc/sdk/include/api/audio/audio_frame.h>

class OBSWebRTCBridge {
 public:
  OBSWebRTCBridge() {
    // Initialize OBS API
    obs_startup("en-US", nullptr, nullptr);
    obs_load_all_modules();

    // Initialize WebRTC API
    rtc::InitializeSSL();
    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();

    // Create OBS source
    source_ = obs_source_create("source_name", "source_id", nullptr, nullptr);

    // Start capturing audio and video data from the source
    obs_source_set_audio_capture_callback(source_, AudioDataCallback, this);
    obs_source_set_video_capture_callback(source_, VideoDataCallback, this);
  }

  ~OBSWebRTCBridge() {
    // Clean up OBS and WebRTC API
    obs_source_destroy(source_);
    peer_connection_factory_->Release();
    rtc::CleanupSSL();
  }

  // Callback for audio data from the OBS source
  static void AudioDataCallback(void* data, obs_source_t* source, const struct audio_data* audio_data, bool muted) {
    OBSWebRTCBridge* bridge = static_cast<OBSWebRTCBridge*>(data);
    if (!muted) {
      // Create WebRTC audio frame and send it to WebRTC peer connection
      webrtc::AudioFrame audio_frame;
      audio_frame.sample_rate_hz_ = audio_data->samples_per_sec;
      audio_frame.num_channels_ = audio_data->channels;
      audio_frame.samples_per_channel_ = audio_data->frames;
      audio_frame.interleaved_samples_ = audio_data->data[0];
      // Send the audio frame to the WebRTC peer connection
      // ...
    }
  }

  // Callback for video data from the OBS source
  static void VideoDataCallback(void* data, obs_source_t* source, const struct video_data* video_data, bool muted) {
    OBSWebRTCBridge* bridge = static_cast<OBSWebRTCBridge*>(data);
    if (!muted) {
      // Create WebRTC video frame and send it to WebRTC peer connection
      webrtc::I420Buffer* buffer = webrtc::I420Buffer::Create(video_data->width, video_data->height);
      uint8_t* y_plane = buffer->MutableDataY();
      uint8_t* u_plane = buffer->MutableDataU();
      uint8_t* v_plane = buffer->MutableDataV();
      int y_stride = buffer->StrideY();
      int u_stride = buffer->StrideU();
      int v_stride = buffer->StrideV();

      // Copy video data to the I420 buffer
      for (int i = 0; i < video_data->height; i++) {
        memcpy(y_plane + i * y_stride, video_data->data[0] + i * video_data->linesize[0], video_data->width);
      }
      for (int i = 0; i < video_data->height / 2; i++) {
        memcpy(u_plane + i * u_stride, video_data->data[1] + i * video_data->linesize[1], video_data->width / 2);
        memcpy(v_plane + i * v_stride, video_data->data[2] + i * video_data->linesize[2], video_data->width / 2);
      }

      webrtc::VideoFrame video_frame(buffer, video_data->timestamp, video_data->duration, webrtc::kVideoRotation_0);
      // Send the video frame to the WebRTC peer connection
      // ...
    }
  }
}