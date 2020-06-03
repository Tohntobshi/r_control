#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "controlsReceiver.h"
#include "constants.h"

bdaddr_t BDADDR_ANY_VAL = {{0, 0, 0, 0, 0, 0}};
bdaddr_t BDADDR_ALL_VAL =  {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
bdaddr_t BDADDR_LOCAL_VAL = {{0, 0, 0, 0xff, 0xff, 0xff}};

ControlsReceiver::ControlsReceiver(std::function<void(char)> onCommand)
: onCommandCallback(onCommand)
{

}

void ControlsReceiver::registerService()
{
    const char *service_name = "tohntobshi's robot";
    const char *service_dsc = "robot remote control";
    const char *service_prov = "tohntobshi";

    uint8_t service_uuid_int[] = { 0x84, 0x8d, 0x82, 0x8b, 0xc4, 0x86, 0x44, 0xdf, 0x83, 0xfa, 0x54, 0x13, 0xb0, 0x61, 0x46, 0xe0 };
    uuid_t svc_uuid;
    sdp_uuid128_create( &svc_uuid, &service_uuid_int );

    sdp_record_t record = { 0 };

    sdp_set_service_id( &record, svc_uuid );
  
    sdp_list_t* svc_class_list = sdp_list_append( 0, &svc_uuid );
    sdp_set_service_classes(&record, svc_class_list);

    uuid_t root_uuid;
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    sdp_list_t* root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( &record, root_list );

    uuid_t rfcomm_uuid;
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    sdp_data_t* channel = sdp_data_alloc(SDP_UINT8, &rfcommChannel);
    sdp_list_t* rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_t* proto_list = sdp_list_append( 0, rfcomm_list );
    sdp_list_t* access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( &record, access_proto_list );

    sdp_profile_desc_t profile;
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    sdp_list_t* profile_list = sdp_list_append(0, &profile);
    sdp_set_profile_descs(&record, profile_list);

    sdp_set_info_attr(&record, service_name, service_prov, service_dsc);

    sdpSession = sdp_connect( &BDADDR_ANY_VAL, &BDADDR_LOCAL_VAL, SDP_RETRY_IF_BUSY );
    if (sdpSession) {
      int err = sdp_record_register(sdpSession, &record, 0);
      if (err) {
        std::cout << "error registering\n";
      } else {
        std::cout << "seems to be registered\n";
      }
    } else {
      std::cout << "sdp connect failed\n";
    }

    sdp_data_free(channel);
    sdp_list_free(svc_class_list, 0);
    sdp_list_free(root_list, 0);
    sdp_list_free(rfcomm_list, 0);
    sdp_list_free(proto_list, 0);
    sdp_list_free(access_proto_list, 0);
    sdp_list_free(profile_list, 0);
}

void ControlsReceiver::startListening()
{
  sockaddr_rc loc_addr = { 0 };
  loc_addr.rc_channel = rfcommChannel;
  loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = BDADDR_ANY_VAL;

  int listening_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (listening_socket == -1)
  {
    std::cout << "no listening socket\n";
    return;
  }

  int bound = bind(listening_socket, (sockaddr*)&loc_addr, sizeof(loc_addr));
  if (bound == -1)
  {
    std::cout << "not bound\n";
    return;
  }

  int listening = listen(listening_socket, 1);
  if (listening == -1)
  {
    std::cout << "not listening\n";
    return;
  }
  
  while (true)
  {
    sockaddr_rc rem_addr = { 0 };
    socklen_t rem_addr_size = sizeof(rem_addr);
    std::cout << "waiting for connection...\n";
    clientConnectionSocket = accept(listening_socket, (sockaddr*)&rem_addr, &rem_addr_size);
    char string_rem_addr[1024] = { 0 };
    ba2str(&rem_addr.rc_bdaddr, string_rem_addr);
    std::cout << "accepted connection from " << string_rem_addr << "\n";
    receiveMessages();
  }
}

void ControlsReceiver::receiveMessages()
{
  while (true)
  {
    char answ;
    int received_bytes = recv(clientConnectionSocket, &answ, 1, 0);
    // std::cout << "received " << received_bytes << "\n";
    if (received_bytes < 0) {
      std::cout << "connection failed\n";
      onCommandCallback(STOP_MOVING);
      close(clientConnectionSocket);
      return;
    }
    onCommandCallback(answ);
  }
}

ControlsReceiver::~ControlsReceiver()
{
  sdp_close(sdpSession);
  close(listeningSocket);
  close(clientConnectionSocket);
}
