import serial
import csv
import time
import matplotlib.pyplot as plt
from collections import deque

# === USER SETTINGS ===
PORT = "COM4"       # Change this to your STM32 COM port (check Device Manager)
BAUD = 115200
LOG_FILE = "accel_data.csv"
WINDOW_SIZE = 100   # Number of points shown in the live plot

# === SETUP SERIAL ===
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
print(f"Connected to {PORT}")

# === SETUP LOGGING ===
csv_file = open(LOG_FILE, "w", newline="")
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["X_mg", "Y_mg", "Z_mg"])

# === LIVE PLOT SETUP ===
plt.ion()
fig, ax = plt.subplots()
x_vals, y_vals, z_vals = deque(maxlen=WINDOW_SIZE), deque(maxlen=WINDOW_SIZE), deque(maxlen=WINDOW_SIZE)
line_x, = ax.plot([], [], label='X')
line_y, = ax.plot([], [], label='Y')
line_z, = ax.plot([], [], label='Z')
ax.legend()
ax.set_ylim(-2000, 2000)
ax.set_title("Live Accelerometer Data (mg)")

# === READ LOOP ===
try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        if line:
            try:
                x, y, z = map(int, line.split(","))
                csv_writer.writerow([x, y, z])
                csv_file.flush()

                x_vals.append(x)
                y_vals.append(y)
                z_vals.append(z)

                line_x.set_data(range(len(x_vals)), list(x_vals))
                line_y.set_data(range(len(y_vals)), list(y_vals))
                line_z.set_data(range(len(z_vals)), list(z_vals))
                ax.relim()
                ax.autoscale_view()
                plt.pause(0.01)
            except ValueError:
                print("Bad data:", line)
except KeyboardInterrupt:
    print("\nStopping...")

finally:
    ser.close()
    csv_file.close()
    print(f"Data saved to {LOG_FILE}")
