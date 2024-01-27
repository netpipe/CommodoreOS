import math

MAX_SINE_VALUE = 65535

for i in range(361):
    angle_rad = math.radians(i)
    sine_val = math.sin(angle_rad)
    print(int(sine_val * MAX_SINE_VALUE) ) 
