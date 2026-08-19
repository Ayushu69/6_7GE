"""
Terraria-inspired world generator for 6_7GE.
Produces a 200x100 CSV tilemap with:
  - Smooth Perlin-noise surface for rolling hills & valleys
  - Variable dirt depth with stone beneath
  - Cave systems deep underground via 2D Perlin noise
  - Dirt pockets in stone layer

Tile IDs (matching the custom tileset):
  0 = SKY / AIR (rendered, not solid)
  1 = GRASS (surface block)
  2 = DIRT
  3 = ROCK / STONE
"""

import random
import math
import os

# ─── World dimensions (must match Constants.h) ─────────────────────────
COLS = 200
ROWS = 100

# ─── Tile IDs ──────────────────────────────────────────────────────────
SKY   = -1
GRASS = 0
DIRT  = 1
STONE = 2

# ─── Perlin Noise (pure-Python, no dependencies) ──────────────────────

_PERM = list(range(256))

def _fade(t):
    return t * t * t * (t * (t * 6 - 15) + 10)

def _lerp(a, b, t):
    return a + t * (b - a)

def _grad1d(h, x):
    return x if (h & 1) == 0 else -x

def perlin1d(x):
    xi = int(math.floor(x)) & 255
    xf = x - math.floor(x)
    u = _fade(xf)
    return _lerp(_grad1d(_PERM[xi], xf),
                 _grad1d(_PERM[xi + 1], xf - 1), u)

def _grad2d(h, x, y):
    v = x if (h & 1) == 0 else -x
    v += y if (h & 2) == 0 else -y
    return v

def perlin2d(x, y):
    xi = int(math.floor(x)) & 255
    yi = int(math.floor(y)) & 255
    xf = x - math.floor(x)
    yf = y - math.floor(y)
    u = _fade(xf)
    v = _fade(yf)

    aa = _PERM[_PERM[xi]     + yi]
    ab = _PERM[_PERM[xi]     + yi + 1]
    ba = _PERM[_PERM[xi + 1] + yi]
    bb = _PERM[_PERM[xi + 1] + yi + 1]

    x1 = _lerp(_grad2d(aa, xf,     yf),
               _grad2d(ba, xf - 1, yf),     u)
    x2 = _lerp(_grad2d(ab, xf,     yf - 1),
               _grad2d(bb, xf - 1, yf - 1), u)
    return _lerp(x1, x2, v)

def octave_noise1d(x, octaves=4, persistence=0.5, lacunarity=2.0):
    total = 0.0
    amp = 1.0
    freq = 1.0
    max_val = 0.0
    for _ in range(octaves):
        total += perlin1d(x * freq) * amp
        max_val += amp
        amp *= persistence
        freq *= lacunarity
    return total / max_val

def octave_noise2d(x, y, octaves=4, persistence=0.5, lacunarity=2.0):
    total = 0.0
    amp = 1.0
    freq = 1.0
    max_val = 0.0
    for _ in range(octaves):
        total += perlin2d(x * freq, y * freq) * amp
        max_val += amp
        amp *= persistence
        freq *= lacunarity
    return total / max_val


# ═══════════════════════════════════════════════════════════════════════
#  WORLD GENERATION
# ═══════════════════════════════════════════════════════════════════════

# Random seed
seed = random.randint(0, 100000)
random.seed(seed)
print(f"World seed: {seed}")

# Re-shuffle permutation with fixed seed
_PERM = list(range(256))
random.shuffle(_PERM)
_PERM *= 2

# Start with all sky
world = [[SKY] * COLS for _ in range(ROWS)]

# ─── 1) Surface heightmap ─────────────────────────────────────────────
# Surface sits around row 45 out of 100, giving plenty of sky above
# and deep underground below.
BASE_SURFACE = 45
SURFACE_AMPLITUDE = 8  # gentle hills, not crazy spikes

surface = []
for col in range(COLS):
    # Broad rolling hills (low frequency) + subtle texture (higher freq)
    n = octave_noise1d(col * 0.015 + seed, octaves=5, persistence=0.4)
    height = BASE_SURFACE + int(n * SURFACE_AMPLITUDE)
    height = max(35, min(60, height))
    surface.append(height)

# ─── 2) Fill terrain layers ───────────────────────────────────────────
for col in range(COLS):
    s = surface[col]

    # Variable dirt depth using noise (3-6 tiles)
    dirt_noise = abs(perlin1d(col * 0.08 + seed + 500))
    dirt_depth = 3 + int(dirt_noise * 4)
    dirt_depth = max(3, min(6, dirt_depth))

    for row in range(ROWS):
        if row < s:
            world[row][col] = SKY
        elif row == s:
            world[row][col] = GRASS
        elif row <= s + dirt_depth:
            world[row][col] = DIRT
        else:
            world[row][col] = STONE

# ─── 3) Cave generation ───────────────────────────────────────────────
# Use two overlapping Perlin fields. Where both are near zero (narrow
# band), carve air. This creates winding tunnels, not giant holes.
# Caves only appear 8+ tiles below surface to keep the top layer clean.
CAVE_MIN_DEPTH = 8

for row in range(ROWS):
    for col in range(COLS):
        if world[row][col] == SKY or world[row][col] == GRASS:
            continue

        depth = row - surface[min(col, COLS - 1)]
        if depth < CAVE_MIN_DEPTH:
            continue

        # Deeper = slightly more caves
        depth_factor = min(1.0, depth / 30.0)
        threshold = 0.08 + 0.04 * depth_factor  # narrow band for thin caves

        n1 = octave_noise2d(col * 0.06 + seed,
                            row * 0.06, octaves=3, persistence=0.45)
        n2 = octave_noise2d(col * 0.08 + seed + 2000,
                            row * 0.08 + 2000, octaves=3, persistence=0.45)

        if abs(n1) < threshold and abs(n2) < threshold:
            world[row][col] = SKY

# ─── 4) Dirt pockets in stone ──────────────────────────────────────────
# Natural-looking embedded dirt patches deep in stone.
for row in range(ROWS):
    for col in range(COLS):
        if world[row][col] != STONE:
            continue
        n = octave_noise2d(col * 0.1 + seed + 5000,
                           row * 0.1 + 5000, octaves=2)
        if n > 0.45:
            world[row][col] = DIRT

# ─── 5) Re-grass exposed surfaces ─────────────────────────────────────
# After cave carving, any solid tile with sky directly above becomes grass.
for col in range(COLS):
    for row in range(1, ROWS):
        if world[row][col] in (DIRT, STONE) and world[row - 1][col] == SKY:
            world[row][col] = GRASS

# ═══════════════════════════════════════════════════════════════════════
#  WRITE OUTPUT
# ═══════════════════════════════════════════════════════════════════════

os.makedirs("assets", exist_ok=True)

with open("assets/world.csv", "w") as f:
    for row in world:
        f.write(",".join(str(t) for t in row) + "\n")

# Stats
counts = {}
for row in world:
    for t in row:
        counts[t] = counts.get(t, 0) + 1

names = {SKY: "SKY", GRASS: "GRASS", DIRT: "DIRT", STONE: "STONE"}
print(f"world.csv generated! ({COLS}x{ROWS})")
for tid in sorted(counts):
    print(f"  {names.get(tid, str(tid)):>5}: {counts[tid]:>6} tiles")