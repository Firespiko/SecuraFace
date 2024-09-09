#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>

int generate_key_iv(unsigned char *key, unsigned char *iv) {
    if (!RAND_bytes(key, 32)) { // AES-256 key is 32 bytes
        fprintf(stderr, "Error generating key\n");
        return -1;
    }

    if (!RAND_bytes(iv, 16)) { // AES block size is 16 bytes
        fprintf(stderr, "Error generating IV\n");
        return -1;
    }

    return 0;
}

int encrypt(const char *input_file, const char *output_file, unsigned char *key, unsigned char *iv) {
    FILE *in_fp = fopen(input_file, "rb");
    FILE *out_fp = fopen(output_file, "wb");
    if (!in_fp || !out_fp) {
        perror("File open error");
        return -1;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    unsigned char buffer[1024];
    unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
    int len, ciphertext_len;

    while ((len = fread(buffer, 1, sizeof(buffer), in_fp)) > 0) {
        EVP_EncryptUpdate(ctx, ciphertext, &ciphertext_len, buffer, len);
        fwrite(ciphertext, 1, ciphertext_len, out_fp);
    }

    EVP_EncryptFinal_ex(ctx, ciphertext, &ciphertext_len);
    fwrite(ciphertext, 1, ciphertext_len, out_fp);

    EVP_CIPHER_CTX_free(ctx);
    fclose(in_fp);
    fclose(out_fp);

    return 0;
}

int decrypt(const char *input_file, const char *output_file, unsigned char *key, unsigned char *iv) {
    FILE *in_fp = fopen(input_file, "rb");
    FILE *out_fp = fopen(output_file, "wb");
    if (!in_fp || !out_fp) {
        perror("File open error");
        return -1;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    unsigned char buffer[1024];
    unsigned char plaintext[1024 + EVP_MAX_BLOCK_LENGTH];
    int len, plaintext_len;

    while ((len = fread(buffer, 1, sizeof(buffer), in_fp)) > 0) {
        EVP_DecryptUpdate(ctx, plaintext, &plaintext_len, buffer, len);
        fwrite(plaintext, 1, plaintext_len, out_fp);
    }

    EVP_DecryptFinal_ex(ctx, plaintext, &plaintext_len);
    fwrite(plaintext, 1, plaintext_len, out_fp);

    EVP_CIPHER_CTX_free(ctx);
    fclose(in_fp);
    fclose(out_fp);

    return 0;
}

int main() {
    unsigned char key[32]; // AES-256 key size
    unsigned char iv[16];  // AES block size

    // Generate key and IV
    if (generate_key_iv(key, iv) < 0) {
        return 1;
    }

    // Save key and IV to files
    FILE *key_fp = fopen("../backend/model/key.bin", "wb");
    FILE *iv_fp = fopen("../backend/model/iv.bin", "wb");
    fwrite(key, 1, sizeof(key), key_fp);
    fwrite(iv, 1, sizeof(iv), iv_fp);
    fclose(key_fp);
    fclose(iv_fp);

    // Encrypt the model.onnx file
    if (encrypt("../backend/model/model.onnx", "../backend/model/model.onnx.enc", key, iv) < 0) {
        return 1;
    }


    return 0;
}

