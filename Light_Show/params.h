/* Windowing type */
#define RECTANGLE 0x00 /* rectangle (Box car) */
#define HAMMING 0x01 /* hamming */
#define HANN 0x02 /* hann */
#define TRIANGLE 0x03 /* triangle (Bartlett) */
#define NUTTALL 0x04 /* nuttall */
#define BLACKMAN 0x05 /* blackman */
#define BLACKMAN_NUTTALL 0x06 /* blackman nuttall */
#define BLACKMAN_HARRIS 0x07 /* blackman harris*/
#define FLT_TOP 0x08 /* flat top */
#define WELCH 0x09 /* welch */

// FFT parameters
#define SAMPLES 256
#define MAX_FREQ 20 // kHz

// Attenuation for display
#define COEF 60
#define COEF2 5
#define COEF3 3
#define SILENCE 1426
//#define SENSITIVITY 30 // decrease for better sensitivity
