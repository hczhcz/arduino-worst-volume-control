const int dataPinA = 4;  // A: pin 1 orange
const int dataPinB = 3;  // A: pin 1 orange
const int dataPinC = 5;  // A: pin 1 orange
const int clrPin = 7; // CLR: pin 9 CLR active low
const int clockPinA = 8; // CLK: pin 8
const int clockPinB = 9;
const int clockPinC = 6;

const int encoderA = 11;
const int encoderB = 12;
const int encoderButton = 10;

int aVal = 0;
int bVal = 0;
int buttonVal = 0;
int previousA = 0;
int previousB = 0;
int previousButton = 0;

int scanN = 0;
int jobN = 0;
int output = 0;

void (*(jobs[]))(bool left, bool right, bool button) = {
    jobNormal,
    jobBalance,
    jobMaximumMinimum,
    jobPowerOfTwo,
    jobSlotMachine,
    jobTirePump,
    jobAngryBird,
    jobSlotMachine2,
    jobCurling,
    job42,
};

// utilities

void clearDisplay() {
    digitalWrite(clrPin, LOW);
}

void resumeDisplay() {
    digitalWrite(clrPin, HIGH);
}

int numToBits(int someNumber) {
    const int numMap[16] = {
        B11111100,
        B01100000,
        B11011010,
        B11110010,
        B01100110,
        B10110110,
        B10111110,
        B11100000,
        B11111110,
        B11110110,
        B11101110, // Hexidecimal A
        B00111110, // Hexidecimal B
        B10011100, // Hexidecimal C or use for Centigrade
        B01111010, // Hexidecimal D
        B10011110, // Hexidecimal E
        B10001110, // Hexidecimal F or use for Fahrenheit
    };

    if (someNumber < 16) {
        return numMap[someNumber];
    } else {
        return B10010010; // Error condition, displays three vertical bars
    }
}

void updateDisplay() {
    shiftOut(dataPinA, clockPinA, LSBFIRST, numToBits(output / 100 % 10));
    shiftOut(dataPinB, clockPinB, LSBFIRST, numToBits(output / 10 % 10));
    shiftOut(dataPinC, clockPinC, LSBFIRST, numToBits(output % 10));
}

// main

void setup() {
    // initialize I/O pins
    pinMode(dataPinA, OUTPUT);
    pinMode(dataPinB, OUTPUT);
    pinMode(dataPinC, OUTPUT);
    pinMode(clrPin, OUTPUT);
    pinMode(clockPinA, OUTPUT);
    pinMode(clockPinB, OUTPUT);
    pinMode(clockPinC, OUTPUT);
    digitalWrite(clrPin, HIGH);

    pinMode(encoderA, INPUT);
    pinMode(encoderB, INPUT);
    pinMode(encoderButton, INPUT_PULLUP);
    previousA = digitalRead(encoderA);
    previousB = digitalRead(encoderB);
    Serial.begin(9600);
}

void loop() {
    aVal = digitalRead(encoderA);
    bVal = digitalRead(encoderB);
    buttonVal = digitalRead(encoderButton);

    if (jobN >= sizeof(jobs) / sizeof(jobs[0])) {
        jobN = 0;
    }

    jobs[jobN](
        aVal != bVal && aVal != previousA,
        aVal != bVal && bVal != previousB,
        previousButton && !buttonVal
    );

    previousA = aVal;
    previousB = bVal;
    previousButton = buttonVal;

    if (output < 0) {
        output = 0;
    }

    if (output > 999) {
        output = 999;
    }

    if ((scanN & 3) == 0) {
        updateDisplay();
    }

    scanN += 1;
    delay(5);
}

// effects

void blink(int n, int deltaT = 500) {
    for (int i = 0; i < n; ++i) {
        clearDisplay();
        delay(deltaT);
        resumeDisplay();
        updateDisplay();
        delay(deltaT);
    }
}

// jobs

void jobNormal(bool left, bool right, bool button) {
    if (button) {
        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left) {
            output -= 5;
        }

        if (right) {
            output += 5;
        }

        if (output > 100) {
            output = 100;
        }
    }
}

void jobBalance(bool left, bool right, bool button) {
    static int status = 1;

    if (button) {
        status = 1;

        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left) {
            status = -1;
        }

        if (right) {
            status = 1;
        }

        if (scanN % 7 == 0) {
            output += status;
        }

        if (output > 100) {
            output = 100;
        }
    }
}

void jobMaximumMinimum(bool left, bool right, bool button) {
    if (button) {
        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left) {
            output = 0;
        }

        if (right) {
            output = 100;
        }
    }
}

void jobPowerOfTwo(bool left, bool right, bool button) {
    static int list[] = {
        1,
        2, 4, 8, 16, 32, 64, 28, 56, 12, 24,
        48, 96, 92, 84, 68, 36, 72, 44, 88, 76,
        52
    };
    static int k = 0;

    if (button) {
        k = 0;

        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left) {
            k -= 1;
        }

        if (right) {
            k += 1;
        }

        if (k < 0) {
            k = 0;
        }

        if (k < 2) {
            output = list[k];
        } else {
            output = list[(k - 2) % 20 + 2];
        }
    }
}

void jobSlotMachine(bool left, bool right, bool button) {
    if (button) {
        blink(3);

        output = 0;
        jobN += 1;
        blink(1);
    } else {
        output += 1;

        if (output > 100) {
            output = 0;
        }
    }
}

void jobTirePump(bool left, bool right, bool button) {
    if (button) {
        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left || right) {
            output += 5;
        } else if (scanN % 3 == 0 || output > 100) {
            output -= 1;
        }
    }
}

void jobAngryBird(bool left, bool right, bool button) {
    static int power = 0;
    static bool play = false;

    if (button) {
        power = 0;
        play = false;

        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (play) {
            if (power) {
                power -= 1;
                output += 1;
            } else {
                play = false;
            }
        } else {
            if (left) {
                power += 5;
                if (power > 100) {
                    power = 100;
                }
                play = false;
                output = 0;
            }

            if (right) {
                play = true;
            }
        }
    }
}

void jobSlotMachine2(bool left, bool right, bool button) {
    static int stage = 0;
    static int head = 0;

    if (button) {
        if (stage == 0) {
            stage = 1;
            head = random(101) / 100;
        } else if (stage == 1) {
            stage = 2;
            head = random(101) / 10;
        } else {
            stage = 0;
            head = 0;
            blink(3);

            output = 0;
            jobN += 1;
            blink(1);
        }
    } else {
        if (stage == 0) {
            output = random(1000);
        } else if (stage == 1) {
            output = random(100) + head * 100;
        } else {
            output = random(10) + head * 10;
        }
    }
}

void jobCurling(bool left, bool right, bool button) {
    static int v = 100;
    static int effect = 0;

    if (button) {
        v = 100;
        effect = 0;

        output = 0;
        jobN += 1;
        blink(1);
    } else {
        if (left || right) {
            effect = 1;
        }

        if (scanN % 7 == 0) {
            if (effect) {
                effect -= 1;
            } else if (v) {
                v -= 1;
            }

            if (v && ((scanN * 21) & 255) < v) {
                output += 1;
            }
        }

        if (output > 100) {
            output = 100;
        }
    }
}

void job42(bool left, bool right, bool button) {
    if (button) {
        output = 0;
        jobN += 1;
        blink(1);
    } else {
        output = 42;
    }
}
