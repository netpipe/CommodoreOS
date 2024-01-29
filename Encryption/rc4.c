#include <stdint.h>

void rc4_init(uint8_t* s, const uint8_t* key, size_t key_len) {
    for (int i = 0; i < 256; i++) {
        s[i] = i;
    }

    uint8_t j = 0;
    for (int i = 0; i < 256; i++) {
        j += s[i] + key[i % key_len];
        uint8_t temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

void rc4_crypt(uint8_t* s, uint8_t* data, size_t data_len) {
    uint8_t i = 0, j = 0;
    for (size_t k = 0; k < data_len; k++) {
        i++;
        j += s[i];
        uint8_t temp = s[i];
        s[i] = s[j];
        s[j] = temp;
        data[k] ^= s[(s[i] + s[j]) & 0xFF];
    }
}

int main() {
    uint8_t s[256];
    uint8_t key[] = {0x12, 0x34, 0x56, 0x78}; // Key for RC4
    uint8_t data[] = "Hello World"; // Data to be encrypted

    rc4_init(s, key, sizeof(key));
    rc4_crypt(s, data, strlen(data)); // Encrypt

    rc4_init(s, key, sizeof(key)); // Reinitialize for decryption
    rc4_crypt(s, data, strlen(data)); // Decrypt

    return 0;
}

