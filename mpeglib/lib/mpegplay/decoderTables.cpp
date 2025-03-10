/*
 * decoders.c
 *
 * This file contains all the routines for Huffman decoding required in 
 * MPEG
 *
 */


#include "decoderTables.h"


/* Decoding table for macroblock_address_increment */
mb_addr_inc_entry     mb_addr_inc[2048];

/* Decoding table for macroblock_type in predictive-coded pictures */
mb_type_entry         mb_type_P[64];

/* Decoding table for macroblock_type in bidirectionally-coded pictures */
mb_type_entry         mb_type_B[64];

/* Decoding table for motion vectors */
motion_vectors_entry  motion_vectors[2048];

/* Decoding table for coded_block_pattern */

coded_block_pattern_entry coded_block_pattern[512] = 
{ {(unsigned int)MPGDECODE_ERROR, 0}, {(unsigned int)MPGDECODE_ERROR, 0}, {39, 9}, {27, 9}, {59, 9}, {55, 9}, {47, 9}, {31, 9},
    {58, 8}, {58, 8}, {54, 8}, {54, 8}, {46, 8}, {46, 8}, {30, 8}, {30, 8},
    {57, 8}, {57, 8}, {53, 8}, {53, 8}, {45, 8}, {45, 8}, {29, 8}, {29, 8},
    {38, 8}, {38, 8}, {26, 8}, {26, 8}, {37, 8}, {37, 8}, {25, 8}, {25, 8},
    {43, 8}, {43, 8}, {23, 8}, {23, 8}, {51, 8}, {51, 8}, {15, 8}, {15, 8},
    {42, 8}, {42, 8}, {22, 8}, {22, 8}, {50, 8}, {50, 8}, {14, 8}, {14, 8},
    {41, 8}, {41, 8}, {21, 8}, {21, 8}, {49, 8}, {49, 8}, {13, 8}, {13, 8},
    {35, 8}, {35, 8}, {19, 8}, {19, 8}, {11, 8}, {11, 8}, {7, 8}, {7, 8},
    {34, 7}, {34, 7}, {34, 7}, {34, 7}, {18, 7}, {18, 7}, {18, 7}, {18, 7},
    {10, 7}, {10, 7}, {10, 7}, {10, 7}, {6, 7}, {6, 7}, {6, 7}, {6, 7}, 
    {33, 7}, {33, 7}, {33, 7}, {33, 7}, {17, 7}, {17, 7}, {17, 7}, {17, 7}, 
    {9, 7}, {9, 7}, {9, 7}, {9, 7}, {5, 7}, {5, 7}, {5, 7}, {5, 7}, 
    {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, 
    {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, 
    {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, 
    {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, 
    {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5},
    {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5},
    {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, 
    {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, 
    {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, 
    {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, 
    {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, 
    {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, 
    {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, 
    {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, 
    {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, 
    {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, 
    {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, 
    {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, 
    {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, 
    {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, 
    {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, 
    {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, 
    {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, 
    {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, 
    {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, 
    {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, 
    {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, 
    {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4},
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}
};

/* Decoding tables for dct_dc_size_luminance */
dct_dc_size_entry dct_dc_size_luminance[32] =
{   {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {0, 3}, {0, 3}, {0, 3}, {0, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {4, 3}, {4, 3}, {4, 3}, {4, 3}, {5, 4}, {5, 4}, {6, 5}, {(unsigned int)MPGDECODE_ERROR, 0}
};

dct_dc_size_entry dct_dc_size_luminance1[16] =
{   {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6},
    {8, 7}, {8, 7}, {8, 7}, {8, 7}, {9, 8}, {9, 8}, {10, 9}, {11, 9}
};

/* Decoding table for dct_dc_size_chrominance */
dct_dc_size_entry dct_dc_size_chrominance[32] =
{   {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {4, 4}, {4, 4}, {5, 5}, {(unsigned int)MPGDECODE_ERROR, 0}
};

dct_dc_size_entry dct_dc_size_chrominance1[32] =
{   {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, 
    {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, 
    {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, 
    {8, 8}, {8, 8}, {8, 8}, {8, 8}, {9, 9}, {9, 9}, {10, 10}, {11, 10}
};

/* DCT coeff tables. */

unsigned short int dct_coeff_tbl_0[256] =
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0x052f, 0x051f, 0x050f, 0x04ff, 
0x183f, 0x402f, 0x3c2f, 0x382f, 
0x342f, 0x302f, 0x2c2f, 0x7c1f, 
0x781f, 0x741f, 0x701f, 0x6c1f, 
0x028e, 0x028e, 0x027e, 0x027e, 
0x026e, 0x026e, 0x025e, 0x025e, 
0x024e, 0x024e, 0x023e, 0x023e, 
0x022e, 0x022e, 0x021e, 0x021e, 
0x020e, 0x020e, 0x04ee, 0x04ee, 
0x04de, 0x04de, 0x04ce, 0x04ce, 
0x04be, 0x04be, 0x04ae, 0x04ae, 
0x049e, 0x049e, 0x048e, 0x048e, 
0x01fd, 0x01fd, 0x01fd, 0x01fd, 
0x01ed, 0x01ed, 0x01ed, 0x01ed, 
0x01dd, 0x01dd, 0x01dd, 0x01dd, 
0x01cd, 0x01cd, 0x01cd, 0x01cd, 
0x01bd, 0x01bd, 0x01bd, 0x01bd, 
0x01ad, 0x01ad, 0x01ad, 0x01ad, 
0x019d, 0x019d, 0x019d, 0x019d, 
0x018d, 0x018d, 0x018d, 0x018d, 
0x017d, 0x017d, 0x017d, 0x017d, 
0x016d, 0x016d, 0x016d, 0x016d, 
0x015d, 0x015d, 0x015d, 0x015d, 
0x014d, 0x014d, 0x014d, 0x014d, 
0x013d, 0x013d, 0x013d, 0x013d, 
0x012d, 0x012d, 0x012d, 0x012d, 
0x011d, 0x011d, 0x011d, 0x011d, 
0x010d, 0x010d, 0x010d, 0x010d, 
0x282c, 0x282c, 0x282c, 0x282c, 
0x282c, 0x282c, 0x282c, 0x282c, 
0x242c, 0x242c, 0x242c, 0x242c, 
0x242c, 0x242c, 0x242c, 0x242c, 
0x143c, 0x143c, 0x143c, 0x143c, 
0x143c, 0x143c, 0x143c, 0x143c, 
0x0c4c, 0x0c4c, 0x0c4c, 0x0c4c, 
0x0c4c, 0x0c4c, 0x0c4c, 0x0c4c, 
0x085c, 0x085c, 0x085c, 0x085c, 
0x085c, 0x085c, 0x085c, 0x085c, 
0x047c, 0x047c, 0x047c, 0x047c, 
0x047c, 0x047c, 0x047c, 0x047c, 
0x046c, 0x046c, 0x046c, 0x046c, 
0x046c, 0x046c, 0x046c, 0x046c, 
0x00fc, 0x00fc, 0x00fc, 0x00fc, 
0x00fc, 0x00fc, 0x00fc, 0x00fc, 
0x00ec, 0x00ec, 0x00ec, 0x00ec, 
0x00ec, 0x00ec, 0x00ec, 0x00ec, 
0x00dc, 0x00dc, 0x00dc, 0x00dc, 
0x00dc, 0x00dc, 0x00dc, 0x00dc, 
0x00cc, 0x00cc, 0x00cc, 0x00cc, 
0x00cc, 0x00cc, 0x00cc, 0x00cc, 
0x681c, 0x681c, 0x681c, 0x681c, 
0x681c, 0x681c, 0x681c, 0x681c, 
0x641c, 0x641c, 0x641c, 0x641c, 
0x641c, 0x641c, 0x641c, 0x641c, 
0x601c, 0x601c, 0x601c, 0x601c, 
0x601c, 0x601c, 0x601c, 0x601c, 
0x5c1c, 0x5c1c, 0x5c1c, 0x5c1c, 
0x5c1c, 0x5c1c, 0x5c1c, 0x5c1c, 
0x581c, 0x581c, 0x581c, 0x581c, 
0x581c, 0x581c, 0x581c, 0x581c, 
};

unsigned short int dct_coeff_tbl_1[16] = 
{
0x00bb, 0x202b, 0x103b, 0x00ab, 
0x084b, 0x1c2b, 0x541b, 0x501b, 
0x009b, 0x4c1b, 0x481b, 0x045b, 
0x0c3b, 0x008b, 0x182b, 0x441b, 
};

unsigned short int dct_coeff_tbl_2[4] =
{
0x4019, 0x1429, 0x0079, 0x0839, 
};

unsigned short int dct_coeff_tbl_3[4] = 
{
0x0449, 0x3c19, 0x3819, 0x1029, 
};

unsigned short int dct_coeff_next[256] = 
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xf7d5, 0xf7d5, 0xf7d5, 0xf7d5, 
0x0826, 0x0826, 0x2416, 0x2416, 
0x0046, 0x0046, 0x2016, 0x2016, 
0x1c15, 0x1c15, 0x1c15, 0x1c15, 
0x1815, 0x1815, 0x1815, 0x1815, 
0x0425, 0x0425, 0x0425, 0x0425, 
0x1415, 0x1415, 0x1415, 0x1415, 
0x3417, 0x0067, 0x3017, 0x2c17, 
0x0c27, 0x0437, 0x0057, 0x2817, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
};

unsigned short int dct_coeff_first[256] = 
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xf7d5, 0xf7d5, 0xf7d5, 0xf7d5, 
0x0826, 0x0826, 0x2416, 0x2416, 
0x0046, 0x0046, 0x2016, 0x2016, 
0x1c15, 0x1c15, 0x1c15, 0x1c15, 
0x1815, 0x1815, 0x1815, 0x1815, 
0x0425, 0x0425, 0x0425, 0x0425, 
0x1415, 0x1415, 0x1415, 0x1415, 
0x3417, 0x0067, 0x3017, 0x2c17, 
0x0c27, 0x0437, 0x0057, 0x2817, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
};

/* Macro for filling up the decoding table for mb_addr_inc */
#define ASSIGN1(start, end, step, val, num) \
  for (i = start; i < end; i+= step) { \
    for (j = 0; j < step; j++) { \
      mb_addr_inc[i+j].value = val; \
      mb_addr_inc[i+j].num_bits = num; \
    } \
    val--; \
    }



/*
 *--------------------------------------------------------------
 *
 * init_mb_addr_inc --
 *
 *	Initialize the VLC decoding table for macro_block_address_increment
 *
 * Results:
 *	The decoding table for macro_block_address_increment will
 *      be filled; illegal values will be filled as MPGDECODE_ERROR.
 *
 * Side effects:
 *	The global array mb_addr_inc will be filled.
 *
 *--------------------------------------------------------------
 */
static void init_mb_addr_inc()
{
  int i, j, val;

  for (i = 0; i < 8; i++) {
    mb_addr_inc[i].value = MPGDECODE_ERROR;
    mb_addr_inc[i].num_bits = 0;
  }

  mb_addr_inc[8].value = MACRO_BLOCK_ESCAPE;
  mb_addr_inc[8].num_bits = 11;

  for (i = 9; i < 15; i++) {
    mb_addr_inc[i].value = MPGDECODE_ERROR;
    mb_addr_inc[i].num_bits = 0;
  }

  mb_addr_inc[15].value = MACRO_BLOCK_STUFFING;
  mb_addr_inc[15].num_bits = 11;

  for (i = 16; i < 24; i++) {
    mb_addr_inc[i].value = MPGDECODE_ERROR;
    mb_addr_inc[i].num_bits = 0;
  }

  val = 33;

  ASSIGN1(24, 36, 1, val, 11);
  ASSIGN1(36, 48, 2, val, 10);
  ASSIGN1(48, 96, 8, val, 8);
  ASSIGN1(96, 128, 16, val, 7);
  ASSIGN1(128, 256, 64, val, 5);
  ASSIGN1(256, 512, 128, val, 4);
  ASSIGN1(512, 1024, 256, val, 3);
  ASSIGN1(1024, 2048, 1024, val, 1);
}


/* Macro for filling up the decoding table for mb_type */
#define ASSIGN2(start, end, quant, motion_forward, motion_backward, pattern, intra, num, mb_type) \
  for (i = start; i < end; i ++) { \
    mb_type[i].mb_quant = quant; \
    mb_type[i].mb_motion_forward = motion_forward; \
    mb_type[i].mb_motion_backward = motion_backward; \
    mb_type[i].mb_pattern = pattern; \
    mb_type[i].mb_intra = intra; \
    mb_type[i].num_bits = num; \
  }
	 


/*
 *--------------------------------------------------------------
 *
 * init_mb_type_P --
 *
 *	Initialize the VLC decoding table for macro_block_type in
 *      predictive-coded pictures.
 *
 * Results:
 *	The decoding table for macro_block_type in predictive-coded
 *      pictures will be filled; illegal values will be filled as MPGDECODE_ERROR.
 *
 * Side effects:
 *	The global array mb_type_P will be filled.
 *
 *--------------------------------------------------------------
 */
static void init_mb_type_P() {
  int i;

  mb_type_P[0].mb_quant = mb_type_P[0].mb_motion_forward 
    = mb_type_P[0].mb_motion_backward = mb_type_P[0].mb_pattern 
      = mb_type_P[0].mb_intra = (unsigned int)MPGDECODE_ERROR;
  mb_type_P[0].num_bits = 0;

  ASSIGN2(1, 2, 1, 0, 0, 0, 1, 6, mb_type_P)
  ASSIGN2(2, 4, 1, 0, 0, 1, 0, 5, mb_type_P)
  ASSIGN2(4, 6, 1, 1, 0, 1, 0, 5, mb_type_P);
  ASSIGN2(6, 8, 0, 0, 0, 0, 1, 5, mb_type_P);
  ASSIGN2(8, 16, 0, 1, 0, 0, 0, 3, mb_type_P);
  ASSIGN2(16, 32, 0, 0, 0, 1, 0, 2, mb_type_P);
  ASSIGN2(32, 64, 0, 1, 0, 1, 0, 1, mb_type_P);
}




/*
 *--------------------------------------------------------------
 *
 * init_mb_type_B --
 *
 *	Initialize the VLC decoding table for macro_block_type in
 *      bidirectionally-coded pictures.
 *
 * Results:
 *	The decoding table for macro_block_type in bidirectionally-coded
 *      pictures will be filled; illegal values will be filled as MPGDECODE_ERROR.
 *
 * Side effects:
 *	The global array mb_type_B will be filled.
 *
 *--------------------------------------------------------------
 */
static void init_mb_type_B() {
  int i;

  mb_type_B[0].mb_quant = mb_type_B[0].mb_motion_forward 
    = mb_type_B[0].mb_motion_backward = mb_type_B[0].mb_pattern 
      = mb_type_B[0].mb_intra = (unsigned int) MPGDECODE_ERROR;
  mb_type_B[0].num_bits = 0;

  ASSIGN2(1, 2, 1, 0, 0, 0, 1, 6, mb_type_B);
  ASSIGN2(2, 3, 1, 0, 1, 1, 0, 6, mb_type_B);
  ASSIGN2(3, 4, 1, 1, 0, 1, 0, 6, mb_type_B);
  ASSIGN2(4, 6, 1, 1, 1, 1, 0, 5, mb_type_B);
  ASSIGN2(6, 8, 0, 0, 0, 0, 1, 5, mb_type_B);
  ASSIGN2(8, 12, 0, 1, 0, 0, 0, 4, mb_type_B);
  ASSIGN2(12, 16, 0, 1, 0, 1, 0, 4, mb_type_B);
  ASSIGN2(16, 24, 0, 0, 1, 0, 0, 3, mb_type_B);
  ASSIGN2(24, 32, 0, 0, 1, 1, 0, 3, mb_type_B);
  ASSIGN2(32, 48, 0, 1, 1, 0, 0, 2, mb_type_B);
  ASSIGN2(48, 64, 0, 1, 1, 1, 0, 2, mb_type_B);
}


/* Macro for filling up the decoding tables for motion_vectors */
#define ASSIGN3(start, end, step, val, num) \
  for (i = start; i < end; i+= step) { \
    for (j = 0; j < step / 2; j++) { \
      motion_vectors[i+j].code = val; \
      motion_vectors[i+j].num_bits = num; \
    } \
    for (j = step / 2; j < step; j++) { \
      motion_vectors[i+j].code = -val; \
      motion_vectors[i+j].num_bits = num; \
    } \
    val--; \
  }



/*
 *--------------------------------------------------------------
 *
 * init_motion_vectors --
 *
 *	Initialize the VLC decoding table for the various motion
 *      vectors, including motion_horizontal_forward_code, 
 *      motion_vertical_forward_code, motion_horizontal_backward_code,
 *      and motion_vertical_backward_code.
 *
 * Results:
 *	The decoding table for the motion vectors will be filled;
 *      illegal values will be filled as MPGDECODE_ERROR.
 *
 * Side effects:
 *	The global array motion_vector will be filled.
 *
 *--------------------------------------------------------------
 */
static void init_motion_vectors()
{
  int i, j, val = 16;

  for (i = 0; i < 24; i++) {
    motion_vectors[i].code = MPGDECODE_ERROR;
    motion_vectors[i].num_bits = 0; 
  }

  ASSIGN3(24, 36, 2, val, 11);
  ASSIGN3(36, 48, 4, val, 10);
  ASSIGN3(48, 96, 16, val, 8);
  ASSIGN3(96, 128, 32, val, 7);
  ASSIGN3(128, 256, 128, val, 5);
  ASSIGN3(256, 512, 256, val, 4);
  ASSIGN3(512, 1024, 512, val, 3);
  ASSIGN3(1024, 2048, 1024, val, 1);
}



extern void init_pre_idct();


/*
 *--------------------------------------------------------------
 *
 * init_tables --
 *
 *	Initialize all the tables for VLC decoding; this must be
 *      called when the system is set up before any decoding can
 *      take place.
 *
 * Results:
 *	All the decoding tables will be filled accordingly.
 *
 * Side effects:
 *	The corresponding global array for each decoding table 
 *      will be filled.
 *
 *--------------------------------------------------------------
 */    
void init_tables() {
  
  init_mb_addr_inc();
  init_mb_type_P();
  init_mb_type_B();
  init_motion_vectors();
  
  if (qualityFlag) {
    cout << "qualityFlag in init_tables float idct removed"<<endl;
  }
  init_pre_idct();

}


