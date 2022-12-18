#include <Arduino.h>

uint8_t sensorNormalCases[] = {
    // Two sensor cases
    0b00011000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b00001100,
    0b00000110,
    // One sensor cases
    0b00000001,
    0b10000000,
    // Three sensor cases
    0b00111000,
    0b01110000,
    0b11100000,
    0b00000111,
    0b00001110,
    0b00011100,
};

uint8_t sensorRightTurnCases[]{
    // 90 degree turn casaes
    0b00011111,
    0b00001111,
    0b00111111,
    // Less than 90 degree turn cases
    //
    // * robot in center
    0b00011001,
    0b00011011,
    // * robot in slightly left
    0b00110001,
    0b00110011,
    0b00110111,
    0b00111001, // 3 sensor
    0b00111011, // 3 sensor
    // * robot in slightly right
    0b00001101,
    0b00011101, // 3 sensor
};

uint8_t sensorLeftTurnCases[]{
    // 90 degree turn cases
    0b11110000,
    0b11111000,
    0b11111100,
    // less than 90 degree turn cases
    // * robot in center
    0b10011000,
    0b11011000,
    // * robot in slightly left
    0b10110000,
    0b10111000, // 3 sensor
    // * robot in slightly right
    0b10001100,
    0b11001100,
    0b10011100, // 3 sensor
    0b11011100, // 3 sensor
};

void count_cases(uint8_t case_array[], int start, int end, uint8_t case_count_arr[])
{
    case_count_arr[0] = 0; // Normal Cases
    case_count_arr[1] = 0; // Left Turn Cases
    case_count_arr[2] = 0; // Right Turn Cases
    case_count_arr[3] = 0; // Full Black Cases
    case_count_arr[4] = 0; // Full white cases
    for (int i = start; i < end; i++)
    {
        for (unsigned int j = 0; j < sizeof(sensorNormalCases) / sizeof(sensorNormalCases[0]); j++)
        {
            if (sensorNormalCases[j] == case_array[i])
                case_count_arr[0]++;
        }
        for (unsigned int j = 0; j < sizeof(sensorLeftTurnCases) / sizeof(sensorLeftTurnCases[0]); j++)
        {
            if (sensorLeftTurnCases[j] == case_array[i])
                case_count_arr[1]++;
        }
        for (unsigned int j = 0; j < sizeof(sensorRightTurnCases) / sizeof(sensorRightTurnCases[0]); j++)
        {
            if (sensorRightTurnCases[j] == case_array[i])
                case_count_arr[2]++;
        }
        if (case_array[i] == 0b11111111)
            case_count_arr[3]++;
        if (case_array[i] == 0b00000000)
            case_count_arr[4]++;
    }
}
