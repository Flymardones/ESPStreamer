#ifndef ESPSTREAMER_NVS_HPP
#define ESPSTREAMER_NVS_HPP


#include <cstdio>
#include <string>
#include <nvs_flash.h>

#define MAX_LEN_BYTES 64

#define WiFiDefaultValue false

class NVS {
private:
    nvs_handle_t nvsHandle;

    bool createNVSPartition();


public:
    static NVS& GetInstance() { static NVS nvs; return nvs; }
    NVS();
    ~NVS();
    void reset();

    uint32_t getBinaryValue(std::string key, uint8_t* data, uint32_t maxSize);
    std::string getStringValue(std::string key);
    bool getBoolValue(std::string key);

    bool setBinaryValue(std::string key, uint8_t* data, uint32_t size);
    bool setStringValue(std::string key, std::string value);
    bool setBoolValue(std::string key, bool value);
};

#endif //ESPSTREAMER_NVS_HPP
