# OBSWebRTCBridge
OBS WebRTC Bridge will create a new `PeerConnectionFactory`, creates a new `PeerConnection`, gets the OBS `audio` and `video sources`.

-----

## Basic Implementations:


 * The above code stacks define a class RTCObsBridge that has two main functions: `Start()` and `Stop()`. 
 * The Start() function  
    - Creates a new `PeerConnectionFactory`, 
    - Creates a new `PeerConnection`, 
    - Gets the `OBS` audio and video sources, 
    - Creates `media streams`, 
    - Adds the `audio` and `video` tracks to the media stream, 
    - Adds the media stream to the PeerConnection, 
    - Creates an RTCOfferDescription and 
    - Sets up `callbacks` for **OBS audio and video data** and start the OBS audio and video sources. 
 * The Stop() function stops the OBS audio and video sources, closes the PeerConnection and releases the PeerConnectionFactory.

> This is a simplified example, it is highly recommended to use a signalling protocol such as SDP to establish the connection between OBS and the WebRTC endpoint.
> Please keep in mind that this is a simplified example and there may be additional error handling and other code that would be needed for a production implementation.

-----

## Basic Understanding:

I can give you an idea of how you could proceed with integrating the WebRTC code into OBS Studio;

- You would need to include the OBS Studio headers in your program, in order to use its APIs and callbacks.
- You would need to create an OBS output module by implementing the `obs_output_info` **struct** and its required functions, such as `obs_output_create`, obs_output_start, `obs_output_stop`, and `obs_output_destroy`.
- In the `obs_output_create` function, you would need to create an instance of the RTCObsBridge class, and initialize it.
- In the `obs_output_start` function, you would need to call the Start method of the RTCObsBridge instance, to start the WebRTC connection and the OBS audio and video sources.
- In the `obs_output_stop` function, you would need to call the Stop method of the RTCObsBridge instance, to stop the WebRTC connection and the OBS audio and video sources.
- In the obs_output_destroy function, you would need to destroy the RTCObsBridge instance.
- You would need to handle the WebRTC signaling, to establish the connection with the remote peer, and exchange the necessary information to start the video and audio streaming.
- You would need to register the output module in the OBS Studio's plugin system, so that it can be used in the user interface.
- It is important to note that this is just a high level idea and it will require more effort and knowledge to complete the integration. Also, it's depend on the specific use case and requirements you have.

