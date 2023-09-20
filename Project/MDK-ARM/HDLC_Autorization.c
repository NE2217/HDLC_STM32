#include <stdint.h>

//enum 

static uint8_t send_1[] = { 0x7E, 0xA0, 0x23, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x93, 0x77, 0x28, 0x81, 0x80, 0x14, 0x05, 0x02,
0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
0x00, 0x01, 0x72, 0xE3, 0x7E };

static uint8_t get_1[] = { 0x7E, 0xA0, 0x23, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0xB9, 0x33, 0x81, 0x80, 0x14, 0x05, 0x02,
0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
0x00, 0x01, 0x72, 0xE3, 0x7E };

static uint8_t send_2[] = { 0x7E, 0xA0, 0x47, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x10, 0x17, 0x55, 0xE6, 0xE6, 0x00, 0x60, 0x36,
0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B,
0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04,
0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0xD5, 0x24, 0x7E };

static uint8_t get_2[] = { 0x7E, 0xA0, 0x3A, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x30, 0xD6, 0xB7, 0xE6, 0xE7, 0x00, 0x61, 0x29,
0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F,
0x04, 0x00, 0x00, 0x10, 0x15, 0x04, 0x00, 0x00, 0x07, 0x62, 0xC5, 0x7E };

static uint8_t send_3[] = { 0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x32, 0x4A, 0x53, 0xE6, 0xE6, 0x00, 0xC0, 0x01,
0x40, 0x00, 0x01, 0x00, 0x00, 0x60, 0x01, 0x08, 0xFF, 0x02, 0x00, 0xDB, 0x82, 0x7E };

static uint8_t get_3[] = { 0x7E, 0xA0, 0x35, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x52, 0x36, 0xEE, 0xE6, 0xE7, 0x00, 0xC4, 0x01,
0x40, 0x00, 0x09, 0x20, 0x39, 0x2E, 0x38, 0x2E, 0x33, 0x30, 0x37, 0x36, 0x2E, 0x31, 0x2E, 0x39,
0x2E, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x24, 0x06, 0x7E };

static uint8_t send_4[] = { 0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x54, 0x7A, 0x55, 0xE6, 0xE6, 0x00, 0xC0, 0x01,
0x40, 0x00, 0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0xFF, 0x02, 0x00, 0x03, 0x67, 0x7E };

static uint8_t get_4[] = { 0x7E, 0xA0, 0x35, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x74, 0x02, 0xAA, 0xE6, 0xE7, 0x00, 0xC4, 0x01,
0x40, 0x00, 0x09, 0x20, 0x30, 0x31, 0x32, 0x36, 0x33, 0x36, 0x31, 0x38, 0x37, 0x38, 0x30, 0x34,
0x34, 0x35, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1D, 0xCA, 0x7E };

static uint8_t send_5[] = { 0x7E, 0xA0, 0x0A, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x53, 0xC1, 0xCC, 0x7E };

static uint8_t get_5[] = { 0x7E, 0xA0, 0x21, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0x02, 0x04, 0x81, 0x80, 0x12, 0x05, 0x01,
0x80, 0x06, 0x01, 0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01, 0x53, 0x3B, 0x7E };
