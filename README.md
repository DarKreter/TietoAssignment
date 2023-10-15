Assignment from Tieto. 

Instruction are included in `Assignment.pdf`.

Build:
---
```
export CC="[gcc/clang]"
mkdir build
cmake -S . -B build
cmake --build . --target firmware
```

Run:
---
```
./build/firmware/firmware

# Check with valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./build/firmware/firmware

# Send SIGTERM(only one process in system with name firmware)
kill -SIGTERM $(ps -aux | grep firmware | awk '{print $2}' | head -n 1)
```