#include <stdint.h>
#include <string.h>

void xor_cipher(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len) {
    size_t i;
    for (i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

int main() {
    uint8_t data[] = "Hello World"; // Data to be encrypted
    uint8_t key[] = {0x12, 0x34, 0x56, 0x78}; // Encryption key
    size_t data_len = strlen(data);
    size_t key_len = sizeof(key);

    xor_cipher(data, data_len, key, key_len); // Encrypt
    xor_cipher(data, data_len, key, key_len); // Decrypt (same as encryption)

    return 0;
}

