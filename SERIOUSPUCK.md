# SeriousPuck

## Cel projektu

**SeriousPuck** to adapter pozwalający używać Steam Controllera / Steam Controllera 2 z Xbox Series X|S.

Główna idea:

- Steam Controller komunikuje się bezprzewodowo z `nRF52840` przez proprietary 2.4 GHz przy użyciu OpenPuck.
- `nRF52840` przekazuje pełny stan kontrolera do `RP2040` przez UART.
- `RP2040` działa jako urządzenie Xbox/GIP podłączone do konsoli.
- Oryginalny kontroler Xbox jest podłączony do `RP2040` jako urządzenie uwierzytelniające.
- W przyszłości można dołożyć Bluetooth Audio TX, np. przez ESP32, aby wysyłać audio do odbiornika typu FiiO/Snowsky Retro Nano.

---

# Architektura v1

```text
Steam Controller / SC2
        │
        │ proprietary 2.4 GHz
        ▼
   nRF52840 SuperMini
      OpenPuck
        │
        │ UART
        ▼
    RP2040-Zero
   GP2040-CE fork
      │       │
      │       └── PIO USB Host ──► oryginalny pad Xbox
      │                               tylko AUTH
      │
      └── native USB-C ──────────► Xbox Series X|S
```

---

# Zadania poszczególnych MCU

## nRF52840

Odpowiada za:

- radio proprietary 2.4 GHz,
- pairing ze Steam Controllerem,
- odbieranie pełnego stanu kontrolera,
- dekodowanie:
  - przycisków,
  - analogów,
  - triggerów,
  - touchpadów,
  - dotyku touchpadów,
  - kliknięć touchpadów,
  - gyro,
  - akcelerometru,
  - L4/L5/R4/R5,
- odbieranie komend zwrotnych z RP2040,
- haptics / rumble do Steam Controllera.

Bazowy projekt:

- OpenPuck
- https://github.com/safijari/openpuck

Nie chcemy konwertować danych do XInput po stronie nRF.

nRF powinien wysyłać do RP2040 możliwie pełny stan Steam Controllera.

---

## RP2040

Odpowiada za:

- komunikację z Xbox Series X|S,
- GIP / Xbox One Input Mode,
- auth przez oryginalny kontroler Xbox,
- odbieranie danych SC z nRF po UART,
- mapowanie SC → Xbox,
- obsługę:
  - deadzone,
  - sensitivity,
  - curve,
  - gyro → right stick,
  - touchpad → stick / trackball / flick,
  - layers,
  - profiles,
  - dodatkowych akcji L4/L5/R4/R5,
- wysyłanie rumble z Xbox do nRF.

Bazowy projekt:

- GP2040-CE

Dla auth zwykłym kontrolerem Xbox potrzebny jest fork / kod oparty o:

- PR #1684
- `Update to allow xbox series auth from a controller`

---

# Połączenia nRF52840 ↔ RP2040

Planowane połączenie UART:

```text
nRF52840 SuperMini         RP2040-Zero
────────────────────────────────────────
P0.06 / TX   ───────────►  GP5 / UART1 RX
P0.08 / RX   ◄───────────  GP4 / UART1 TX
GND          ────────────  GND
RAW          ◄───────────  5V
```

Uwagi:

- UART pracuje na poziomach 3.3 V.
- Masa obu płytek musi być wspólna.
- nRF jest zasilany z 5 V pochodzących z Xboxa przez RP2040.
- `RAW` na nRF służy jako wejście zasilania.
- Nie podawać 5 V bezpośrednio na `VCC/3V3`.

Docelowy baudrate UART:

```text
1 000 000 baud
```

To wartość startowa; można później zmienić.

---

# USB Host dla kontrolera Xbox AUTH

RP2040-Zero:

```text
RP2040-Zero      USB-A female
─────────────────────────────
5V          ───► VBUS
GND         ───► GND
GP0         ───► D+
GP1         ───► D-
```

USB Host realizowane przez PIO USB.

GP0 / GP1 są wygodną parą sąsiednich GPIO.

Natywny USB-C RP2040 jest używany jako:

```text
RP2040 USB-C ──► Xbox Series X|S
```

---

# Zasilanie

```text
Xbox Series X|S
      │
      │ USB 5V
      ▼
 RP2040-Zero
      │
      ├── 5V ─────► USB-A VBUS ──► Xbox controller
      │
      └── 5V ─────► RAW nRF52840
```

Cały adapter może być zasilany wyłącznie z konsoli.

Do finalnej PCB warto dodać:

- polyfuse / current limiter dla USB-A,
- ESD protection na USB,
- kondensator bulk przy USB Host,
- poprawne prowadzenie D+/D−.

---

# Planowany protokół UART SeriousPuck

Przykładowy pakiet wejściowy:

```c
struct SeriousPuckInput {
    uint32_t buttons;

    int16_t lx;
    int16_t ly;

    int16_t rx;
    int16_t ry;

    uint16_t lt;
    uint16_t rt;

    int16_t lpad_x;
    int16_t lpad_y;
    int16_t lpad_pressure;

    int16_t rpad_x;
    int16_t rpad_y;
    int16_t rpad_pressure;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    uint32_t timestamp;
};
```

Do `buttons` powinny wejść osobne flagi dla:

- A/B/X/Y,
- LB/RB,
- L3/R3,
- Menu/View,
- D-pad,
- L4/L5/R4/R5,
- touchpad click,
- touchpad touch,
- trigger click,
- grip touch,
- innych pól udostępnianych przez OpenPuck.

---

# Kierunek danych

## Input

```text
SC2
 ↓ RF
nRF52840
 ↓ UART
RP2040
 ↓ GIP
Xbox Series X|S
```

## Rumble / haptics

```text
Xbox Series X|S
 ↓ GIP
RP2040
 ↓ UART
nRF52840
 ↓ RF
SC2
```

---

# Mapowanie zaawansowanych funkcji

## Gyro

Xbox nie ma natywnego pola gyro w standardowym wejściu gamepada.

Dlatego:

```text
SC gyro
  ↓
SeriousPuck processing
  ↓
Xbox Right Stick
```

Możliwe opcje:

- gyro always-on,
- gyro while touching right pad,
- gyro while holding L4,
- gyro while aiming,
- osobna sensitivity X/Y,
- smoothing,
- deadzone,
- acceleration.

---

## Touchpady

Przykładowe tryby:

```text
Right Pad:
- right stick
- trackball
- flick stick
- radial menu

Left Pad:
- left stick
- D-pad
- ABXY
- radial menu
- layer selector
```

---

# L4 / L5 / R4 / R5

Nie chcemy ograniczać ich wyłącznie do duplikowania A/B/X/Y.

Plan:

```text
L4 / L5 / R4 / R5
        │
        ├── zwykły Xbox button
        ├── layer switch
        ├── macro / combo
        ├── gyro enable
        ├── touchpad mode switch
        └── keyboard-like extra action
```

Na Xbox Elite Series 2 paddles zwykle mapują istniejące wejścia, ale Xbox wspiera też mapowanie wybranych przycisków Elite na klawisze klawiatury w grach wspierających keyboard.

To jest kierunek do zbadania dla SeriousPuck:

```text
L4 → keyboard M
L5 → keyboard I
R4 → keyboard J
R5 → keyboard K
```

Nie jest to jeszcze gotowa funkcja GP2040-CE.

---

# Co już istnieje

## OpenPuck

Gotowe / dostępne:

- RF do Steam Controllera,
- pairing,
- przyciski,
- analogi,
- triggery,
- touchpady,
- gyro,
- accel,
- tylne przyciski,
- haptics.

## GP2040-CE

Gotowe / dostępne:

- RP2040,
- USB Device,
- PIO USB Host,
- Xbox One Input Mode,
- profile,
- input processing,
- USB Host framework,
- passthrough auth.

Eksperymentalne / wymagające forka:

- auth Xbox Series z normalnego kontrolera Xbox przez PR #1684.

---

# Co trzeba napisać

## nRF

- moduł UART TX:
  - Serializacja `PuckInput`,
  - checksum / CRC,
  - sequence number,
  - timestamp,
  - wysyłanie co ~4 ms.

- moduł UART RX:
  - rumble,
  - haptics,
  - config,
  - LED / status.

## RP2040

Nowy moduł np.:

```text
src/addons/seriouspuck/
```

Przykładowe pliki:

```text
SeriousPuckAddon.cpp
SeriousPuckAddon.h
SeriousPuckProtocol.h
SeriousPuckMapper.cpp
SeriousPuckMapper.h
```

Funkcje:

- UART RX/TX,
- parser pakietów,
- mapowanie na `GamepadState`,
- gyro processing,
- touchpad processing,
- layers,
- profiles,
- rumble forwarding.

---

# Etapy implementacji

## Stage 1 — Xbox AUTH

```text
Xbox pad
   │
USB Host
   ▼
RP2040
   │
USB
   ▼
Xbox Series X
```

Cel:

- potwierdzić auth,
- potwierdzić działanie PR #1684.

---

## Stage 2 — UART

```text
nRF52840
   │
UART
   ▼
RP2040
```

Cel:

- wysłać testowy stan przycisków,
- np. `A`,
- sprawdzić czy Xbox dostaje poprawny input.

---

## Stage 3 — OpenPuck

```text
SC2
 ↓ RF
nRF
 ↓ UART
RP2040
 ↓ USB
XSX
```

Najpierw:

- A/B/X/Y,
- analogi,
- triggery,
- D-pad.

---

## Stage 4 — Advanced input

Dodać:

- L4/L5/R4/R5,
- touchpady,
- gyro,
- touch detection,
- layers,
- profiles.

---

## Stage 5 — Haptics

```text
XSX
 ↓
RP2040
 ↓ UART
nRF
 ↓ RF
SC2
```

---

# Audio — przyszłość

Nie jest częścią v1.

Planowana architektura:

```text
Xbox
 ↓ USB/GIP audio
RP2040
 ↓ PCM / I2S
ESP32 / Bluetooth Audio SoC
 )))
Retro Nano
 ↓
IEM
```

Potrzebne będzie:

- odebranie audio GIP z Xbox,
- konwersja do PCM,
- A2DP Source,
- opcjonalnie HFP dla mikrofonu.

Do prototypu v1:

```text
ESP32 = brak
audio = brak
```

---

# Docelowa PCB

Późniejsza wersja może zawierać na jednej płytce:

```text
┌────────────────────────────────────┐
│            SERIOUSPUCK             │
│                                    │
│ nRF52840          RP2040           │
│    │                │              │
│    └──── UART ──────┘              │
│                                    │
│ USB-A AUTH         USB-C XSX       │
│                                    │
│ ESP32 / BT Audio — future          │
└────────────────────────────────────┘
```

Pierwszy prototyp powinien zostać wykonany na:

- nRF52840 SuperMini,
- RP2040-Zero,
- USB-A female,
- przewodach.

---

# Repo

Proponowana nazwa:

```text
seriouspuck
```

Repozytorium: https://github.com/mkarenko/seriouspuck

Możliwa struktura:

```text
seriouspuck/
├── README.md
├── docs/
│   ├── architecture.md
│   ├── wiring.md
│   ├── protocol.md
│   └── xbox-auth.md
├── firmware/
│   ├── nrf-openpuck/
│   └── rp2040-gp2040ce/
└── hardware/
    ├── prototype/
    └── pcb/
```

---

# Najważniejsze założenie projektu

Nie redukować Steam Controllera do XInput po stronie nRF.

Przesyłać możliwie pełny stan:

```text
SC2 raw/decoded state
        ↓
      UART
        ↓
RP2040 SeriousPuck mapping
        ↓
Xbox/GIP
```

Dzięki temu można zachować:

- gyro,
- trackpady,
- L4/L5/R4/R5,
- pressure,
- touch,
- layers,
- profile-specific behavior.
