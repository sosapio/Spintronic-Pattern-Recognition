import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lfilter

# Parameters
dt = 0.001               # Time step
time = np.arange(0, 1, dt)  # Time array
freq = 180
input_voltage = np.sin(2 * np.pi * freq * time)  # sinusoidal voltage

# Schmitt Trigger Parameters
v_high = 5.0           # High output level
v_low = -5.0           # Low output level
v_th_up = 0.5          # Upper threshold
v_th_down = -0.5       # Lower threshold

# Schmitt Trigger Function
def schmitt_trigger(v_in, prev_output):
    if v_in > v_th_up:
        return v_high
    elif v_in < v_th_down:
        return v_low
    else:
        return prev_output

# Initialize Schmitt Trigger Output
schmitt_output = np.zeros_like(input_voltage)
prev_output = 0

# Calculate Schmitt Trigger Output
for i in range(len(input_voltage)):
    schmitt_output[i] = schmitt_trigger(input_voltage[i], prev_output)
    prev_output = schmitt_output[i]

# First-Order Filter Parameters
R1 = 100000    # Resistance in Ohms
C1 = 1e-6    # Capacitance in Farads
tau1 = R1 * C1  # Time constant

# Second-Order Filter Parameters
R2 = 100000    # Resistance in Ohms
C2 = 100e-9    # Capacitance in Farads
tau2 = R2 * C2  # Time constant

# High-Pass Filter Coefficients
def high_pass_coefficients(R, C, dt):
    alpha = R * C / (R * C + dt)
    b = [1, -1]
    a = [1, -alpha]
    return b, a

# First High-Pass Filter Coefficients
b1, a1 = high_pass_coefficients(R1, C1, dt)

# Second High-Pass Filter Coefficients
b2, a2 = high_pass_coefficients(R2, C2, dt)


# First Filter Output
filtered_output1 = lfilter(b1, a1, schmitt_output)

# Second Filter Output
filtered_output2 = lfilter(b2, a2, filtered_output1)

# Sense Capacitor Parameters
C_sense = 0.01e-12  # Sense capacitance in Farads-

# Integrating the filtered signal (voltage across the sense capacitor)
sense_voltage = np.zeros_like(filtered_output2)
for i in range(1, len(filtered_output2)):
    sense_voltage[i] = sense_voltage[i - 1] + (filtered_output2[i] * dt) / C_sense

# Plotting the Results
plt.figure(figsize=(12, 8))

plt.subplot(4, 1, 1)
plt.plot(time, input_voltage, label='Input Voltage (V)')
plt.title('Input Voltage')
plt.xlabel('Time (s)')
plt.ylabel('Voltage (V)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 2)
plt.plot(time, schmitt_output, label='Schmitt Trigger Output (V)')
plt.title('Schmitt Trigger Output')
plt.xlabel('Time (s)')
plt.ylabel('Voltage (V)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 3)
plt.plot(time, filtered_output1, label='First Filter Output (V)')
plt.title('First Filter Output')
plt.xlabel('Time (s)')
plt.ylabel('Voltage (V)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 4)
plt.plot(time, filtered_output2, label='Second Filter Output (V)')
plt.title('Second Filter Output (Input to Sense Capacitor)')
plt.xlabel('Time (s)')
plt.ylabel('Voltage (V)')
plt.grid()
plt.legend()

plt.tight_layout()
plt.show()

# Plotting Hysteresis Loops
plt.figure(figsize=(12, 6))

plt.subplot(1, 2, 1)
plt.plot(input_voltage, filtered_output1, label='Hysteresis Loop (1st Filter)')
plt.title('Hysteresis Loop - Input vs. 1st Filter Output')
plt.xlabel('Input Voltage (V)')
plt.ylabel('First Filter Output (V)')
plt.grid()
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(input_voltage, filtered_output2, label='Hysteresis Loop (2nd Filter)')
plt.title('Hysteresis Loop - Input vs. 2nd Filter Output')
plt.xlabel('Input Voltage (V)')
plt.ylabel('Second Filter Output (V)')
plt.grid()
plt.legend()

plt.tight_layout()
plt.show()
