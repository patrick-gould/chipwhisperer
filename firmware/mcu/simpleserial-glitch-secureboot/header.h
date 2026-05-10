#include <stddef.h>
#include <stdbool.h>
#include "ecc.h"
/*
In this header we give some definitions that are used by our snippet of secureboot, but does not draw from the spiderweb of other headers that would bloat our code's size.

- [author of note anonymized for submission review]
*/

#define CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME 1    // Define as true; this enables SecureBoot V1.
#define ESP_SECURE_BOOT_SCHEME 0xdeadbeef           // Dummy value.

#define ESP_ERR_IMAGE_BASE       0x2000
#define ESP_ERR_IMAGE_FLASH_FAIL (ESP_ERR_IMAGE_BASE + 1)
#define ESP_ERR_IMAGE_INVALID    (ESP_ERR_IMAGE_BASE + 2)

typedef int esp_err_t; // From esp_err.h
typedef unsigned char uint8_t;
// typedef unsigned int uint32_t;


#define ARRAY_LENGTH 64                             // Length of signature in esp_secure_boot_sig_block_t
#define ESP_SECURE_BOOT_DIGEST_LEN 32               // Length of digest a esp_secure_boot_sig_block_t struct.
// From esp_secure_boot.h
typedef struct {
    uint32_t version;
    uint8_t signature[ARRAY_LENGTH];
} esp_secure_boot_sig_block_t;
