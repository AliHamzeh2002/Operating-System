#!/bin/bash

# Arguments for parallel.out and serial.out
arg1=$1
arg2="output.bmp"

# Execute serial.out and measure execution time
echo "Serial:"
start_serial=$(date +%s.%N)
serial/serial.out "$arg1" "$arg2"
end_serial=$(date +%s.%N)
execution_time_serial=$(echo "$end_serial - $start_serial" | bc)

echo "********"

# Execute parallel.out and measure execution time
echo "Parallel:"
start_parallel=$(date +%s.%N)
parallel/parallel.out "$arg1" "$arg2"
end_parallel=$(date +%s.%N)
execution_time_parallel=$(echo "$end_parallel - $start_parallel" | bc)

echo "********"


# Calculate speedup
speedup=$(echo "scale=2; $execution_time_serial / $execution_time_parallel" | bc)

# Print execution time and speedup
echo "Serial Execution Time: $execution_time_serial seconds"
echo "Parallel Execution Time: $execution_time_parallel seconds"

echo "Speedup: $speedup"
