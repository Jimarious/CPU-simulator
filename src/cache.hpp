#include <stdint.h>

struct Cache_Block{
    friend class Pipeline; // TD: I am setting this we will see if I will be able to actually access from PL

    bool valid = false;
    bool dirty = false;
    uint32_t tag;
    uint8_t core_id;
    uint8_t access_time;
    uint8_t access_frequency; // Will need to use saturation later
};

class Cache {
    friend class Pipeline; // TD: May also need to declare Simulator here for the shared cache

    private:
        std::vector<std::vector<Cache_Block>> data;
        uint32_t totalblocks = 512;
        uint8_t associativity = 8;
        uint16_t blocksize = 64;
        uint8_t evictionpolicy = 0;
        bool cache_access();
        void cache_install();
        void cache_evict(); // TD: may need to be remade as stated
};