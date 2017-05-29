// Example usage for MeterColumn library by Jim Wheaton.

#include "MeterColumn.h"

// Initialize objects from the lib
MeterColumn meterColumn;

void setup() {
    // Call functions on initialized library objects that require hardware
    meterColumn.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    meterColumn.process();
}
