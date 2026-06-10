import random

COLS = 200
ROWS = 100

# Tile IDs - we'll pick actual tileset IDs later, for now use simple ones
AIR   = 0
GRASS = 858
DIRT  = 521
STONE = 863

world = [[AIR] * COLS for _ in range(ROWS)]

# Generate a simple heightmap with some variation
surface = []
height = 60  # base surface row (out of 100)
for col in range(COLS):
    height += random.randint(-1, 1)
    height = max(50, min(70, height))  # clamp so it doesn't go crazy
    surface.append(height)

for col in range(COLS):
    s = surface[col]
    for row in range(ROWS):
        if row == s:
            world[row][col] = GRASS
        elif row > s and row < s + 5:
            world[row][col] = DIRT
        elif row >= s + 5:
            world[row][col] = STONE

# Write to CSV
import os
os.makedirs("assets", exist_ok=True)

with open("assets/world.csv", "w") as f:
    for row in world:
        f.write(",".join(str(t) for t in row) + "\n")

print("world.csv generated!")