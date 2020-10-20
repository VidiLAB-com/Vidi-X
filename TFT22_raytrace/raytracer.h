/*------------------------------------------------------------------------
  Ovaj kod baziran je na Paul Heckbertovom "business card raytracer" kodu.
  Njegov web je: http://www.cs.cmu.edu/~ph/

  Modificirali su ga mnogi prije nas dodajući mu boje/materiale, sfere
  pozicije, itd.
  
  Ovaj kod je komentriran i učinjen čitljivim jer je originalni
  kod napšisan kako bi stao na poleđinu vizit karte
  te je s toga napisan bez kometara i maksimalno je zbijen.
------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
  (Potrebno za ne arduino platforme)
  Niže navedeni 'ifdef' trebali bi raditi automatski no greška u 
  Arduino 1.6.5 IDE ponekada može raditi probleme s toga ih možete
  zakomentirati u nekim sliučajevima
------------------------------------------------------------------------*/
#ifndef PROGMEM
#define PROGMEM
float pgm_read_float(const float *f) {  return *f;  }
#endif

/*------------------------------------------------------------------------
  Varijable s kojima se možete igrati i mijnjati ih...
------------------------------------------------------------------------*/

// Pozicija kamere (pozitivan 'X' je desno, negativan je pomak u lijevo,
// 'Y' je naprijed/nazad, 'Z' je gore/dolje)
static const float cameraX = 3.0;
static const float cameraY = 0.0;
static const float cameraZ = 6.0;

// Koordinate na koje kamera gleda
static const float targetX = -2.0;
static const float targetY = 8.0;
static const float targetZ = 4.0;

// Šaljemo ovoliko zraka po pixelau radi antialiasinga i mekih prijelaza sjena. 
//
// Veči broj definira lijepšu sliku no i duže traje procesiranje
// Varijabla je zakomentirana te postavljena unutar funkcije doRaytrace()
//static const int raysPerPixel = 4;

// Kut gledanja kamere, manji broj => veći zoom, veći broj => širi kut
static const float fov = 0.745f;

// Velićina sjena, veći broj => šire područje sjene
static const float shadowRegion = 0.325f;

/*------------------------------------------------------------------------
  Definiramo materiale i boje
------------------------------------------------------------------------*/
static const float ambient = 0.05f;
static const float materials[] PROGMEM = {
// R,    G,    B,  REFLECTIVITY
  0.8f, 0.8f, 0.8f,   0.5f,    // Ogledalo
  1.0f, 0.0f, 0.0f,   0.3f,    // Red - Crvena
  0.0f, 1.0f, 0.0f,   0.2f,    // Green - Zelena
  0.0f, 0.0f, 0.1f,   0.3f,    // Dark blue - Tamno plava
  0.0f, 0.8f, 0.8f,   0.3f     // Cyan - Svijetlo plava
};

/*------------------------------------------------------------------------
  Definiramo sfere scene
------------------------------------------------------------------------*/
#define NUM_SPHERES 6
static const float spheres[] PROGMEM = {
// center  radius material
   5,15,8,   5,     0, //Kugla ogledalo
   8,10,0,   4,     3,
  -6,12,4,   3,     4,
   1,10,2,   2,     1,
   0, 9,5,   1,     2, //Zelena kugla
 -20,12,3,   9,     0
};

/*------------------------------------------------------------------------
  Definiramo klasu 3D vectora 'class'
------------------------------------------------------------------------*/
struct vec3 {
  float x,y,z;  // Vector ima 3 float attributa.
  vec3(){}
  vec3(float a, float b, float c){x=a;y=b;z=c;}
  vec3 operator+(const vec3& v) const { return vec3(x+v.x,y+v.y,z+v.z);  }    // Vector add
  vec3 operator-(const vec3& v) const { return (*this)+(v*-1);           }    // Vector subtract
  vec3 operator*(float s)       const { return vec3(x*s,y*s,z*s);        }    // Vector scale
  float operator%(const vec3& v)const { return x*v.x+y*v.y+z*v.z;        }    // Scalar product
  vec3 operator^(const vec3& v) const { return vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);  } // Vector product
  vec3 operator!()              const { return *this*(1.0/sqrt(*this%*this));  }  // Normalized vector
  void operator+=(const vec3& v)      { x+=v.x;  y+=v.y;  z+=v.z;        }
  void operator*=(float s)            { x*=s;    y*=s;    z*=s;          }
};

// Zraka 'ray'...
struct ray {
  // Ovo zauzima 24 bytes - možete staviti samo 20 ovakvih na
  // Tiny85 pločicu čak i kada biste mogli koristiti cijeli RAM (što ne možete...)
  vec3 o;  // Izvor zrake
  vec3 d;  // Smjer
};

/*------------------------------------------------------------------------
  Presjek zrake i scene
  Vraća 'SKY' ukoliko neme kontakta a zraka ide prema gore
  Vraća 'FLOOR' ukoliko neme kontakta a zraka ide prema podu
  Vraća index materiala ukoliko je zraka naišla na objekt

  Udaljenost do objekta vraćena je u varijabli 'distance'. 
  Definicija 'normal' objerkta vraćena je u varijabli 'normal'
------------------------------------------------------------------------*/
// Vrijednosti za 'SKY' i 'FLOOR'
static const byte SKY=255;
static const byte FLOOR=254;

/*------------------------------------------------------------------------
  Funkcija trace() zadužena je za vektorske izračune
  zraka
------------------------------------------------------------------------*/
byte trace(const ray& r, float& distance, vec3& normal)
{
  // Pretpostavi da nismo naišli na ništa
  byte result = SKY;

  // Ide li zraka prema dolje?
  float d = -r.o.z/r.d.z;
  if (d > 0.01) {
    // Ukoliko ide, pretpostavi da smo došli do poda.
    result = true;
    distance = d;
    result = FLOOR;
    normal = vec3(0,0,1);
  }

  // Testiraj objekte scene da provjerimo jeli neki na putu zrake
  for (byte i=0; i<NUM_SPHERES; ++i) {
    vec3 oc = r.o;
    // Pročitaj sferu iz 'progmem' memorije, izračunaj vektor 'oc'
    const float *n = spheres+(i*5);
    oc.x -= pgm_read_float(n++);
    oc.y -= pgm_read_float(n++);
    oc.z -= pgm_read_float(n++);
    d = pgm_read_float(n++);  // Radius

    // Test presjeka Ray i sfere
    // Više o matematičkom modleu pronađite ovjde: http://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
    const float b = r.d%oc;        // I.(o-c)
    const float c = (oc%oc)-(d*d); // (o-c).(o-c) - r^2

    // Dali zraka udara u sferu?
    d = (b*b)-c;
    if (d > 0) {
      // Da, izračunaj udaljenost do kontakta
      d = (-b)-sqrt(d);

      // Dali je to najbliži kontakt do sada?
      if ((d > 0.01) and ((result==SKY) or (d<distance))) {
        // Ukoliko je, spremi rezlutat
        distance = d;
        normal = !(oc+r.d*d);
        result = byte(pgm_read_float(n));  // Materijal sfere
      }
    }
  }
  return result;
}
float raise(float p, byte n)
{
  while (n--) {
    p = p*p;
  }
  return p;
}

/*----------------------------------------------------------
  Maleni i brzi generator slučajnog pseudo broja
----------------------------------------------------------*/
byte rngA, rngB, rngC, rngX;
byte randomByte()
{
  ++rngX;                        // X je povećan za jedn svaki put i ostale variable ne utjeći na njega ili on na njih
  rngA = (rngA^rngC^rngX);       // primjetite mješavinu dodavanja XOR
  rngB = (rngB+rngA);            // i korištenje zbrajanja
  rngC = (rngC+(rngB>>1)^rngA);  // bitno je osigurati da bitovi visokog reda iz B mogu utjecati
  return rngC;
}

// slučajni 'float' raspona otprilike [-0.5 ... 0.5]
float randomFloat()
{
  char r = char(randomByte());
  return float(r)/256.0;
}
#define RF randomFloat()
#define SH (RF*shadowRegion)

/*------------------------------------------------------------------------
  Ovo je funkcija koja provjerava scenu i vraća boju pixela zrake
  prema mateijalu na koji je naišla. Nije li zraka naišla na objekt
  vraćamo vrijednosti neba ili poda
------------------------------------------------------------------------*/
float sample(ray& r, vec3& color)
{
  // Dali zraka udara u bilošto na sceni?
  float t;  vec3& n = color;      // RAM is tight, use 'color' as temp workspace
  const byte hit = trace(r,t,n);

  // Jesmo li došli do objekta
  if (hit == SKY) {
    // Generiraj boju neba ako zraka ide prema gore bez kontakta s objektima
    color = vec3(0.1f,0.0f,0.3f) + vec3(.7f,.2f,0.5f)*raise(1.0-r.d.z,2);
    return 0.0;
  }

  // Novi izvor zrake
  r.o += r.d*t;

  // Half vector
  const vec3 half = !(r.d+n*((n%r.d)*-2));

// Vector koji pokazuje prema izvoru svjetlosti na sceni
  r.d = vec3(9+SH, 6+SH,16); // Gdje je svjetlo
  r.d = !(r.d-r.o);          // Normalizirani vector svjetla

  // Lambertov kosinusni zakon
  float d = r.d%n;    // d = Vector svijetlosti % normala površine

  // Provjeri jesmo li u sjeni
  if ((d<0) or (trace(r,t,n)!=SKY)) {
    d = 0;
  }

  // Jesmo li naišli na pod?
  if (hit == FLOOR) {
    // Ukoliko jesmo, generiraj boju poda
    d=(d*0.2)+0.1;   t=d*3.0;  // d=tamno, t=svijetlo
    color = vec3(t,t,t);       // Pretpostavi sivu boju
    t = 1.0f/5.0f;     // Veličina mreže je 5 metara
    int fx = int(ceil(r.o.x*t));
    int fy = int(ceil(r.o.y*t));
    bool dark = ((fx+fy)&1)!=0;  // Svijetla ili tamna boja?
//  bool dark = (((int)(ceil(r.o.x*t)+ceil(r.o.y*t)))&1);  // Svijetla ili tamna boja? -> zakrpa za AVR compiler
    if (dark) { color.y = color.z = d; }        // g+b => tamna boja => 'red'
    return 0;
  }

  // Ukoliko nismo naišli na pod, došli smo do objekta te pročitaj boju materiala iz 'progmem'
  const float *mat = materials+(hit*4);
  color.x = pgm_read_float(mat++);
  color.y = pgm_read_float(mat++);
  color.z = pgm_read_float(mat++);
 
  // Kopiraj vrijednost svjetlosti u varijablu 't'
  t = d;
  if (t > 0) {
    t = raise(r.d%half,5);
  }

  // Izračunajte ukupnu boju pomoću difuznih i zrcalnih komponenata
  color *= d*d+ambient;  // Okolina + difuzija
  color += vec3(t,t,t);  // Zrcalo

  // Moramo pratiti refleksiju zrake... treba promijeniti 'r' za rekurziju
  r.d = half;
  return pgm_read_float(mat);    // Vrati vrijednost refleksije materijala
}

/*------------------------------------------------------------------------
  Raytrace kompletne slike - ovo je glavna funkcija programa
------------------------------------------------------------------------*/
void doRaytrace(int raysPerPixel = 4, int dw = 320, int dh = 240, int q = 1)
{
  // Prati zraku
  int dw2=dw/2;;
  int dh2=dh/2;
  const float pixel =  fov/float(dh2);    // Velićina jednog pixela ekrana

  // Pozicija cilja i kamere
  const vec3 camera = vec3(cameraX,cameraY,cameraZ);
  const vec3 target = vec3(targetX,targetY,targetZ);
  
  unsigned long t = millis();

  for (int y=0; y<dh; y+=q) {
    for (int x=0; x<dw; x+=q) {
      vec3 acc(0,0,0);     // Spremnik boje
      for (int p=raysPerPixel; p--;) {
        ray r;  vec3 temp;
        float xpos = float(x-dw2), ypos=float(dh2-y);
        if (raysPerPixel>1) { xpos+=RF; ypos+=RF; }       // Pretpostavi antialiasing za RPP > 1

        // Izračujnaj zraku kroz pixel
        temp = !(target-camera);
        vec3& right = r.o;   right = !(temp^vec3(0,0,1));
        vec3& up = r.d;      up = !(right^temp);
        r.d = !(temp + ((right*xpos)+(up*ypos))*pixel);  // Smjer zrake
        r.o = camera;                                    // Zraka kreće od kamere
        
        // Sample za scenu, pribroji boju i vrati vrijednost
        vec3& color = temp;
        float reflect1 = sample(r,color);
        acc += color;
        // 'sample()' bi inaće bila rekurzivna funkcija ali nemamo dovoljno RAM-a na Tiny85 ili drugima...
        if (reflect1 > 0) {
          // ...s toga rekurziju radimo ručno
          float reflect2 = sample(r,color);
          acc += color*reflect1;
          if (reflect2 > 0) {
            // ...i to 3 nivoa petlje
            sample(r,color);
            acc += color*(reflect1*reflect2);
          }
        }
      }
      
      // Iscrtaj pixel
      acc = acc*(255.0/float(raysPerPixel));
      int r = acc.x;    if (r>255) { r=255; }
      int g = acc.y;    if (g>255) { g=255; }
      int b = acc.z;    if (b>255) { b=255; }
      if(q==1) display.drawPixel(x, y, RGBTO565(r,g,b));
      else display.fillRect(x, y, q, q, RGBTO565(r,g,b));
    }
// za ESP8266 mora biti postvaljen delay jer ukoliko je ESP8266 predugo u petlji slijedi reboot
#ifdef ESP8266
    delay(1);
#endif
    char buf[100];
    snprintf(buf,100,"%3d%% %3ds", (y+q)*100/dh, (millis()-t)/1000);
    display.setCursor(8,0);
    display.println(buf);
  }
}
