# Sovereign Indian LEO Internet Grid — Engineering Roadmap v2

A pure-C simulation platform and low-cost SDR proof of concept for a regional LEO broadband system serving India.

**Stack:** C11 · CMake · raylib 5.5 · raygui (pure C, header-only) · libpredict (SGP4) · Linux `tc netem` · HackRF One

---

## 0. What changed from v1 (and why)

| v1 assumption                                   | Problem                                                                                                                                                                                                                                                   | v2 approach                                                                                                        |
| ----------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| 24 satellites give continuous coverage of India | A LEO satellite at ~550 km sees a cap roughly 1,000 km wide and goes around the whole Earth every ~95 min. Rough first-order estimate: continuous single coverage of India's band needs **50–70+ satellites at a 10° minimum elevation and 150+ at 25°**. | **Phase 1 is a coverage simulator** that finds the real number before anything else gets built.                    |
| Target band 20°–30°N                            | India runs from ~8°N (Kanyakumari) to ~37°N (Ladakh).                                                                                                                                                                                                     | Cover 8°–37°N. Inclination ≳ 40° plus footprint.                                                                   |
| Domestic gateways are a regulatory moat         | Starlink has already built ~20 gateways in India and agreed to keep user data in India.                                                                                                                                                                   | Domestic gateways are just a requirement. Differentiate on cost, sovereign/defence customers, and regional focus.  |
| "Pure C" with Dear ImGui + rlImGui              | Both are C++.                                                                                                                                                                                                                                             | **raygui**: pure C, single header, made by raylib's author.                                                        |
| Doppler = audio pitch shift                     | RF Doppler is a carrier frequency offset (±650 kHz at 28 GHz). The audible time-stretch is ~2×10⁻⁵.                                                                                                                                                       | **Complex IQ baseband + NCO** Doppler injection, predictive correction, and PLL/FLL tracking.                      |
| Link budget = Tx + gain − FSPL − fade           | No receiver noise, G/T, Eb/N0, or modulation and coding.                                                                                                                                                                                                  | Full C/N₀ → Eb/N₀ → margin chain with ITU-R P.618 rain at a set availability target.                               |
| `nanosleep()` proxy                             | Blocks every packet behind the current one.                                                                                                                                                                                                               | Simulator drives **`tc netem`** (delay plus Gilbert-Elliott burst loss), with an optional `epoll`/`timerfd` proxy. |
| HackRF retunes its frequency on the fly         | Causes PLL relock glitches, and HackRF can't reach Ka/Ku band (max 6 GHz).                                                                                                                                                                                | Keep the radio's tuning fixed and apply Doppler digitally, scaled to 2.4 GHz, over a cabled link.                  |
| ₹4 Cr seed buys satellites + SSLV launch        | Off by an order of magnitude. Also leaves out user terminals and ITU filing.                                                                                                                                                                              | Staged funding: software + ground + tech demo first.                                                               |

---

## 1. Repository layout & build system

```
sovereign-leo/
├── CMakeLists.txt
├── include/
│   ├── leocore.h          # shared types + public API
│   └── raygui.h           # download from github.com/raysan5/raygui (v4.0)
├── src/                   # leocore static library (no graphics)
│   ├── constellation.c    # Walker generator + J2 Keplerian propagator
│   ├── coverage.c         # grid coverage statistics
│   ├── orbit.c            # libpredict wrapper: TLE → look angles
│   ├── linkbudget.c       # C/N0, Eb/N0, ITU-R rain
│   ├── doppler.c          # IQ NCO, compensation, PLL/FLL
│   └── netem.c            # drives tc netem from link state
├── tools/                 # one CLI per project (terminal deliverables)
│   ├── coverage_cli.c
│   ├── track_cli.c
│   ├── link_cli.c
│   ├── doppler_cli.c
│   └── netem_cli.c
├── app/main.c             # raylib + raygui dashboard
├── sdr/                   # HackRF tx/rx (Phase 7)
├── data/                  # TLEs, India grid points, ITU rain tables
└── tests/                 # ctest unit + validation tests
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(SovereignLeoSim C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
add_compile_options(-Wall -Wextra -Wpedantic)

# --- raylib, fetched and built automatically ---
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5)
FetchContent_MakeAvailable(raylib)

find_package(Threads REQUIRED)
find_library(PREDICT_LIB predict)          # libpredict (install from github.com/la1k/libpredict)

# --- core physics library (no graphics dependency) ---
add_library(leocore STATIC
  src/constellation.c src/coverage.c src/orbit.c
  src/linkbudget.c src/doppler.c src/netem.c)
target_include_directories(leocore PUBLIC include)
target_link_libraries(leocore PUBLIC m Threads::Threads ${PREDICT_LIB})

# --- CLI tools ---
foreach(tool coverage_cli track_cli link_cli doppler_cli netem_cli)
  add_executable(${tool} tools/${tool}.c)
  target_link_libraries(${tool} PRIVATE leocore)
endforeach()

# --- dashboard ---
add_executable(leosim app/main.c)
target_link_libraries(leosim PRIVATE leocore raylib)

# --- tests ---
enable_testing()
add_executable(test_core tests/test_core.c)
target_link_libraries(test_core PRIVATE leocore)
add_test(NAME core COMMAND test_core)
```

Build:

```bash
cmake -B build && cmake --build build -j
ctest --test-dir build --output-on-failure
```

> Keep `leocore` free of raylib. Every physics engine should be testable from the terminal, and the dashboard is just a view on top.

---

## 2. Phase plan (solo developer, rough timeline)

| #   | Phase                          | Weeks | Gate to pass before moving on                              |
| --- | ------------------------------ | ----- | ---------------------------------------------------------- |
| 1   | Constellation & coverage sizer | 2–3   | You know the real satellite count for your coverage target |
| 2   | SGP4 tracker                   | 1–2   | Matches Vallado test vectors and a real ISS pass           |
| 3   | Link budget engine             | 2     | FSPL/Eb/N₀ unit tests pass. Monsoon margin quantified      |
| 4   | IQ Doppler emulator            | 2–3   | Residual frequency error < 100 Hz after tracking           |
| 5   | Network emulator               | 1–2   | Image transfer over emulated link with handovers           |
| 6   | Unified dashboard              | 2–3   | Single binary, live 3D + telemetry                         |
| 7   | HackRF hardware loop           | 3–4   | Data lock held through a full emulated pass                |
| 8   | Pitch package                  | 1–2   | Numbers in the deck come from your simulator               |

---

## 3. Phase 1 — Constellation & coverage sizer (do this first)

**Why first:** your business case depends on the satellite count, so find it before anything else.

### Geometry

For altitude _h_, Earth radius _Rₑ_ = 6371 km, and minimum user elevation _ε_:

- Earth central half-angle: `λ = acos(Rₑ·cos ε / (Rₑ + h)) − ε`
- Ground footprint radius ≈ `Rₑ · λ` (λ in radians)
- Slant range at the edge: `d = sqrt((Rₑ+h)² − (Rₑ cos ε)²) − Rₑ sin ε`

Sanity values at h = 550 km:

| ε   | λ      | Footprint radius | Edge slant range |
| --- | ------ | ---------------- | ---------------- |
| 10° | 14.97° | ~1,665 km        | —                |
| 25° | 8.45°  | ~940 km          | ~1,123 km        |

### Implementation

1. **Walker generator** `i:T/P/F` (inclination, total satellites, planes, phasing). Spread the planes' RAAN evenly over 360° and the satellites evenly in each plane, offsetting each plane by `F·360/T`.
2. **Propagator** for hypothetical satellites (no TLEs exist for them): circular Keplerian orbit + J2 secular RAAN drift:
   `dΩ/dt = −1.5 · n · J2 · (Rₑ/a)² · cos i` (J2 = 1.08263×10⁻³)
3. **India grid:** 0.5° lat/lon points inside India's boundary (export once from a shapefile to `data/india_grid.csv`, ~1,300 points).
4. **Simulate** 3 days at 30 s steps. Account for Earth rotation (GMST). For each point at each step, record whether any satellite is above ε.
5. **Metrics per configuration:** % time covered (mean / worst point), maximum gap (min), mean revisit, average satellites in view.
6. **Sweep** T ∈ {24…300}, P, i ∈ {35°…55°}, h ∈ {500…1200 km}. Write the results to CSV.

**Deliverable:** `coverage_cli --sweep > sweep.csv` and a chart of _coverage % vs. satellite count_. This becomes slide 3 of your pitch.

**Decision point:** if 99%+ coverage needs too many satellites for your budget, consider:

- **(a)** start with an intermittent service (IoT, store-and-forward, backhaul at scheduled windows)
- **(b)** higher altitude / MEO
- **(c)** a hybrid with GEO capacity

Phase 1 exists to make this choice with data.

---

## 4. Phase 2 — SGP4 tracker

- Use **libpredict** (C, SGP4/SDP4, observer look angles and Doppler built in). Alternative: port Vallado's reference SGP4.
- Get TLEs from CelesTrak (start with the ISS, then a Starlink or OneWeb satellite).
- Processing chain: TEME position → ECEF (via GMST) → geodetic lat/lon/alt → topocentric **azimuth, elevation, range, range-rate** from your ground station (e.g., Bengaluru 12.97°N, 77.59°E).
- **Range-rate is the key output.** It drives Doppler (Phase 4) and latency (Phase 5).

Sketch (check the exact signatures against your installed `predict.h`):

```c
predict_orbital_elements_t *tle = predict_parse_tle(line1, line2);
predict_observer_t *obs = predict_create_observer("BLR",
        12.97 * M_PI/180.0, 77.59 * M_PI/180.0, 920.0);
struct predict_position pos;
struct predict_observation look;
predict_orbit(tle, &pos, predict_to_julian(time(NULL)));
predict_observe_orbit(obs, &pos, &look);
printf("az %.2f el %.2f range %.1f km rr %.3f km/s\n",
       look.azimuth*180/M_PI, look.elevation*180/M_PI,
       look.range, look.range_rate);
```

**Validation:** Vallado's `SGP4-VER.TLE` test cases (meter-level agreement), plus one real ISS pass compared against Heavens-Above.

**Deliverable:** live terminal stream of lat/lon/az/el/range/range-rate, plus pass prediction (AOS / max elevation / LOS times).

---

## 5. Phase 3 — Link budget engine

### Equations (all in dB)

```
EIRP     = P_tx[dBW] + G_tx[dBi] − L_feeder
FSPL     = 20·log10(d_km) + 20·log10(f_GHz) + 92.45
L_atm    = A_rain + A_gas + A_cloud + A_scint        (ITU-R P.618 / P.676 / P.840)
C/N0     = EIRP − FSPL − L_atm − L_point − L_pol + G/T[dB/K] + 228.6
Eb/N0    = C/N0 − 10·log10(R_b)
Margin   = Eb/N0 − Eb/N0_required(MODCOD) − L_impl
```

### Rain (the monsoon story)

- **P.838**: specific attenuation `γ = k·R^α` (k and α depend on frequency and polarization)
- **P.837**: rain rate R₀.₀₁ for each location (Mumbai, Kolkata, Guwahati are the hardest cases)
- **P.839**: rain height
- **P.618**: path reduction → attenuation exceeded p% of the time (calculate for p = 1%, 0.5%, 0.1%)

### API

```c
typedef struct {
    double tx_power_dbw, tx_gain_dbi, freq_ghz;
    double rx_gt_dbk, bitrate_bps, req_ebn0_db, impl_loss_db;
    double lat_deg, lon_deg, elevation_deg, range_km;
    double availability_pct;          /* e.g. 99.5 */
} LinkInput;

typedef struct {
    double fspl_db, rain_db, gas_db, cn0_dbhz, ebn0_db, margin_db;
} LinkResult;

LinkResult link_budget(const LinkInput *in);
```

### Adaptive coding and modulation (ACM)

Add a DVB-S2X MODCOD table (QPSK 1/4 → 256APSK). Given Eb/N₀, select the highest MODCOD that still has margin. Rain fade then **reduces throughput** gradually instead of dropping the link. This is how real systems survive monsoon.

**Unit tests:**

- FSPL at 550 km, 28 GHz = **176.2 dB**
- FSPL at 1,123 km, 28 GHz = **182.4 dB**
- Compare rain attenuation against the ITU-R P.618 worked examples

**Deliverable:** `link_cli` prints margin + selected MODCOD + throughput for Ku (12 GHz) vs Ka (20/30 GHz), clear sky vs Mumbai monsoon.

---

## 6. Phase 4 — IQ Doppler emulator & corrector

### Physics

- Doppler shift: `f_d(t) = −f_c · ṙ(t) / c`
- Peak at Ka 28 GHz: |ṙ| up to ~7 km/s → **≈ ±650 kHz**
- Peak Doppler rate on an overhead pass: ≈ f_c · v²/(h·c) ≈ **~10 kHz/s**. Your tracking loop must handle this.

### Pipeline (complex float IQ, e.g., 2 Msps)

1. **Source:** generate a QPSK test burst (known preamble + payload bits).
2. **Channel:** a numerically controlled oscillator (NCO) applies the Doppler from Phase 2's range-rate:
   `φ += 2π·f_d(t)/fs;  y[n] = x[n] · e^{jφ}`. Add AWGN at the Eb/N₀ from Phase 3.
3. **Pre-compensation:** the receiver predicts f_d from the TLE and removes it with an opposite NCO.
4. **Residual tracking:** a 2nd-order FLL → Costas PLL locks onto the remaining error (TLE error, clock drift).
5. **Measure:** residual frequency error, bit error rate (BER), lock/unlock events across a 6–10 min pass.

If you need time-scaling (symbol timing drift), use a **Farrow / polyphase fractional resampler**. Never skip samples, which causes aliasing.

**Optional demo:** play the residual tone through speakers (it sweeps and then flattens when the loop locks). Present it as a visualization, not physics.

**Deliverable:** `doppler_cli pass.csv` → CSV of predicted vs. tracked frequency and BER. Plot: raw Doppler S-curve vs. near-flat residual.

---

## 7. Phase 5 — Network emulator

### Latency model (bent-pipe: user → satellite → gateway)

- One-way propagation = `(d_user + d_gateway) / c` → ~4–8 ms at 550 km
- Add processing, queuing, and terrestrial backhaul → **25–50 ms RTT** typical
- **Handover** every few minutes when the serving satellite changes: brief latency jump + possible loss burst

### Implementation (easiest first)

1. **`tc netem` driver** (recommended): the simulator updates delay and loss at 10 Hz:

   ```bash
   sudo tc qdisc add dev lo root netem delay 30ms 2ms loss gemodel 1% 10% 70% 0.1%
   sudo tc qdisc change dev lo root netem delay 34ms 2ms loss gemodel ...
   ```

   Call these from `netem.c` (via `system()` initially, rtnetlink later).
2. **Gilbert-Elliott loss**: a good/bad 2-state model. Map link margin to the bad-state probability so loss arrives in bursts, like real fades.
3. **Optional userspace proxy**: UDP/TCP relay with `epoll` + `timerfd` and a timestamped release queue. One thread, no `nanosleep`.

**Experiments:**

- Image transfer
- `iperf3` TCP throughput (compare CUBIC vs BBR during handovers)
- Video streaming during a monsoon fade

**Deliverable:** a full pass replayed through the network, with throughput/latency graphs and the image arriving intact.

---

## 8. Phase 6 — Unified dashboard (raylib + raygui)

```
┌─────────────────────────────────────┬──────────────────────────────┐
│ 3D VIEW                             │ TELEMETRY (raygui)           │
│ • Earth + India outline             │ • Tx power / freq / G/T      │
│ • Walker constellation + tracks     │ • Weather: clear/overcast/   │
│ • Beams: green/yellow/red by margin │   monsoon (rain rate)        │
│ • Gateways (domestic)               │ • Margin, MODCOD, throughput │
│ • Coverage heatmap toggle           │ • Doppler + residual plot    │
│                                     │ • Latency / loss plot        │
│ Timeline: play / pause / ×60 speed  │ • Satellite count slider     │
└─────────────────────────────────────┴──────────────────────────────┘
```

### app/main.c skeleton

```c
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "leocore.h"

#define KM_PER_UNIT 1000.0f          /* 1 render unit = 1000 km */
#define HIST 240

/* ECEF (z = north) -> raylib (y = up) */
static Vector3 ecef_to_render(double x, double y, double z) {
    return (Vector3){ (float)(x / KM_PER_UNIT), (float)(z / KM_PER_UNIT), (float)(-y / KM_PER_UNIT) };
}

int main(void) {
    InitWindow(1440, 810, "Sovereign LEO Grid - Simulator");
    SetTargetFPS(60);

    Camera3D cam = { .position = {18, 10, 18}, .target = {0, 0, 0},
                     .up = {0, 1, 0}, .fovy = 45, .projection = CAMERA_PERSPECTIVE };

    float tx_dbw = 10.0f, freq_ghz = 28.0f, rain_mmh = 0.0f, sats = 72;
    int weather = 0;
    float margin_hist[HIST] = {0};
    int head = 0;

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_ORBITAL);

        /* --- physics step (leocore) --- */
        static const float rain_by_mode[] = { 0.0f, 5.0f, 60.0f };
        rain_mmh = rain_by_mode[weather];
        LinkInput in = { .tx_power_dbw = tx_dbw, .tx_gain_dbi = 35, .freq_ghz = freq_ghz,
                         .rx_gt_dbk = 10, .bitrate_bps = 50e6, .req_ebn0_db = 4.0,
                         .impl_loss_db = 2.0, .lat_deg = 19.07, .lon_deg = 72.88,
                         .elevation_deg = 40, .range_km = 800, .availability_pct = 99.5 };
        (void)rain_mmh;                                   /* feed into your rain model */
        LinkResult r = link_budget(&in);
        margin_hist[head] = (float)r.margin_db;
        head = (head + 1) % HIST;

        BeginDrawing();
        ClearBackground((Color){ 12, 14, 20, 255 });

        BeginMode3D(cam);
            DrawSphere(Vector3Zero(), 6.371f, (Color){ 30, 80, 160, 255 });
            DrawSphereWires(Vector3Zero(), 6.38f, 24, 24, (Color){ 60, 120, 200, 80 });
            /* for each satellite: DrawSphere(ecef_to_render(...), 0.08f, color by margin); */
        EndMode3D();

        /* --- telemetry panel --- */
        Rectangle p = { 1040, 10, 390, 790 };
        GuiPanel(p, "Telemetry Control");
        GuiSliderBar((Rectangle){ 1150, 50, 200, 20 }, "Tx power dBW", TextFormat("%.1f", tx_dbw), &tx_dbw, 0, 20);
        GuiSliderBar((Rectangle){ 1150, 80, 200, 20 }, "Freq GHz", TextFormat("%.1f", freq_ghz), &freq_ghz, 10, 40);
        GuiSliderBar((Rectangle){ 1150, 110, 200, 20 }, "Satellites", TextFormat("%.0f", sats), &sats, 24, 300);
        GuiComboBox((Rectangle){ 1150, 140, 200, 24 }, "Clear;Overcast;Mumbai monsoon", &weather);

        GuiLabel((Rectangle){ 1060, 180, 360, 20 }, TextFormat("FSPL %.1f dB   Rain %.1f dB", r.fspl_db, r.rain_db));
        GuiLabel((Rectangle){ 1060, 200, 360, 20 }, TextFormat("Eb/N0 %.1f dB   Margin %.1f dB", r.ebn0_db, r.margin_db));

        /* margin history plot, 0..40 dB */
        Rectangle g = { 1060, 230, 350, 100 };
        DrawRectangleLinesEx(g, 1, GRAY);
        for (int i = 1; i < HIST; i++) {
            float a = margin_hist[(head + i - 1) % HIST], b = margin_hist[(head + i) % HIST];
            Vector2 p0 = { g.x + (i - 1) * g.width / HIST, g.y + g.height * (1 - Clamp(a, 0, 40) / 40) };
            Vector2 p1 = { g.x + i * g.width / HIST,       g.y + g.height * (1 - Clamp(b, 0, 40) / 40) };
            DrawLineV(p0, p1, b > 3 ? LIME : (b > 0 ? YELLOW : RED));
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
```

**Beam color rule:**

- Green: margin > 3 dB
- Yellow: 0–3 dB (ACM stepping down)
- Red: < 0 dB (link lost)

---

## 9. Phase 7 — HackRF hardware loop

### Kit

- 2× HackRF One (half-duplex: board A = satellite Tx, board B = ground Rx)
- SMA cables + **30–60 dB of fixed attenuation** (HackRF's documented max Rx input is about −5 dBm)
- Optional: SMA cable linking CLKOUT → CLKIN to share a reference clock
- Antennas only if you move to over-the-air later in a licence-exempt band and stay within WPC rules. Start cabled.

### Approach

- **Keep the RF tuning fixed** (e.g., 2.4 GHz). Doppler is applied **digitally** in the Tx IQ stream by the Phase 4 NCO. Real systems do it this way, and it avoids PLL retune glitches.
- Two Doppler modes:
  - **Scaled:** same fractional shift as Ka-band (±56 kHz at 2.4 GHz)
  - **Absolute:** inject the real ±650 kHz Ka profile (fits easily within a 4–10 Msps stream)
- **libhackrf API:** `hackrf_open`, `hackrf_set_freq`, `hackrf_set_sample_rate`, `hackrf_set_txvga_gain` / `hackrf_set_lna_gain` / `hackrf_set_vga_gain`, `hackrf_start_tx(dev, tx_cb, ctx)`, `hackrf_start_rx(dev, rx_cb, ctx)`. Samples are 8-bit interleaved I/Q.
- **Use a lock-free ring buffer** between the callbacks and your DSP thread. Never run DSP inside the USB callback.

### Experiments

1. **Shared clock vs. free-running.** Two HackRFs can disagree by tens of kHz at 2.4 GHz without a shared reference. Your FLL must handle that too. (It's a good demo.)
2. **Full pass replay:** Phase 2 TLE → Doppler profile → Tx. Rx maintains lock, BER logged.
3. **End-to-end:** carry the Phase 5 image file through the RF link.

**Deliverable:** a video of the dashboard showing the Doppler S-curve, the residual staying flat, and the image arriving, with real radios on the desk.

---

## 10. Phase 8 — Pitch package

### Corrected thesis

1. **Regional focus, sized with data.** "Our simulator shows N satellites at i°/h km deliver X% coverage of India, with Y-minute max gaps." Show the coverage-vs-count curve and the phased path (intermittent → continuous).
2. **Monsoon-resilient design.** Quantified Ka-band rain fade for Indian cities, plus ACM and Ku fallback. Few competitors talk about this specifically.
3. **Sovereign by design, not by regulation.** Domestic gateways are table stakes (Starlink complies too). The angle is Indian ownership, defence/government customers, local manufacturing of user terminals, and pricing for Tier-2/3 and rural India.
4. **Honest competitive slide:** Starlink, Eutelsat OneWeb, Jio-SES, Amazon Kuiper. Where do you win: price, segment, government contracts, or as a component/software supplier to them?

### Regulatory path (verify current rules before the pitch)

- IN-SPACe authorization (Indian Space Policy 2023)
- DoT satellite service authorization under the Telecommunications Act 2023
- Spectrum assignment
- ITU filing via the Indian administration (multi-year lead time; start early)

### Staged funding (get real quotes from NSIL/ISRO and rideshare brokers)

- **Pre-seed:** simulator, ground station, HackRF demo, coverage study, first customer LOIs
- **Seed:** 1–2 tech-demo satellites (CubeSat or hosted payload) + one domestic gateway prototype
- **Series A+:** constellation build, user terminal, launches

Budget lines most early decks forget:

- The **user terminal** (phased array)
- Gateway sites
- Spectrum and ITU fees
- Insurance
- Ground software operations

### Demo build

- Ship `leosim` as one static Linux binary + Windows build (raylib cross-compiles cleanly).
- 3-minute demo script: coverage sweep → monsoon fade with ACM → Doppler lock → image over RF.

---

## 11. Validation checklist

- [ ] Coverage: footprint formula matches table in §3; sweep results reproducible
- [ ] SGP4 matches Vallado SGP4-VER.TLE; ISS pass times within seconds of Heavens-Above
- [ ] FSPL tests: 176.2 dB (550 km, 28 GHz), 182.4 dB (1,123 km, 28 GHz)
- [ ] Rain attenuation matches ITU-R P.618 examples
- [ ] Doppler peak ≈ ±650 kHz at 28 GHz; residual < 100 Hz after tracking
- [ ] netem latency trace matches computed slant ranges
- [ ] HackRF loop holds lock through a full pass with free-running clocks
- [ ] Every number in the pitch deck traces back to a simulator output

---

## 12. References

- Vallado et al., _Revisiting Spacetrack Report #3_ (SGP4 reference + test vectors)
- libpredict: github.com/la1k/libpredict
- CelesTrak TLEs: celestrak.org
- ITU-R P.618, P.676, P.837, P.838, P.839, P.840
- ETSI EN 302 307-2 (DVB-S2X MODCODs)
- Walker, _Satellite Constellations_ (1984), and Wertz, _Space Mission Analysis and Design_ (coverage geometry)
- raylib: raylib.com · raygui: github.com/raysan5/raygui
- HackRF docs: hackrf.readthedocs.io
- Linux `tc-netem(8)` man page (gemodel loss)
