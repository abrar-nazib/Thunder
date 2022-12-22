#include <Arduino.h>
#include "case_detection.h"

/*--------- Global variables --------------*/
uint8_t sensorReadingArray[100]; // for accessing memory as an array
extern struct Memory sensorMemory;

/*--------- External Global Variables -----*/

/*------- External Functions --------------*/
extern void memoryGetArray(struct Memory *m, uint8_t arr[]);
extern void displayCaseDetector(uint8_t case_count_arr[]); // from display to display case detection data
extern void Stop(double del);
uint8_t sensorNormalCases[] = {
    // Two sensor cases
    0b00011000,
    0b00110000,
    0b01100000,
    0b00001100,
    0b00000110,
    // One sensor cases

    // Three sensor cases
    0b00111000,
    0b01110000,
    0b11100000,
    0b00000111,
    0b00001110,
    0b00011100,
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
uint8_t sensorDangerLeft[] = {
    0b11100000,
    0b11000000,
    0b10000000,
};

uint8_t sensorRightTurnCases[]{
    // 90 degree turn casaes
    0b00001111,
    0b00011111,
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
    // Special case for handling triangle
    0b01111110,
    0b00111100,
    0b01100110,
};

uint8_t sensorDangerRight[] =
    {
        0b00000001,
        0b00000011,
        0b00000111,
};

void count_cases(uint8_t case_count_arr[], int start, int end)
{
    // Update the memory with latest data to count cases from it
    memoryGetArray(&sensorMemory, sensorReadingArray);

    // Clear case_count_arr from previous values
    case_count_arr[0] = 0; // Normal Cases
    case_count_arr[1] = 0; // Left Turn Cases
    case_count_arr[2] = 0; // Danger Left Cases
    case_count_arr[3] = 0; // Right Turn Cases
    case_count_arr[4] = 0; // Danger Right Cases
    case_count_arr[5] = 0; // Full Black Cases
    case_count_arr[6] = 0; // Full white cases

    // Enumerate cases
    for (int i = start; i < end; i++)
    {
        for (unsigned int j = 0; j < sizeof(sensorNormalCases) / sizeof(sensorNormalCases[0]); j++)
        {
            if (sensorReadingArray[i] == sensorNormalCases[j])
                case_count_arr[0]++; // Normal cases
        }
        for (unsigned int j = 0; j < sizeof(sensorLeftTurnCases) / sizeof(sensorLeftTurnCases[0]); j++)
        {
            if (sensorReadingArray[i] == sensorLeftTurnCases[j])
                case_count_arr[1]++; // Left turn cases
        }
        for (unsigned int j = 0; j < sizeof(sensorDangerLeft) / sizeof(sensorDangerLeft[0]); j++)
        {
            if (sensorReadingArray[i] == sensorDangerLeft[j])
                case_count_arr[2]++; // Danger Left
        }
        for (unsigned int j = 0; j < sizeof(sensorRightTurnCases) / sizeof(sensorRightTurnCases[0]); j++)
        {
            if (sensorRightTurnCases[j] == sensorReadingArray[i])
                case_count_arr[3]++; // Right turn cases
        }

        for (unsigned int j = 0; j < sizeof(sensorDangerRight) / sizeof(sensorDangerRight[0]); j++)
        {
            if (sensorDangerRight[j] == sensorReadingArray[i])
                case_count_arr[4]++; // Danger Right
        }
        if (sensorReadingArray[i] == 0b11111111)
            case_count_arr[5]++; // Black cases
        if (sensorReadingArray[i] == 0b00000000)
            case_count_arr[6]++; // White cases
    }
}

String detect_case()
{
    // Check the previous situation before unusual things happened
    uint8_t case_count_arr_before[7];
    uint8_t case_count_arr_after[7];
    count_cases(case_count_arr_before, 50, 100);
    count_cases(case_count_arr_after, 0, 50);

    //---------------- uncomment for debugging ---------------
    Stop(100);
    displayCaseDetector(case_count_arr_before);
    delay(5000);
    displayCaseDetector(case_count_arr_after);
    delay(5000);
    //--------------------------------------------------------

    // Detection of PID Failure in Left Side
    if (case_count_arr_after[5] > 7 && case_count_arr_after[0] > 10 && case_count_arr_after[6] < 5)
    {
        // 6 -> white cases
        // 5 -> black cases
        // 0 -> normal cases
        return "+"; // T Junction
    }
    if (case_count_arr_after[5] > 7 && case_count_arr_after[6] > 10)
    {
        return "T";
    }
    if (case_count_arr_after[0] > 15 && case_count_arr_after[3] > 5)
    {
        return "TR";
    }
    if (case_count_arr_after[0] > 15 && case_count_arr_after[1] > 5)
    {
        return "TL";
    }
    if ((case_count_arr_before[2] + case_count_arr_after[2]) > 5 && case_count_arr_after[6] > 5)
    {
        // 2 -> danger left cases
        // 6 -> white cases
        return "P_L"; // PID-Failure in left side
    }
    if ((case_count_arr_before[4] + case_count_arr_after[4]) > 5 && case_count_arr_after[6] > 5)
    {
        // 4 -> danger right cases
        // 6 -> white cases
        return "P_R"; // PID-Failure in right side
    }
    if ((case_count_arr_after[1] + case_count_arr_before[1]) > 10 && (case_count_arr_after[1] + case_count_arr_before[1]) > (case_count_arr_after[3] + case_count_arr_before[3]))
    {
        // 0 -> normal cases
        // 1 -> left turn cases
        // 3 -> right turn cases
        return "L";
    }
    if ((case_count_arr_after[3] + case_count_arr_before[3]) > 10 && (case_count_arr_after[3] + case_count_arr_before[3]) > (case_count_arr_after[1] + case_count_arr_before[1]))
    {
        // 1 -> left turn cases
        // 3 -> right turn cases
        return "R";
    }

    if (case_count_arr_after[6] > 20)
    {
        // 1 -> left turn cases
        // 2 -> danger left cases
        // 3 -> right turn cases
        // 4 -> danger right cases
        // 6 -> white cases
        return "L_B"; // Line break
    }

    if (case_count_arr_after[5] > 30)
    {
        return "B";
    }
    return "N";
}