#include <stdint.h>

struct {
    bool valid = false;
    bool dirty = false;
    uint32_t tag;
    uint8_t core_id;

} Cache_Block;

struct {

} Cache_Set;

class Cache {
    bool cache_access();
    void cache_install();
    void cache_evict(); // TD: may need to be remade as stated
};