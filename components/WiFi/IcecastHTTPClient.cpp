#include "IcecastHTTPClient.hpp"
#include "lwip/netdb.h"
#include "QPrint.hpp"
#include "StreamDecoder.hpp"
#include "ICESleep.hpp"
#include "freertos/task.h"

//Husk at skrive i rapport om clock syncing

IcecastHTTPClient::IcecastHTTPClient(StreamDecoder *decoder) {
    streamDecoder = decoder;
}

void IcecastHTTPClient::startStream(const std::string& url) {
    //This expects a url without the protocol. Format: live-icy.gslb01.dr.dk/A/A05L.mp3
    //find first / as everything before / is hostname
    size_t position = url.find('/');
    std::string host = url.substr(0, position);
    std::string path = url.substr(position);

    startStream(host, path);
}

void IcecastHTTPClient::startStream(const std::string& host, const std::string& path) {
    QPrint::println("Starting stream from: " + host + " path: " + path);
    socketAddress = hostToSockAddr(host);
    if (socketAddress.sin_addr.s_addr == 0) {
        return; // We found no ip address. Give up.
    }
    QPrint::println("Socket address found: " + std::to_string(socketAddress.sin_addr.s_addr));

    //The host here should be stripped to hostname, no protocol. sockaddr will contain port.
    hostname = host;
    webPath = path;

    this->setup("ICECastHTTPClient", 2048, 0);
    this->start();
}

struct sockaddr_in IcecastHTTPClient::hostToSockAddr(const std::string& host) {
    struct hostent* hostent;
    hostent=gethostbyname(host.c_str());

    struct sockaddr_in output;
    memset(&output, 0, sizeof(struct sockaddr_in));

    if (hostent != NULL) {
        output.sin_family = AF_INET;
        output.sin_port = lwip_htons(80); //We might want to pull the port from the protocol in the future.
        //While hostent technically can hold multiple addresses, the gethostbyname can only ever return 1
        memcpy(&output.sin_addr, hostent->h_addr_list[0], 4); //ipv4 always 4 bytes
    }

    return output;
}

std::string IcecastHTTPClient::generateGetRequest(const std::string& hostname, const std::string &path) {
    std::string linebreak = "\n";
    std::string request;

    request += "GET " + path + " HTTP/1.0" + linebreak;
    request += "Host: " + hostname + linebreak;
    request += linebreak + linebreak;

    QPrint::print("Get for webserver:\n");
    QPrint::print(request);

    return request;
}

void IcecastHTTPClient::stopStream() {
    TaskStatus_t decoderStatus;
    streamDecoder->stop(); //If we get the right task name, we assume the task is running..
    free(readBuffer);
    lwip_close(radioSocket);
}

void IcecastHTTPClient::runTask(void *unused) {
    if (streamDecoder == nullptr) {
        return;
    }

    radioSocket = lwip_socket(PF_INET, SOCK_STREAM, 0);

    int status = lwip_connect(radioSocket, (struct sockaddr*) &socketAddress, sizeof(struct sockaddr));
    if (status != 0) {
        lwip_close(radioSocket);
        QPrint::println("Error connecting to stream socket. Exiting.");
        this->stopStream();
    }

    int readBytes = 0;
    QPrint::println("Beginning read!");
    streamDecoder->setAudioType(StreamDecoder::MP3);
    streamDecoder->setup("stream decoder", 9216, 0); //MP3 Decoding apparently requires a very big stack...
    streamDecoder->start();
    readBuffer = (uint8_t*) malloc(ReadBufferSize);
    if (readBuffer == nullptr) {
        stopStream();
    }

    std::string httpRequest = generateGetRequest(hostname, webPath);
    uint32_t written = lwip_write(radioSocket, httpRequest.c_str(), httpRequest.size());
    QPrint::println("Wrote " + std::to_string(written) + " bytes to http socket");

    while(1) {
        readBytes = lwip_recv(radioSocket, readBuffer, ReadBufferSize, MSG_DONTWAIT);
        
        if (readBytes > 0) {
            streamDecoder->QueueData(readBuffer, readBytes);
        }
            //QPrint::println("Read " + std::to_string(readBytes) + " bytes!");
        /*} else if (readBytes < 0) {
            QPrint::println("Error in IcecastStream");
            break; //Something wrong, we shut down here.
        }*/
    }
}
