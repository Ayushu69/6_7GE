import struct, os

sheets = [
    "Idle_Side-Sheet.png",
    "Walk_Side-Sheet.png",
    "Run_Side-Sheet.png",
    "Hit_Side-Sheet.png",
    "Death_Side-Sheet.png",
    "Slice_Side-Sheet.png",
    "Pierce_Side-Sheet.png",
    "Crush_Side-Sheet.png",
    "Collect_Side-Sheet.png",
    "Fishing_Side-Sheet.png",
    "Watering_Side-Sheet.png",
    "Carry_Idle_Side-Sheet.png",
    "Carry_Walk_Side-Sheet.png",
    "Carry_Run_Side-Sheet.png",
]

for s in sheets:
    path = os.path.join("assets", s)
    if not os.path.exists(path):
        print(f"{s}: NOT FOUND")
        continue
    with open(path, "rb") as f:
        data = f.read(24)
    w = struct.unpack(">I", data[16:20])[0]
    h = struct.unpack(">I", data[20:24])[0]
    print(f"{s:40s}  {w:4d}x{h:<4d}  frames={w//64}  (assuming 64px wide)")
