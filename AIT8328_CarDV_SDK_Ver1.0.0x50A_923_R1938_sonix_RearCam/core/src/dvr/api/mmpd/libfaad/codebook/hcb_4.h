
/* 2-step huffman table HCB_4 */


/* 1st step: 5 bits
 *           2^5 = 32 entries
 *
 * Used to find offset into 2nd step table and number of extra bits to get
 */
static hcb hcb4_1[] = {
    /* 4 bit codewords */
    { /* 00000 */ 0, 0 },
    { /*       */ 0, 0 },
    { /* 00010 */ 1, 0 },
    { /*       */ 1, 0 },
    { /* 00100 */ 2, 0 },
    { /*       */ 2, 0 },
    { /* 00110 */ 3, 0 },
    { /*       */ 3, 0 },
    { /* 01000 */ 4, 0 },
    { /*       */ 4, 0 },
    { /* 01010 */ 5, 0 },
    { /*       */ 5, 0 },
    { /* 01100 */ 6, 0 },
    { /*       */ 6, 0 },
    { /* 01110 */ 7, 0 },
    { /*       */ 7, 0 },
    { /* 10000 */ 8, 0 },
    { /*       */ 8, 0 },
    { /* 10010 */ 9, 0 },
    { /*       */ 9, 0 },

    /* 5 bit codewords */
    { /* 10100 */ 10, 0 },
    { /* 10101 */ 11, 0 },
    { /* 10110 */ 12, 0 },
    { /* 10111 */ 13, 0 },
    { /* 11000 */ 14, 0 },
    { /* 11001 */ 15, 0 },

    /* 7 bit codewords */
    { /* 11010 */ 16, 2 },
    { /* 11011 */ 20, 2 },

    /* 7/8 bit codewords */
    { /* 11100 */ 24, 3 },

    /* 8 bit codewords */
    { /* 11101 */ 32, 3 },

    /* 8/9 bit codewords */
    { /* 11110 */ 40, 4 },

    /* 9/10/11/12 bit codewords */
    { /* 11111 */ 56, 7 }
};

/* 2nd step table
 *
 * Gives size of codeword and actual data (x,y,v,w)
 */
static hcb_2_quad hcb4_2[] = {
    /* 4 bit codewords */
    { 4,  1,  1,  1,  1 },
    { 4,  0,  1,  1,  1 },
    { 4,  1,  1,  0,  1 },
    { 4,  1,  1,  1,  0 },
    { 4,  1,  0,  1,  1 },
    { 4,  1,  0,  0,  0 },
    { 4,  1,  1,  0,  0 },
    { 4,  0,  0,  0,  0 },
    { 4,  0,  0,  1,  1 },
    { 4,  1,  0,  1,  0 },

    /* 5 bit codewords */
    { 5,  1,  0,  0,  1 },
    { 5,  0,  1,  1,  0 },
    { 5,  0,  0,  0,  1 },
    { 5,  0,  1,  0,  1 },
    { 5,  0,  0,  1,  0 },
    { 5,  0,  1,  0,  0 },

    /* 7 bit codewords */
    /* first 5 bits: 11010 */
    { 7,  2,  1,  1,  1 },
    { 7,  1,  1,  2,  1 },
    { 7,  1,  2,  1,  1 },
    { 7,  1,  1,  1,  2 },
    /* first 5 bits: 11011 */
    { 7,  2,  1,  1,  0 },
    { 7,  2,  1,  0,  1 },
    { 7,  1,  2,  1,  0 },
    { 7,  2,  0,  1,  1 },

    /* 7/8 bit codewords */
    /* first 5 bits: 11100 */
    { 7,  0,  1,  2,  1 }, { 7,  0,  1,  2,  1 },
    { 8,  0,  1,  1,  2 },
    { 8,  1,  1,  2,  0 },
    { 8,  0,  2,  1,  1 },
    { 8,  1,  0,  1,  2 },
    { 8,  1,  2,  0,  1 },
    { 8,  1,  1,  0,  2 },

    /* 8 bit codewords */
    { 8,  1,  0,  2,  1 },
    { 8,  2,  1,  0,  0 },
    { 8,  2,  0,  1,  0 },
    { 8,  1,  2,  0,  0 },
    { 8,  2,  0,  0,  1 },
    { 8,  0,  1,  0,  2 },
    { 8,  0,  2,  1,  0 },
    { 8,  0,  0,  1,  2 },

    /* 8/9 bit codewords */
    { 8,  0,  1,  2,  0 }, { 8,  0,  1,  2,  0 },
    { 8,  0,  2,  0,  1 }, { 8,  0,  2,  0,  1 },
    { 8,  1,  0,  0,  2 }, { 8,  1,  0,  0,  2 },
    { 8,  0,  0,  2,  1 }, { 8,  0,  0,  2,  1 },
    { 8,  1,  0,  2,  0 }, { 8,  1,  0,  2,  0 },
    { 8,  2,  0,  0,  0 }, { 8,  2,  0,  0,  0 },
    { 8,  0,  0,  0,  2 }, { 8,  0,  0,  0,  2 },
    { 9,  0,  2,  0,  0 },
    { 9,  0,  0,  2,  0 },

    /* 9/10/11 bit codewords */
    /* 9 bit codewords repeated 2^3 = 8 times */
    { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 },
    { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 }, { 9,  1,  2,  2,  1 },
    { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 },
    { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 }, { 9,  2,  2,  1,  1 },
    { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 },
    { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 }, { 9,  2,  1,  2,  1 },
    { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 },
    { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 }, { 9,  1,  1,  2,  2 },
    { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 },
    { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 }, { 9,  1,  2,  1,  2 },
    { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 },
    { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 }, { 9,  2,  1,  1,  2 },
    /* 10 bit codewords repeated 2^2 = 4 times */
    { 10,  1,  2,  2,  0 }, { 10,  1,  2,  2,  0 }, { 10,  1,  2,  2,  0 }, { 10,  1,  2,  2,  0 },
    { 10,  2,  2,  1,  0 }, { 10,  2,  2,  1,  0 }, { 10,  2,  2,  1,  0 }, { 10,  2,  2,  1,  0 },
    { 10,  2,  1,  2,  0 }, { 10,  2,  1,  2,  0 }, { 10,  2,  1,  2,  0 }, { 10,  2,  1,  2,  0 },
    { 10,  0,  2,  2,  1 }, { 10,  0,  2,  2,  1 }, { 10,  0,  2,  2,  1 }, { 10,  0,  2,  2,  1 },
    { 10,  0,  1,  2,  2 }, { 10,  0,  1,  2,  2 }, { 10,  0,  1,  2,  2 }, { 10,  0,  1,  2,  2 },
    { 10,  2,  2,  0,  1 }, { 10,  2,  2,  0,  1 }, { 10,  2,  2,  0,  1 }, { 10,  2,  2,  0,  1 },
    { 10,  0,  2,  1,  2 }, { 10,  0,  2,  1,  2 }, { 10,  0,  2,  1,  2 }, { 10,  0,  2,  1,  2 },
    { 10,  2,  0,  2,  1 }, { 10,  2,  0,  2,  1 }, { 10,  2,  0,  2,  1 }, { 10,  2,  0,  2,  1 },
    { 10,  1,  0,  2,  2 }, { 10,  1,  0,  2,  2 }, { 10,  1,  0,  2,  2 }, { 10,  1,  0,  2,  2 },
    { 10,  2,  2,  2,  1 }, { 10,  2,  2,  2,  1 }, { 10,  2,  2,  2,  1 }, { 10,  2,  2,  2,  1 },
    { 10,  1,  2,  0,  2 }, { 10,  1,  2,  0,  2 }, { 10,  1,  2,  0,  2 }, { 10,  1,  2,  0,  2 },
    { 10,  2,  0,  1,  2 }, { 10,  2,  0,  1,  2 }, { 10,  2,  0,  1,  2 }, { 10,  2,  0,  1,  2 },
    { 10,  2,  1,  0,  2 }, { 10,  2,  1,  0,  2 }, { 10,  2,  1,  0,  2 }, { 10,  2,  1,  0,  2 },
    { 10,  1,  2,  2,  2 }, { 10,  1,  2,  2,  2 }, { 10,  1,  2,  2,  2 }, { 10,  1,  2,  2,  2 },
    /* 11 bit codewords repeated 2^1 = 2 times */
    { 11,  2,  1,  2,  2 }, { 11,  2,  1,  2,  2 },
    { 11,  2,  2,  1,  2 }, { 11,  2,  2,  1,  2 },
    { 11,  0,  2,  2,  0 }, { 11,  0,  2,  2,  0 },
    { 11,  2,  2,  0,  0 }, { 11,  2,  2,  0,  0 },
    { 11,  0,  0,  2,  2 }, { 11,  0,  0,  2,  2 },
    { 11,  2,  0,  2,  0 }, { 11,  2,  0,  2,  0 },
    { 11,  0,  2,  0,  2 }, { 11,  0,  2,  0,  2 },
    { 11,  2,  0,  0,  2 }, { 11,  2,  0,  0,  2 },
    { 11,  2,  2,  2,  2 }, { 11,  2,  2,  2,  2 },
    { 11,  0,  2,  2,  2 }, { 11,  0,  2,  2,  2 },
    { 11,  2,  2,  2,  0 }, { 11,  2,  2,  2,  0 },
    /* 12 bit codewords */
    { 12,  2,  2,  0,  2 },
    { 12,  2,  0,  2,  2 },
};