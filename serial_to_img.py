import serial

ser = serial.Serial('COM6', 115200, timeout=2) # Keeps reading chunks (from port COM6) until no data arrives for 2s (timeout)

filename = r"C:\Users\Mosta\Desktop\ITP\PY_output.png" # modify as required

with open(filename, "wb") as f:
    print("Logging data... waiting for image from STM32")
    total_bytes = 0
    while True:
        data = ser.read(1024)
        if data:
            f.write(data)
            f.flush()
            total_bytes += len(data)
            print(f"Received {len(data)} bytes (Total: {total_bytes})")
        else:
            # No more data for 1s (timeout) → assume image is complete
            break

print(f"\n✅ Image fully received and saved to {filename}")
print(f"Final file size: {total_bytes} bytes")

ser.close()