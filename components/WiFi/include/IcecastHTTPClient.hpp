#ifndef ESPSTREAMER_ICECASTHTTPCLIENT_H
#define ESPSTREAMER_ICECASTHTTPCLIENT_H

#include <string>
#include "ICETask.hpp"
#include "lwip/sockets.h"
#include "StreamDecoder.hpp"

#define ReadBufferSize 1024 //Larger read buffer allows ICECastHTTPClient to sleep for longer

class IcecastHTTPClient : public ICETask {
private:
    std::string hostname;
    std::string webPath;
    struct sockaddr_in socketAddress = {};
    StreamDecoder* streamDecoder = nullptr;
    int radioSocket = 0;
    uint8_t* readBuffer = nullptr;

    struct sockaddr_in hostToSockAddr(const std::string& host);
    std::string generateGetRequest(const std::string& hostname, const std::string& path);
public:
    IcecastHTTPClient(StreamDecoder* decoder);
    void runTask(void* unused) override;
    void startStream(const std::string& url);
    void startStream(const std::string& host, const std::string& path);
    void stopStream();
};

#endif //ESPSTREAMER_ICECASTHTTPCLIENT_H