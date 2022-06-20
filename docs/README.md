# Thunder LFR OS docs

Thunder is intended to be an extensible OS for controlling a line follower robot

## EEPROM

EEPROM memory diagram:

| Memory Location | Variable   |
| --------------- | ---------- |
| 0               | Motorspeed |
| 4               | P          |
| 8               | I          |
| 12              | D          |

## Todo

1. button.cpp optim. **Done**
2. continuous reading in display.cpp **Done**
3. portRead **Done**
4. invert implementation **Done**
5. SONAR
6. SERVO
7. Case detection copy-paste
8. PWM frequency modification

## Mega Pin Mapping

- 22 Down_btn
- 24 right_btn PINC3
- 28 select_btn PINC1
- 38 left_btn PIND7
- 30 up_btn
- 5 STBY
- 3 PWMA (changed to 4)
- 2 AIN1
- 4 AIN2 (changed to 3)
- 13 PWMB
- 6 BIN1
- 7 BIN2
- rightSonar(trig, echo) = (52, 53)
- leftSonar(trig, echo) = (49, 48)
- frontSonar(trig, echo) = (50, 51)