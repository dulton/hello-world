
#ifndef NR_H
#define NR_H

typedef short           nr_int16_t;
typedef unsigned short  nr_uint16_t;
typedef int             nr_int32_t;
typedef unsigned int    nr_uint32_t;

#define max(a,b) ((a) > (b) ? (a) : (b))
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#define absx(x)  (x < 0 ? -x : x);

//Noise floor must be adjusted for different environment 
#define     NR_NOISE_FLOOR             600           // for LG Adagio with AGC off
//#define     NR_NOISE_FLOOR             300           // for LG RD6300 with loudness/AGC off
//#define     NR_NOISE_FLOOR             300           // for LG RD6300 with loudness/AGC off
//#define     NR_NOISE_FLOOR             900			  // for LG RD6300 with AGC(with noise gate) on 
//#define     NR_NOISE_FLOOR             1200			// for LG RD6300 with loudness on(max=12dB), not suggest it

#if 1
// The powers-of-two parameters
#define ALPHA1      0.953125f                       //(1.0 - 1.0/16 + 1.0/64), the larger ALPHA1, the louder speech
#define ALPHA2      0.8125f                         //(1.0/2 + 1.0/4 + 1.0/16)
#define BETA        0.8125f                         //(1.0/2 + 1.0/4 + 1.0/16)
#define GAMMA       0.998046875f                    //(1.0 - 1.0/512)
#define SIGMA       2
//#define NX          0.703125f                       //(1.0/2 +1.0/4 - 1.0/16 + 1.0/64)
#define NX          0.5f                              //2007-01-05
#define NX1         (float)(1.0f-NX)
#define NX_16       FRAC_CONST_16((float)NX)           //(1.0/2 +1.0/4 - 1.0/16 + 1.0/64)
#define NX1_16      FRAC_CONST_16((float)(1.0f-NX))
#define NX32        (NX1_16 << 16)|NX_16
#define MX_16       FRAC_CONST_16((float)0.8)
#define MX1_16       FRAC_CONST_16((float)0.2)   
#define MX32        (MX1_16 << 16)|MX_16     
#define EX          0.8125f                         //(1.0/2 + 1.0/4 + 1.0/16)
#define EX1         (float)(1.0f-EX)
#define SCALING     (float)(1.0f - GAMMA)/(1.0f - BETA)   //(1.0/64)
#else
#define NX_16       16383                          //(1.0/2 +1.0/4 - 1.0/16 + 1.0/64)
#define NX1_16      16384
#define NX32        (NX1_16 << 16)|NX_16

#endif

#define NR_FRAME_SIZE  128
#define log2N     (log2FFT + 1)

struct drft_lookup;

/** NR pre-processor state. */
typedef struct NRState {
   int    frame_size;        /**< Number of samples processed each time */
   int    ps_size;           /**< Number of points in the power spectrum */
   int    nb_adapt;
   int    noise_floor;       /**< Maximum absolute value of background noise */ 

   int   *frame;             /**< Processing frame (2*ps_size) */
   __int64 *ps;              /**< Current power spectrum */
   __int64 *Psmooth;         /**< Smoothed power spectrum */
//   __int64 *Pmin;            /**< Minimum of current noisy speech frame*/
   __int64 *Nestimate;       /**< Noise estimate */
   short *Gs;                  /**< Adjusted gains */
//   float *Sr;                /**< Ratio of noisy speech power spectrum and its local min */

   short   *inbuf;             /**< Input buffer (overlapped analysis) */
   short   *overlap;           /**< Output overlap buffer (for overlap and add) */

   struct drft_lookup *fft_lookup;   /**< Lookup table for the FFT */

} NRState;

/** Creates a new preprocessing state */
NRState *nr_state_init(int NoiseFloor);

/** Noise Reduction main function */
void noise_reduction(NRState *st, nr_int16_t *x, nr_int16_t channels);

extern const short win[2*NR_FRAME_SIZE];

#endif
