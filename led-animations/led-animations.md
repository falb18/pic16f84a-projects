# LED animations

The microcontroller runs different LED animations. The user can change to the next animation when pressing the push button.

## Compile project
```
make
```

## Flash program
```
# Flash the program manually:
pk2cmd -P PIC16F84A -X -M -R -F bin/led-animations.hex

# Or instead you can use the make file target:
make flash
```
## References
- https://github.com/cheavornkh/Khmer-Learn-Microcontroller/blob/master/LED%20Flashing%204%20Mode/code.c
- [YouTube: 12 Channel LED Chaser PIC16F84A](https://www.youtube.com/watch?v=DVMx4DwvFEA)
- [YouTube: Led Animation 12 Chanel LED Chaser PIC16F84A Programmer](https://www.youtube.com/watch?v=EnD4fRhWYw0)