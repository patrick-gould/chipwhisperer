/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "header.h"
#include "esp_err.h"
#include "sdkconfig.h"                 // Defines a couple defs (ONFIG_IDF_TARGET_X86, CONFIG_ESP_SYSTEM_USE_EH_FRAME) as 1.

// #include "bootloader_flash_priv.h"  // Defines how to flash target (read, write, erase) + defines hex for various commands.
// #include "bootloader_sha.h"         // Declares some sha functions for communicating with target. !This may cause size constraints!
// #include "bootloader_utility.h"     // Declares helper functions for loading and parsing data.
// #include "esp_log.h"                // Logging; might cut.
// #include "esp_image_format.h"       // Declares functions loads, varify bootloader images; defines bootloader image struct.
// #include "esp_secure_boot.h"        // Sets up secure boot, varifies signitures, esp_secure_boot_verify_signature is noted as having no fault protections. Perhaps an uninteresting target.
#include "esp_fault.h"                 // Assertations, fault handling. Has fault protections for illeagal instructions by resetting the chip.
// #include "esp32/rom/sha.h"          // SHA operations; uses a lock for each operation.
#include "uECC_verify_antifault.h"     // "uECC_verify() that also copies message_hash to verified_hash..." little mean of them.

#include <sys/param.h>                 // Machine-level constants.
// #include <string.h>                 // Will likely have to cut due to size constraints. Adds about 10kb to image.

// Added Lines
#define CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME 1
#define ESP_SECURE_BOOT_SCHEME 0xdeadbeef
// Added Lines


// ESP_LOG_ATTR_TAG(TAG, "secure_boot"); // Logging function; can cut

#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME // We will always want this to be true.

/* Below are pointers to key files embedded in the boot image, we don't have a full image to play with so we don't have those files. So, we will use dummy pointers so the calculations still procred as normal */
// extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
// extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64
extern const uint8_t signature_verification_key_start = 0;    // The difference between these values need only be 64.
extern const uint8_t signature_verification_key_end = SIGNATURE_VERIFICATION_KEYLEN;

// uECC_Curve uECC_secp256r1(void) { return &curve_secp256r1; } // Not possible without hardware support.

// esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
// {
//     uint8_t digest[ESP_SECURE_BOOT_DIGEST_LEN];
//     uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN] = { 0 }; /* ignored in this function */
//     const esp_secure_boot_sig_block_t *sigblock;

//     // ESP_LOGD(TAG, "verifying signature src_addr 0x%" PRIx32 " length 0x%" PRIx32, src_addr, length);

//     esp_err_t err = bootloader_sha256_flash_contents(src_addr, length, digest);
//     if (err != ESP_OK) {
//         return err;
//     }

//     // Map the signature block
//     sigblock = (const esp_secure_boot_sig_block_t *) bootloader_mmap(src_addr + length, sizeof(esp_secure_boot_sig_block_t));
//     if(!sigblock) {
//         // ESP_LOGE(TAG, "bootloader_mmap(0x%" PRIx32 ", 0x%x) failed", src_addr + length, sizeof(esp_secure_boot_sig_block_t));
//         return ESP_FAIL;
//     }
//     // Verify the signature
//     err = esp_secure_boot_verify_ecdsa_signature_block(sigblock, digest, verified_digest);
//     // Unmap
//     bootloader_munmap(sigblock);

//     return err;
// }

uECC_Curve uECC_secp256r1(void)
{
	return &curve_secp256r1;
}


esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    ptrdiff_t keylen;

    keylen = signature_verification_key_end - signature_verification_key_start;
    if (keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        // ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen); // Logging function; can cut.
        return ESP_FAIL;
    }

    if (sig_block->version != ESP_SECURE_BOOT_SCHEME) {
        // ESP_LOGE(TAG, "image has invalid signature version field 0x%08" PRIx32 " (image without a signature?)", sig_block->version); // Logging function; can cut.
        return ESP_FAIL;
    }

    // ESP_LOGD(TAG, "Verifying secure boot signature"); // Logging function; can cut.

    bool is_valid; // <-- Attack point. Default value will be garbage from the stack.
    is_valid = uECC_verify_antifault(
                           (const uint8_t*)signature_verification_key_start, // skip this with fault
                           image_digest,
                           ESP_SECURE_BOOT_DIGEST_LEN,
                           sig_block->signature,
                           uECC_secp256r1(),
                           verified_digest
                        );
    // ESP_LOGD(TAG, "Verification result %d", is_valid); // Logging function; can cut.
    return !is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID; // will evaluate to true
}

#endif CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME // We will always want this to be true
