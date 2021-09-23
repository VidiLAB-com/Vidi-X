typedef enum fft_dir {
  FFT_FORWARD,    /* kernel uses "-1" sign */
  FFT_INVERSE     /* kernel uses "+1" sign */
} fft_dir;

void window (float complex *data, unsigned int log2_N, byte windowType, fft_dir direction) {
  unsigned int N = 1 << log2_N;
  unsigned int Nd2 = N >> 1;
  for (int i = 0; i < Nd2; i++) {
    double indexMinusOne = double(i);
    double ratio = (indexMinusOne / (N - 1));
    double weighingFactor = 1.0;
    // Compute and record weighting factor
    switch (windowType) {
      case RECTANGLE: // rectangle (box car)
        weighingFactor = 1.0;
        break;
      case HAMMING: // hamming
        weighingFactor = 0.54 - (0.46 * cos(2 * PI * ratio));
        break;
      case HANN: // hann
        weighingFactor = 0.54 * (1.0 - cos(2 * PI * ratio));
        break;
      case TRIANGLE: // triangle (Bartlett)
        weighingFactor = 1.0 - ((2.0 * abs(indexMinusOne - ((N - 1) / 2.0))) / (N - 1));
        break;
      case NUTTALL: // nuttall
        weighingFactor = 0.355768 - (0.487396 * (cos(2 * PI * ratio))) + (0.144232 * (cos(4 * PI * ratio))) - (0.012604 * (cos(6 * PI * ratio)));
        break;
      case BLACKMAN: // blackman
        weighingFactor = 0.42323 - (0.49755 * (cos(2 * PI * ratio))) + (0.07922 * (cos(4 * PI * ratio)));
        break;
      case BLACKMAN_NUTTALL: // blackman nuttall
        weighingFactor = 0.3635819 - (0.4891775 * (cos(2 * PI * ratio))) + (0.1365995 * (cos(4 * PI * ratio))) - (0.0106411 * (cos(6 * PI * ratio)));
        break;
      case BLACKMAN_HARRIS: // blackman harris
        weighingFactor = 0.35875 - (0.48829 * (cos(2 * PI * ratio))) + (0.14128 * (cos(4 * PI * ratio))) - (0.01168 * (cos(6 * PI * ratio)));
        break;
      case FLT_TOP: // flat top
        weighingFactor = 0.2810639 - (0.5208972 * cos(2 * PI * ratio)) + (0.1980399 * cos(4 * PI * ratio));
        break;
      case WELCH: // welch
        weighingFactor = 1.0 - sq((indexMinusOne - (N - 1) / 2.0) / ((N - 1) / 2.0));
        break;
    }
    if (direction == FFT_FORWARD) {
      data[i] *= weighingFactor;
      data[N - (i + 1)] *= weighingFactor;
    }
    else {
      data[i] /= weighingFactor;
      data[N - (i + 1)] /= weighingFactor;
    }
  }
}

void ffti_shuffle_f(float complex *data, unsigned int log2_N)
{
  /*
     Basic Bit-Reversal Scheme:

     The incrementing pattern operations used here correspond
     to the logic operations of a synchronous counter.

     Incrementing a binary number simply flips a sequence of
     least-significant bits, for example from 0111 to 1000.
     So in order to compute the next bit-reversed index, we
     have to flip a sequence of most-significant bits.
  */

  unsigned int N = 1 << log2_N;   /* N */
  unsigned int Nd2 = N >> 1;      /* N/2 = number range midpoint */
  unsigned int Nm1 = N - 1;       /* N-1 = digit mask */
  unsigned int i;                 /* index for array elements */
  unsigned int j;                 /* index for next element swap location */

  for (i = 0, j = 0; i < N; i++) {
    if (j > i) {
      float complex tmp = data[i];
      data[i] = data[j];
      data[j] = tmp;
    }

    /*
       Find least significant zero bit
    */

    unsigned int lszb = ~i & (i + 1);

    /*
       Use division to bit-reverse the single bit so that we now have
       the most significant zero bit

       N = 2^r = 2^(m+1)
       Nd2 = N/2 = 2^m
       if lszb = 2^k, where k is within the range of 0...m, then
           mszb = Nd2 / lszb
                = 2^m / 2^k
                = 2^(m-k)
                = bit-reversed value of lszb
    */

    unsigned int mszb = Nd2 / lszb;

    /*
       Toggle bits with bit-reverse mask
    */

    unsigned int bits = Nm1 & ~(mszb - 1);
    j ^= bits;
  }
}



void ffti_evaluate_f(float complex *data, unsigned int log2_N, fft_dir direction)
{
  /*
     In-place FFT butterfly algorithm

     input:
         A[] = array of N shuffled complex values where N is a power of 2
     output:
         A[] = the DFT of input A[]

     for r = 1 to log2(N)
         m = 2^r
         Wm = exp(−j2π/m)
         for n = 0 to N-1 by m
             Wmk = 1
             for k = 0 to m/2 - 1
                 u = A[n + k]
                 t = Wmk * A[n + k + m/2]
                 A[n + k]       = u + t
                 A[n + k + m/2] = u - t
                 Wmk = Wmk * Wm

     For inverse FFT, use Wm = exp(+j2π/m)
  */

  unsigned int N = 1 << log2_N;
  double theta_2pi = (direction == FFT_FORWARD) ? -PI : PI;
  theta_2pi *= 2;

  for (unsigned int r = 1; r <= log2_N; r++)
  {
    unsigned int m = 1 << r;
    unsigned int md2 = m >> 1;
    double theta = theta_2pi / m;
    double re = cos(theta);
    double im = sin(theta);
    double complex Wm = re + I * im;
    for (unsigned int n = 0; n < N; n += m) {
      double complex Wmk = 1.0f;   /* Use double for precision */
      for (unsigned int k = 0; k < md2; k++) {
        unsigned int i_e = n + k;
        unsigned int i_o = i_e + md2;
        double complex t = Wmk * data[i_o];
        data[i_o] = data[i_e] - t;
        data[i_e] = data[i_e] + t;
        t = Wmk * Wm;
        Wmk = t;
      }
    }
  }
}

void ffti_f(float complex data[], unsigned int log2_N, fft_dir direction)
{
  ffti_shuffle_f(data, log2_N);
  ffti_evaluate_f(data, log2_N, direction);
}

int mean(int i1, int i2) {
  unsigned long sum = 0;
  for (int i = i1; i < i2; i++) sum += abs(creal(data[i]));
  sum /= (i2 - i1);
  return (int)sum;
}

void acquireSound () {
  for (int i = 0; i < SAMPLES; i++) {
    unsigned long chrono = micros();
    data[i] = analogRead(MIC);
    while (micros() - chrono < sampling_period_us); // do nothing
  }
}
void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void displaySpectrum () {
  window (data, LOG2SAMPLE, HAMMING, FFT_FORWARD);
  ffti_f(data, LOG2SAMPLE, FFT_FORWARD);
  int nFreq = SAMPLES / 2;
  int hauteur = display.height();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  const float freqs[7] = {.5, 1, 2, 3, 4, 6, 8};
  for (int i = 0; i < 7; i++) {
    int pos =  FREQ2IND * freqs[i] * 1.9 ;
    if (i == 0) display.drawString(".5", pos - 5, hauteur - 13, 2);
    else display.drawString(String(freqs[i], 0), pos, hauteur - 13, 2);
  }
  display.drawString("kHz", display.width() - 22, hauteur - 13, 2);
  /*
    static uint8_t hue = 0;
    // First slide the led in one direction
    for(int ii = 0; ii < NUM_LEDS; ii++) {
      //hue = peak[i];
      // Set the i'th led to red
      leds[ii] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show();
    }
    // Now go in the other direction.
    for(int ii = (NUM_LEDS)-1; ii >= 0; ii--) {
      // Set the i'th led to red
      //hue = peak[i];
      leds[ii] = CHSV(hue++, 255, 255);
      FastLED.show();
    }
  */

  int decal = 15;
  // Affichage du spectre
  int ampmax = 0;
  int imax = 0;
  for (int i = 2; i < nFreq; i++) {
    int amplitude = (int)creal(data[i]) / COEF;
    if (amplitude > ampmax) {
      ampmax = amplitude;
      imax = i;
    }
    amplitude = min(amplitude, hauteur - decal);
    display.drawFastVLine (i * 2, hauteur - amplitude - decal - 80, amplitude * 2 + 1, TFT_GREEN);
    if (i < 9) {
      //leds[i-2] = CHSV( 100, hauteur - amplitude - decal-80, 255);
      //Serial.println(hauteur - amplitude - decal-80);
      //Serial.println(i-2);
      //leds[i-2] = CRGB( 0, map(hauteur - amplitude - decal-80, 145, 600, 0, 255), 0);
      // leds[i-2] = CHSV( map(hauteur - amplitude - decal-80, 145, 600, 0, 255), 255, 255);
    }
  }

  // Affichage fréquence de plus forte amplitude
  if (ampmax > 0) { //Nulu možemo povećati ukoliko se želimo riješiti reagiranja LEDica na šumove
    display.setTextSize(2);
    display.setTextColor(TFT_BLUE);
    int freqmax = 985.0 * imax * MAX_FREQ / SAMPLES;
    char texte[14];
    sprintf(texte, "%4d Hz: %3d", freqmax, ampmax);
    display.drawString(texte, display.width() - 145, 0, 1);
    //for(int l = 0; l < NUM_LEDS; l++)
    if (ampmax * 2 > NUM_LEDS) {
      ampmax = NUM_LEDS / 2;
    }
    for (int l = 0; l < ampmax * 2; l++)
    {
      leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MAXSVJETLINA);
    }
    for (int l = ampmax * 2; l < NUM_LEDS; l++) {
      //leds[l] = CHSV( 0, 0, 0);
      leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MINSVJETLINA);
    }
    FastLED.show();
  }

  // Affichage et décroissance des valeurs peak
  for (int i = 2; i < nFreq - 4; i = i + 4) {
    int amplitude = (int)creal(data[i]);
    amplitude = max(amplitude, (int)creal(data[i + 1]));
    amplitude = max(amplitude, (int)creal(data[i + 2]));
    amplitude = max(amplitude, (int)creal(data[i + 3]));
    amplitude /= COEF;
    if (amplitude > peak[i]) peak[i] = amplitude;
    if (peak[i] > hauteur - decal) peak[i] = hauteur - decal;
    if (peak[i] > 8) {
      peak[i] -= 2; // Decay the peak
      display.drawFastHLine(i * 2, hauteur - peak[i] - decal - 80, 4, TFT_BROWN);
      //      Serial.println(i);
    }
  }
  /* Slow FADE
    leds[0] = CHSV(hauteur - peak[6] - decal-80, 255, 255);
    leds[1] = CHSV(hauteur - peak[10] - decal-80, 255, 255);
    leds[2] = CHSV(hauteur - peak[14] - decal-80, 255, 255);
    leds[3] = CHSV(hauteur - peak[18] - decal-80, 255, 255);
    leds[4] = CHSV(hauteur - peak[22] - decal-80, 255, 255);
    leds[5] = CHSV(hauteur - peak[26] - decal-80, 255, 255);
    leds[6] = CHSV(hauteur - peak[30] - decal-80, 255, 255);
    FastLED.show();
    fadeall();
  */
}



void rectangleGR (int x0, int w, int h) {
  const uint16_t colors[8] = {TFT_DARKGREEN, TFT_GREEN, TFT_GREENYELLOW, TFT_YELLOW,
                              TFT_GOLD, TFT_ORANGE, TFT_RED, TFT_BROWN
                             };
  const int decal = 15;
  int x = x0 * (w + 2);
  int dh = (display.height() - decal) / 8;
  for (int i = 1; i < 8; i++) {
    if (h > i * dh) {
      int y = display.height() - decal - i * dh;
      display.fillRect(x, y, w, dh, colors[i - 1]);
    } else {
      int y = display.height() - decal - h;
      display.fillRect(x, y, w, h - (i - 1) * dh, colors[i - 1]);
      break;
    }
  }
}

void displaySpectrum2 () {
  window (data, LOG2SAMPLE, HAMMING, FFT_FORWARD);
  ffti_f(data, LOG2SAMPLE, FFT_FORWARD);
  int nFreq = SAMPLES / 2;
  int hauteur = display.height();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  const float freqs[6] = {.5, 1, 1.5, 2, 3, 4};
  for (int i = 0; i < 6; i++) {
    int pos =  FREQ2IND * freqs[i] * 3.8 ;
    if (i == 0) display.drawString(".5", pos - 5, hauteur - 13, 2);
    else if (i == 2) display.drawString("1.5", pos - 5, hauteur - 13, 2);
    else display.drawString(String(freqs[i], 0), pos, hauteur - 13, 2);
  }
  display.drawString("kHz", display.width() - 22, hauteur - 13, 2);

  const int decal = 100;
  // Affichage du spectre
  for (int i = 0; i < nFreq / 2; i++) {
    int amplitude = 0;
    for (int j = 0; j < 8; j++) amplitude += abs((int)creal(data[i * 8 + j + 2]) / COEF);
    amplitude = min(amplitude, hauteur - decal);
    rectangleGR (i, 13, amplitude + 1);
  }
}

void displayAmplitudeBars() {
  const uint16_t colors[8] = {TFT_DARKGREEN, TFT_GREEN, TFT_GREENYELLOW, TFT_YELLOW,
                              TFT_GOLD, TFT_ORANGE, TFT_RED, TFT_BROWN
                             };
  int ampmax = -5000;
  int ampmin =  5000;
  int N = display.width();
  int decal = 15;
  int hauteur = display.height() - decal;
  for (int i = 0; i < N; i++) {
    sound[i] = analogRead(MIC);
    //delayMicroseconds(50);
  }
  display.fillScreen(TFT_BLACK);
  int prevamp = 1;
  for (int i = 0; i < N; i++) {
    uint16_t colour;
    //    colour = TFT_GREEN;
    //    colour = colors[(i / 10) % 8];
    uint16_t red  = 0b11111;
    uint16_t red2 = 0b00001;
    if (i < N / 2) colour = (red - i * (red - red2) * 2 / N) << 11;
    else colour = (i + N / 2) * 32 / N;
    int amplitude = (sound[i] - SILENCE) / COEF2;
    constrain(amplitude, -hauteur / 2, hauteur / 2);
    if (amplitude > ampmax) ampmax = amplitude;
    if (amplitude < ampmin) ampmin = amplitude;
    display.drawFastVLine (i, hauteur / 2 - amplitude + decal + display.height() / 2, abs(amplitude), colour);
    if (amplitude * prevamp < 0 && i > 0)
      display.drawLine(i - 1, hauteur / 2 - prevamp + decal + display.height() / 2, i, hauteur / 2 - amplitude + decal, colour);

    if (i < NUM_LEDS) {
      leds[i] = CHSV( map(sound[i], 0, 4095, 0, 255), 255, 255);
    }

    //Serial.print(amplitude);
    //Serial.print(" h-d ");
    //Serial.println(hauteur - decal);
  }
  FastLED.show();
  // Affichage plus forte amplitude
  display.setTextColor(TFT_BLUE);
  display.setTextSize(2);
  char texte[25];
  sprintf(texte, "Min: %3d, Max: %3d", ampmin, ampmax);
  display.drawString(texte, 10, 0, 1);
  //delay(50);
}

void displayEnvelope(bool erase) {
  static int i = 0;
  if (i == 1 || erase) display.fillScreen(TFT_BLACK);
  const int decal = 15;
  const int hauteur = display.height();
  const int N = display.width();
  static unsigned int maxP2P = 0;
  static unsigned int minP2P = 5000;
  static unsigned int maxP2Pold = 0;
  static unsigned int minP2Pold = 5000;
  static int amplitudeold = 0;

  if (i >= N || erase) {
    i = 0;
    maxP2P = 0;
    minP2P = 5000;
    maxP2Pold = 0;
    minP2Pold = 5000;
  }

  unsigned int signalMax = 0;
  unsigned int signalMin = 4096;
  unsigned long chrono = micros(); // Sample window 10ms
  while (micros() - chrono < 10000ul) {
    int sample = analogRead(MIC) / COEF3;
    if (sample > signalMax) signalMax = sample;
    else if (sample < signalMin) signalMin = sample;
  }
  unsigned int peakToPeak = signalMax - signalMin;
  int amplitude = peakToPeak - SENSITIVITY;
  if (amplitude < 0) amplitude = 0;
  else if (amplitude > 500) amplitude = 500;
  int ampPrev = map(amplitudeold, 0, 500, hauteur - 1, decal);
  int amp     = map(amplitude,    0, 500, hauteur - 1, decal);
  if (amplitude > maxP2P) maxP2P = amplitude;
  if (amplitude < minP2P) minP2P = amplitude;
  if (i > 0) display.drawLine (i - 1, ampPrev, i, amp, TFT_GREEN);
  amplitudeold = amplitude;
  // Affichage plus forte amplitude
  if ((maxP2Pold != maxP2P) || (minP2Pold != minP2P) || (i == 0)) {
    display.fillRect(0, 0, N, 15, TFT_BLACK);
    display.setTextColor(TFT_BLUE);
    char texte[25];
    sprintf(texte, "Min : %3d, Max: %3d", minP2P, maxP2P);
    display.drawString(texte, 0, 0, 1);
  }
  //int sensorValue = (map(freqmax, 0, 4095, 0, NUM_LEDS / 2));
  // print out the value you read:
  //Serial.print(sensorValue);
  //Serial.print(" - s - a14 - ");
  //Serial.println(analogRead(14));
  // print out the value you read:
  amplitude = map(amplitude, 0, hauteur, 1, NUM_LEDS / 2);
  if (amplitude * 2 > NUM_LEDS) {
    amplitude = NUM_LEDS / 2;
  }
  for (int l = 0; l < amplitude; l++)
  {
    //leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MAXSVJETLINA);
    leds[l] = CRGB::Green;
    leds[NUM_LEDS - l - 1] = CRGB::Yellow;
  }
  for (int l = amplitude; l < NUM_LEDS / 2; l++) {
    //leds[l] = CHSV( 0, 0, 0);
    //leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MINSVJETLINA);
    leds[l] = CRGB::Black;
    leds[NUM_LEDS - l - 1] = CRGB::Black;
  }
  FastLED.show();
  delay(11);

  maxP2Pold = maxP2P;
  minP2Pold = minP2P;
  ++i;
}

void displaySpectrum4 () {
  window (data, LOG2SAMPLE, HAMMING, FFT_FORWARD);
  ffti_f(data, LOG2SAMPLE, FFT_FORWARD);
  int nFreq = SAMPLES / 2;
  int hauteur = display.height();
  int decal = 15;
  // Affichage du spectre
  int ampmax = 0;
  int imax = 0;
  for (int i = 2; i < nFreq; i++) {
    int amplitude = (int)creal(data[i]) / COEF;
    if (amplitude > ampmax) {
      ampmax = amplitude;
      imax = i;
    }
    amplitude = min(amplitude, hauteur - decal);
  }

  int freqmax = 985.0 * imax * MAX_FREQ / SAMPLES;

  int sensorValue = (map(freqmax, 0, 4095, 0, NUM_LEDS / 2));
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" - s - a14 - ");
  Serial.println(analogRead(14));
  // print out the value you read:

  if (sensorValue * 2 > NUM_LEDS) {
    sensorValue = NUM_LEDS / 2;
  }
  for (int l = 0; l < sensorValue; l++)
  {
    //leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MAXSVJETLINA);
    leds[l] = CRGB::Red;
    leds[NUM_LEDS - l - 1] = CRGB::Red;
  }
  for (int l = sensorValue; l < NUM_LEDS / 2; l++) {
    //leds[l] = CHSV( 0, 0, 0);
    //leds[l] = CHSV( map(freqmax, 0, BOJA - (l * FAKTOR), MINBOJA, MAXBOJA), 255, MINSVJETLINA);
    leds[l] = CRGB::Black;
    leds[NUM_LEDS - l - 1] = CRGB::Black;
  }
  FastLED.show();
  delay(11);

}
