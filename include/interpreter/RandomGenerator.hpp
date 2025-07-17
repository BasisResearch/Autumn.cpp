// Add after the existing includes
#include <cstdint>

namespace Autumn {

class RandomGenerator {
private:
    uint32_t seed;
    uint32_t state;
    // Using standard minstd_rand parameters
    static constexpr uint32_t MULTIPLIER = 48271;
    static constexpr uint32_t MODULUS = 2147483647;  // 2^31 - 1

public:
    RandomGenerator(uint64_t initialSeed = 0) : seed(static_cast<uint32_t>(initialSeed)), state(static_cast<uint32_t>(initialSeed)) {}
    
    void setSeed(uint64_t newSeed) {
        seed = static_cast<uint32_t>(newSeed);
        state = static_cast<uint32_t>(newSeed);
    }
    
    uint64_t getSeed() const { return static_cast<uint64_t>(seed); }
    
    // Generate a random number between 0 and max (exclusive)
    uint64_t next(uint64_t max) {
        if (max == 0) return 0;
        state = (MULTIPLIER * state) % MODULUS;
        return static_cast<uint64_t>(state) % max;
    }
    
    // Generate a random number between min and max (inclusive)
    int64_t nextRange(int64_t min, int64_t max) {
        if (min >= max) return min;
        uint64_t range = max - min + 1;
        return min + static_cast<int64_t>(next(range));
    }
    
    // Generate a random double between 0 and 1
    double nextDouble() {
        state = (MULTIPLIER * state) % MODULUS;
        return static_cast<double>(state) / static_cast<double>(MODULUS);
    }
};
}