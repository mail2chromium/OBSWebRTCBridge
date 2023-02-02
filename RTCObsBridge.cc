
/*Copy the RTCObsBridge.cc file -> Muhammad Usman Bashir -> 29-01-2023*/

#include <iostream>
#include <webrtc/api/peerconnectioninterface.h>
#include "OBS_API.h"

class RTCObsBridge {
 public:
    RTCObsBridge();
    void Start();
    void Stop();
 private:
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    obs_source_t* audio_source_;
    obs_source_t* video_source_;
};

RTCObsBridge::RTCObsBridge() {
    // Initialize WebRTC
    rtc::InitializeSSL();
    webrtc::PeerConnectionFactory::Initialize();

    // Create a new PeerConnectionFactory
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory();

    // Create a new PeerConnection
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    peer_connection_ = factory->CreatePeerConnection(config, nullptr, nullptr, nullptr);

    // Get OBS audio and video sources
    audio_source_ = obs_get_source_by_name("audio_source");
    video_source_ = obs_get_source_by_name("video_source");
}

void RTCObsBridge::Start() {
    // Create media streams
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = peer_connection_->CreateLocalMediaStream("stream");
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track = factory->CreateAudioTrack("audio", factory->CreateAudioSource(cricket::AudioOptions()));
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track = factory->CreateVideoTrack("video", factory->CreateVideoSource(cricket::VideoOptions()));

    // Add audio and video tracks to the media stream
    stream->AddTrack(audio_track);
    stream->AddTrack(video_track);

    // Add the media stream to the PeerConnection
    if (!peer_connection_->AddStream(stream)) {
        std::cerr << "Failed to add stream to PeerConnection" << std::endl;
        return;
    }

    // Create an RTCOfferDescription
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
    peer_connection_->CreateOffer(options);

    // Set up callbacks for OBS audio and video data
    obs_source_output_audio audio_data_callback = [&](void* data, size_t size) {
        // Pass audio data to WebRTC pipeline
        audio_track->AddSamples(data, size);
    };
    obs_source_output_video video_data_callback = [&](gs_texture* texture) {
        // Pass video data to WebRTC pipeline
        video_track->AddTexture(texture);
    };

    // Start the OBS audio and video sources
    obs_source_add_output(video_source_, video_data_callback, nullptr);
    obs_source_start(audio_source_);
    obs_source_start(video_source_);
}

void RTCObsBridge::Stop() {
    // Stop the OBS audio and video sources
    obs_source_remove_output(audio_source_, audio_data_callback, nullptr);
    obs_source_remove_output(video_source_, video_data_callback, nullptr);
    obs_source_stop(audio_source_);
    obs_source_stop(video_source_);

    // Close the PeerConnection
    peer_connection_->Close();

    // Release the PeerConnectionFactory
    webrtc::PeerConnectionFactory::Release();

    // Deinitialize SSL
    rtc::CleanupSSL();
}