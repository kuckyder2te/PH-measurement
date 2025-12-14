# PH_Measurement

## Links

Important: https://docs.elegantota.pro/getting-started/async-mode

Anleitung: https://github.com/nthnn/PH4502C-Sensor

Video: https://www.google.com/search?q=ph+4502c+sensor+calibration&sca_esv=6345a290e4fcd0a9&sxsrf=AE3TifOizjbvMwXpVi3WNkXea24Qp_ZS7A%3A1765713596479&ei=vKY-aZmDHZ-wi-gP36aLiA4&oq=4502c+ph+sensor+ca&gs_lp=Egxnd3Mtd2l6LXNlcnAiEjQ1MDJjIHBoIHNlbnNvciBjYSoCCAAyBhAAGBYYHjIIEAAYCBgNGB4yBRAAGO8FMgUQABjvBTIFEAAY7wUyBRAAGO8FSNckUJgIWO8UcAF4AZABAJgBTKAB3gGqAQEzuAEByAEA-AEBmAIEoALoAcICChAAGLADGNYEGEfCAggQABgWGAoYHpgDAIgGAZAGCJIHATSgB6gOsgcBM7gH5QHCBwMwLjTIBwaACAA&sclient=gws-wiz-serp#fpstate=ive&vld=cid:4f1b5d03,vid:1ZeW8GIIT6Y,st:150



## Hardware
![PH_sensor](/doc/4502c_picture1.png)
![PH_sensor_wiring](/doc/4502c_picture2.png)


## Step 1: Hardware Offset Calibration (Setting pH 7 = 2.5V)
- Disconnect Probe: Remove the pH electrode from the BNC connector.
- Short BNC: Short the center pin (Po) to the outer shield of the BNC connector using a wire.
- Connect to Arduino: Connect the PH-4502C module to your Arduino (VCC, GND, Analog Pin A0 for Po).
- Upload Code: Use a simple Arduino sketch to read analog values from A0 and print them (or the voltage equivalent) to the Serial Monitor.
- Adjust Pot: Turn the offset potentiometer (the one closest to the BNC connector) until the Serial Monitor reads approximately 2.5V (or the middle of your ADC - range, e.g., 512 for 0-1023). 

## Step 2: Software/Buffer Calibration (Slope Adjustment)
- Rinse Probe: Rinse the pH electrode with distilled water.
- Buffer 1 (pH 7): Place the probe in a pH 7 buffer solution. Adjust your Arduino code or the pH limit potentiometer (the other one) to match the sensor reading to pH 7.0.
- Buffer 2 (pH 4 or 10): Rinse the probe. Place it in a different buffer (e.g., pH 4.01 or 10.00).
- Adjust Code/Pot: Modify your code (or adjust the pH limit pot) to match the actual pH value of the buffer. A two-point calibration (pH 7 and pH 4/10) provides better accuracy.
- Repeat: Rinse and check readings in different buffers to confirm accuracy. 

## Key Considerations
Stabilization: Always wait for readings to stabilize in buffers.
Rinsing: Thorough rinsing with distilled water between buffers is crucial.
Potentiometer: The offset pot sets pH 7 to mid-range (2.5V for 0-5V input); the other pot might adjust the slope if your board has one.
Arduino Code: Libraries like ph4502c_sensor.h simplify calibration, but manual adjustments in code are common for accuracy. 