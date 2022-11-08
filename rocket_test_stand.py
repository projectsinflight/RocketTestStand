import matplotlib.pyplot as plt
from math import ceil
THRESHOLD = 0.05
NEWTONS_PER_POUND = 4.4482
NEWTONS_PER_GRAM = 101.97162
MOTOR_CLASSES = [
    ('Micro',0),
    ('1/4A',0.071),
    ('1/2A',0.141),
    ('A',0.281),
    ('B',0.561),
    ('C',1.121),
    ('D',2.251),
    ('E',4.51),
    ('F',8.991),
    ('G',18.01),
    ('H',36.01),
    ('I',71.9),
    ('J',144.01),
    ('K',288.01),
    ('L',576.01),
    ('M',1151.01),
    ('N',2302.01),
    ('O',4604.01)
    ]

start_time = None
max_thrust = 0
total_impulse = 0
motor_class = ""
with open('/Users/Nick/Desktop/LOG.txt') as f:
    lines = f.read().split()
    data = []
    for line in lines:
        x,y = line.split(',')
        x,y = float(x),float(y)
        y = y / NEWTONS_PER_POUND
        if (y > THRESHOLD) and (start_time is None):
            start_time = x
        if (y < THRESHOLD) and (start_time is not None):
            break
        if start_time is not None:
            data.append([x-start_time,y])
            try:
                total_impulse += y * (x-start_time-data[-2][0])
            except:
                pass
            if y > max_thrust:
                max_thrust = y

for val in MOTOR_CLASSES:
    c,imp = val
    if total_impulse > imp:
        motor_class = c
        
propellant_mass = float(input("Propellant Mass (g)?: "))
specific_impulse = total_impulse * 4.4482 / propellant_mass * NEWTONS_PER_GRAM
    
x_lst = [d[0] for d in data]
y_lst = [d[1] for d in data]

fig, ax = plt.subplots(figsize=(10,6))
plt.plot(x_lst,y_lst)
plt.title("Thrust Curve")
plt.ylabel("Thrust (pounds)")
plt.xlabel("Time (seconds)")
plt.grid(which='major', axis='both')
plt.xlim(0,x_lst[-1])
plt.ylim(0,ceil(max_thrust))
text_str = "Propellant Mass: %.1f g" % propellant_mass
text_str += "\nBurn Time: %.2f s" % x_lst[-1]
text_str += "\nMax Thrust: %.2f lb" % max_thrust
text_str += "\nTotal Impulse: %.2f N%ss" % (total_impulse*NEWTONS_PER_POUND, u"\u00B7")
text_str += "\nSpecific Impulse: %.1f s" % specific_impulse
text_str += "\nMotor Class: %s" % motor_class
plt.text(0.02, 0.97, text_str, transform=ax.transAxes,
        verticalalignment='top', bbox=dict(facecolor='white'))
plt.show()
