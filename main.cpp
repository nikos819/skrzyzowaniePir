
const int zielone1 = 2;
const int czerwone1 = 3;

const int zielone2 = 4;
const int czerwone2 = 5;

const int zielone3 = 6;
const int czerwone3 = 7;

const int zielone4 = 8;
const int czerwone4 = 9;

// Czujniki PIR
const int pir1 = A1;
const int pir2 = A2;
const int pir3 = A3;
const int pir4 = A4;

// Czas œwiecenia zielonego
const unsigned long czasZielonego = 5000;

// Tablice pinów
const int zielone[4] = {
  zielone1, zielone2, zielone3, zielone4
};

const int czerwone[4] = {
  czerwone1, czerwone2, czerwone3, czerwone4
};

const int pir[4] = {
  pir1, pir2, pir3, pir4
};

// Kolejka FIFO
int kolejka[4];
int poczatek = 0;
int koniec = 0;
int liczbaWKolejce = 0;

// Stany czujników
bool poprzedniRuch[4] = {false, false, false, false};

// Aktualnie obs³ugiwany sygnalizator
int aktywneSwiatlo = -1;
unsigned long czasStartu = 0;

void wszystkieCzerwone() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(zielone[i], LOW);
    digitalWrite(czerwone[i], HIGH);
  }
}

bool juzZgloszony(int nr) {
  if (aktywneSwiatlo == nr) {
    return true;
  }

  for (int i = 0; i < liczbaWKolejce; i++) {
    int indeks = (poczatek + i) % 4;

    if (kolejka[indeks] == nr) {
      return true;
    }
  }

  return false;
}

void dodajDoKolejki(int nr) {
  if (juzZgloszony(nr)) {
    return;
  }

  if (liczbaWKolejce >= 4) {
    return;
  }

  kolejka[koniec] = nr;
  koniec = (koniec + 1) % 4;
  liczbaWKolejce++;
}

int pobierzZKolejki() {
  if (liczbaWKolejce == 0) {
    return -1;
  }

  int nr = kolejka[poczatek];

  poczatek = (poczatek + 1) % 4;
  liczbaWKolejce--;

  return nr;
}

void wlaczZielone(int nr) {
  wszystkieCzerwone();

  digitalWrite(czerwone[nr], LOW);
  digitalWrite(zielone[nr], HIGH);

  aktywneSwiatlo = nr;
  czasStartu = millis();
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(zielone[i], OUTPUT);
    pinMode(czerwone[i], OUTPUT);
    pinMode(pir[i], INPUT);
  }

  wszystkieCzerwone();

  // Zapamiêtaj pocz¹tkowe stany czujników
  for (int i = 0; i < 4; i++) {
    poprzedniRuch[i] = digitalRead(pir[i]) == HIGH;
  }
}

void loop() {
  // Wykrywanie nowych zg³oszeñ
  for (int i = 0; i < 4; i++) {
    bool ruch = digitalRead(pir[i]) == HIGH;

    // Dodaj zg³oszenie tylko przy zmianie LOW na HIGH
    if (ruch && !poprzedniRuch[i]) {
      dodajDoKolejki(i);
    }

    poprzedniRuch[i] = ruch;
  }

  // Wy³¹cz zielone po 5 sekundach
  if (aktywneSwiatlo != -1 &&
      millis() - czasStartu >= czasZielonego) {

    digitalWrite(zielone[aktywneSwiatlo], LOW);
    digitalWrite(czerwone[aktywneSwiatlo], HIGH);

    aktywneSwiatlo = -1;
  }

  // W³¹cz nastêpny sygnalizator zgodnie z kolejnoœci¹ zg³oszeñ
  if (aktywneSwiatlo == -1 && liczbaWKolejce > 0) {
    int nr = pobierzZKolejki();

    if (nr != -1) {
      wlaczZielone(nr);
    }
  }
}
