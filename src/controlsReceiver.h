#pragma once
#include <functional>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

class ControlsReceiver
{
private:
  int listeningSocket = 0;
  int clientConnectionSocket = 0;
  uint8_t rfcommChannel = 11;
  sdp_session_t* sdpSession = nullptr;
  std::function<void(char)> onCommandCallback;
  void receiveMessages();
public:
  ControlsReceiver(std::function<void(char)> onCommandCallback);
  ~ControlsReceiver();
  void registerService();
  void startListening();
};
