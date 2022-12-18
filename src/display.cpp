#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// On an arduino MEGA 2560: 20(SDA), 21(SCL)
#define OLED_RESET 10       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define CHAR_W 5 // Characer width of text size 1
#define CHAR_H 6 // Character height of text size 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// External Helper
const int numOfSensors = 8;
bool specialCase = 0;
// External variables in use
extern struct Memory sensorMemory;
extern int sensorThreshold[numOfSensors];
extern int sensorRawReading[numOfSensors];
extern boolean sensorBinaryReading[numOfSensors];
extern float motorVariables[4];

// External Functions in use
extern String buttonPressed();
extern void sonarDrive();
extern void memorySaveMotorVariables();
extern void generateThreshold();
extern void readSensors();
extern void generateBinary();
extern void Run();
extern void Stop(double del);
extern float sonarSearchF();
extern float sonarSearchL();
extern float sonarSearchR();
extern void Forward(double del, int vel);
extern void memorySetup(struct Memory *m);
extern void memoryGetArray(struct Memory *m, uint8_t arr[]);
extern void count_cases(uint8_t case_array[], int start, int end, uint8_t case_count_arr[]); // from case_detection

// Local  variables----------------------------------------------------------

// Global variables related to Option Selector
const int memoryLength = 100;
uint8_t displayMemoryArray[memoryLength];

int optX = 2;
int optY = 2;
int optH = 7;
int optionStart = 0; // For tracking options
String displayOptionBuffer[3];
String mainMenuOptions[6] = {
    "PID_MENU",
    "SENSOR_MENU",
    "SERVO_MENU",
    "SONAR_MENU",
    "MEMORY_MENU",
    "RUN"};
String PIDoptions[6] =
    {
        "M_SP",
        "Kp",
        "Ki",
        "Kd",
        "SAVE",
        "BACK"};
String sensorMenuOptions[6] = {
    "G_THRESHOLD",
    "V_THRESHOLD",
    "S_R_BIN",
    "S_R_RAW",
    "",
    "BACK",
};

String sensorCases[5] = {
    "NORMAL",
    "LEFT",
    "RIGHT",
    "BLACK",
    "WHITE",
};

int frontSonar;
int leftSonar;
int rightSonar;

/**
 * This function setups the display. Initiates communication in i2c protocol with the screen
 */
void displaySetup()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    // display.setTextSize(1);
}

/**
 * This function adjusts the screen buffer with the menu buffer
 * Ex: The screen buffer is limited which can show only three lines. But the menu has to show stuffs more than 3.
 *      This function handles the pseudo scrolling behavior of the screen.
 */
void displaySetOptionBuffer(String menuType)
{
    String str = "";
    for (int i = 0; i < 3; i++)
    {
        if (menuType == "MAIN_MENU")
            displayOptionBuffer[i] = mainMenuOptions[optionStart + i];
        else if (menuType == "PID_MENU")
            displayOptionBuffer[i] = PIDoptions[optionStart + i];
        else if (menuType == "SENSOR_MENU")
            displayOptionBuffer[i] = sensorMenuOptions[optionStart + i];
        else if (menuType == "MEMORY_MENU")
        {
            str = String(optionStart + i);
            str.concat(" ");
            for (int xx = 0; xx < 8; xx++)
            {
                if ((displayMemoryArray[optionStart + i] & (0b10000000 >> xx)) >> (7 - xx))
                    str.concat("1");
                else
                    str.concat("0");
            }
            displayOptionBuffer[i] = str;
        }
    }
}

/**
 * This function displays the boot screen showing the name of the robot and software.
 */
void displayBootScreen()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 0);
    display.println(F("THUNDER"));
    display.setCursor(18, 25);
    display.setTextSize(1);
    display.println("CORTEX ROBOTICS");
    for (int x = 0; x < SCREEN_WIDTH - 5; x += CHAR_W)
    {
        display.setCursor(x, 16);
        display.drawRect(x, 18, 4, 4, SSD1306_WHITE);
        display.fillRect(x, 18, 4, 4, SSD1306_WHITE);
        display.display();
        delay(80);
    }
    delay(1000);
    display.clearDisplay();
    display.display();
    delay(300);
}

/**
 * Function for handling options inside every menu. Option name has to be unique
 */
void optionHandler(String option)
{
    if (option == "SAVE")
    {
        display.clearDisplay();
        display.display();
        memorySaveMotorVariables();
    }
    else if (option == "G_THRESHOLD")
    {
        display.clearDisplay();
        display.display();
        generateThreshold();
    }
    else if (option == "V_THRESHOLD")
    {
        display.clearDisplay();
        display.display();
        delay(300);
        while (true)
        {
            displayDrawMenu("SENSOR_THRESHOLD_MENU");
            if (buttonPressed() != "NO")
            {
                break;
            }
        }
        delay(200);
    }
    else if (option == "S_R_RAW")
    {
        display.clearDisplay();
        display.display();
        delay(300);
        while (true)
        {
            display.clearDisplay();
            display.display();
            readSensors();
            displayDrawMenu("SENSOR_RAW_MENU");
            if (buttonPressed() != "NO")
            {
                break;
            }
        }
        delay(200);
    }
    else if (option == "S_R_BIN")
    {
        display.clearDisplay();
        display.display();
        delay(300);
        while (true)
        {
            display.clearDisplay();
            display.display();
            readSensors();
            generateBinary();
            displayDrawMenu("SENSOR_BINARY_MENU");
            if (buttonPressed() != "NO")
            {
                break;
            }
        }
        delay(200);
    }
    else if (option == "SONAR_MENU")
    {
        display.clearDisplay();
        display.display();
        delay(300);
        while (true)
        {
            display.clearDisplay();
            display.display();
            frontSonar = sonarSearchF();
            leftSonar = sonarSearchL();
            rightSonar = sonarSearchR();
            displayDrawMenu("SONAR_MENU");
            if (buttonPressed() != "NO")
            {
                break;
            }
        }
        delay(200);
    }
    else if (option == "RUN")
    {

        memorySetup(&sensorMemory); // clears the memory with all 0 before running
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 10);
        display.println(F("THUNDER"));
        display.display();
        delay(300);
        Forward(50, 100);
        while (true)
        {
            Run();
            if (buttonPressed() != "NO")
            {
                Stop(100);
                break;
            }
            if (specialCase == 1)
            {
                Stop(100);
                specialCase = 0;
                break;
            }
        }
    }
}

/**
 * Handler function for displaying and option selecting for a menu.
 * This function needs to be called with button press to update the screen
 */
void displayMenu(String menu_type)
{
    delay(300);
    displayDrawMenu(menu_type);
    displayOptionSelector(menu_type);
}

/**
 * Function to draw menu items
 */
void displayDrawMenu(String menuType)
{
    int x = 14;
    int y = 2;
    display.clearDisplay();              // Clear the display
    display.setTextSize(1);              // Set the text size
    display.setTextColor(SSD1306_WHITE); // Set the text color

    if (menuType == "MAIN_MENU") // Shows the main menu
    {
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        // drawing the options

        // Push all the options to the display buffer
        for (int optionsIterator = 0; optionsIterator < SCREEN_HEIGHT / 10; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                               // Set the cursor position . The top left position is 0,0
            display.println(displayOptionBuffer[optionsIterator]); // Print from the cursor position
        }
    }

    else if (menuType == "PID_MENU") // Shows the PID menu.
    {
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        for (int optionsIterator = 0; optionsIterator < SCREEN_HEIGHT / 10; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                               // Set the cursor position . The top left position is 0,0
            display.println(displayOptionBuffer[optionsIterator]); // Print from the cursor position
            display.setCursor(SCREEN_WIDTH / 2 + 4, y);
            if (optionsIterator + optionStart < 4)
                display.println(motorVariables[optionsIterator + optionStart]);
        }
    }

    else if (menuType == "SENSOR_MENU")
    {
        // Drawing the gridlines
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        // Push all the options to the display buffer
        for (int optionsIterator = 0; optionsIterator < 3; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                               // Set the cursor position . The top left position is 0,0
            display.println(displayOptionBuffer[optionsIterator]); // Print from the cursor position
        }
    }

    else if (menuType == "SENSOR_THRESHOLD_MENU")
    {
        for (int cursorX = 2, cursorY = 2; cursorX < SCREEN_WIDTH - 30; cursorX += 30)
        {
            display.setCursor(cursorX, cursorY);
            display.println(sensorThreshold[cursorX / 30]);
        }
        for (int cursorX = 2, cursorY = 20; cursorX < SCREEN_WIDTH - 30; cursorX += 30)
        {
            display.setCursor(cursorX, cursorY);
            display.println(sensorThreshold[4 + (cursorX / 30)]);
        }
    }

    else if (menuType == "SENSOR_RAW_MENU")
    {
        for (int cursorX = 2, cursorY = 2; cursorX < SCREEN_WIDTH - 30; cursorX += 30)
        {
            display.setCursor(cursorX, cursorY);
            display.println(sensorRawReading[cursorX / 30]);
        }
        for (int cursorX = 2, cursorY = 20; cursorX < SCREEN_WIDTH - 30; cursorX += 30)
        {
            display.setCursor(cursorX, cursorY);
            display.println(sensorRawReading[4 + (cursorX / 30)]);
        }
    }

    else if (menuType == "SENSOR_BINARY_MENU")
    {
        for (int cursorX = 6, cursorY = 15; cursorX < SCREEN_WIDTH - 15; cursorX += 15)
        {
            display.setCursor(cursorX, cursorY);
            display.println(sensorBinaryReading[cursorX / 15]);
        }
    }

    else if (menuType == "SONAR_MENU")
    {
        display.setCursor(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 0);
        display.println(frontSonar);
        display.setCursor(SCREEN_WIDTH * 0.1, SCREEN_HEIGHT / 2 - 5);
        display.println(leftSonar);
        display.setCursor(SCREEN_WIDTH * 0.8, SCREEN_HEIGHT / 2 - 5);
        display.println(rightSonar);
    }

    else if (menuType == "MEMORY_MENU") // Shows the main menu
    {
        memoryGetArray(&sensorMemory, displayMemoryArray);
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        // drawing the options

        // Push all the options to the display buffer
        for (int optionsIterator = 0; optionsIterator < SCREEN_HEIGHT / 10; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                               // Set the cursor position . The top left position is 0,0
            display.println(displayOptionBuffer[optionsIterator]); // Print from the cursor position
        }
    }

    display.display();
}

void displayDrawGrid(String menuType)
{
    // Draw Horizontal lines
    for (byte i = 0; i < SCREEN_HEIGHT; i += 10)
    {
        display.drawLine(0, i, SCREEN_WIDTH, i, SSD1306_WHITE);
    }
    // Draw two corner vertical Line
    display.drawLine(0, 0, 0, SCREEN_HEIGHT, SSD1306_WHITE);
    display.drawLine(SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT, SSD1306_WHITE);

    // Display optionSelectior Lines
    display.drawLine(10, 0, 10, SCREEN_HEIGHT, SSD1306_WHITE); // For left option selector line
    if (menuType == "PID_MENU")
    {
        // If type is PID menu, draw two more vertical lines
        display.drawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, SSD1306_WHITE);
        display.drawLine(SCREEN_WIDTH - 11, 0, SCREEN_WIDTH - 11, SCREEN_HEIGHT, SSD1306_WHITE);
    }
}

/**
 * Function for navigating through menu options
 */
void displayOptionSelector(String menuType)
{
    optY = 2;
    String buttonInstruction;
    if (menuType == "MAIN_MENU")
    {
        // Initial rectangle draw -> Bug fix needed
        display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.display();
        while (true)
        {
            buttonInstruction = buttonPressed();
            if (buttonInstruction != "NO")
            {
                if (buttonInstruction == "BTN_UP")
                {
                    if (optY - 10 < 2)
                    {
                        if (optionStart > 0)
                        {
                            optionStart--;
                        }
                    }
                    else
                    {
                        optY -= 10;
                    }
                }
                else if (buttonInstruction == "BTN_DOWN")
                {
                    if (optY + 10 >= SCREEN_HEIGHT)
                    {
                        if (optionStart < 3)
                        {
                            optionStart++;
                        }
                    }
                    else
                    {
                        optY += 10;
                    }
                }
                else if (buttonInstruction == "BTN_SELECT")
                {
                    if (displayOptionBuffer[optY / 10] != "RUN")
                        displayMenu(displayOptionBuffer[optY / 10]);
                    else
                        optionHandler(displayOptionBuffer[optY / 10]);
                }
                displayDrawMenu("MAIN_MENU");
                display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.display();
                delay(300);
            }
        }
    }

    else if (menuType == "PID_MENU")
    {
        // Draw initial rectangle
        display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.drawRect((SCREEN_WIDTH - 11 + optX), optY, optH, optH, SSD1306_WHITE);
        display.fillRect((SCREEN_WIDTH - 11 + optX), optY, optH, optH, SSD1306_WHITE);
        display.display();
        while (true)
        {
            buttonInstruction = buttonPressed();
            if (buttonInstruction != "NO")
            {

                if (buttonInstruction == "BTN_UP")
                {
                    if (optY - 10 < 2)
                    {
                        if (optionStart > 0)
                        {
                            optionStart--;
                        }
                    }
                    else
                    {
                        optY -= 10;
                    }
                }
                else if (buttonInstruction == "BTN_DOWN")
                {
                    if (optY + 10 >= SCREEN_HEIGHT)
                    {
                        if (optionStart < 3)
                        {
                            optionStart++;
                        }
                    }
                    else
                    {
                        optY += 10;
                    }
                }
                else if (buttonInstruction == "BTN_LEFT")
                {
                    if (displayOptionBuffer[optY / 10] != "SAVE" && displayOptionBuffer[optY / 10] != "BACK")
                        motorVariables[(optY / 10) + optionStart] -= 0.2;
                }
                else if (buttonInstruction == "BTN_RIGHT")
                {
                    if (displayOptionBuffer[optY / 10] != "SAVE" && displayOptionBuffer[optY / 10] != "BACK")
                        motorVariables[optY / 10 + optionStart] += 0.2;
                }
                else if (buttonInstruction == "BTN_SELECT")
                {
                    if (displayOptionBuffer[optY / 10] == "BACK")
                    {
                        delay(300);
                        optionStart = 0;
                        return;
                    }
                    else
                    {
                        optionHandler(displayOptionBuffer[optY / 10]);
                        delay(300);
                    }
                }
                displayDrawMenu("PID_MENU");
                display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.drawRect((SCREEN_WIDTH - 11 + optX), optY, optH, optH, SSD1306_WHITE);
                display.fillRect((SCREEN_WIDTH - 11 + optX), optY, optH, optH, SSD1306_WHITE);
                display.display();
                delay(100);
            }
        }
    }

    else if (menuType == "SONAR_MENU")
    {
        while (true)
        {
            buttonInstruction = buttonPressed();
            displayDrawMenu(menuType);
            optionHandler(menuType);
            if (buttonInstruction != "NO")
                break;
        }
    }

    else if (menuType == "SENSOR_MENU")
    {
        // Initial rect draw
        display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.display();

        while (true)
        {
            buttonInstruction = buttonPressed();
            if (buttonInstruction != "NO")
            {

                if (buttonInstruction == "BTN_UP")
                {
                    if (optY - 10 < 2)
                    {
                        if (optionStart > 0)
                        {
                            optionStart--;
                        }
                    }
                    else
                    {
                        optY -= 10;
                    }
                }
                else if (buttonInstruction == "BTN_DOWN")
                {
                    if (optY + 10 >= SCREEN_HEIGHT)
                    {
                        if (optionStart < 3)
                        {
                            optionStart++;
                        }
                    }
                    else
                    {
                        optY += 10;
                    }
                }
                else if (buttonInstruction == "BTN_SELECT" && optY >= 2 && optY <= display.height() - 10)
                {
                    if (displayOptionBuffer[optY / 10] == "BACK")
                    {
                        delay(300);
                        optionStart = 0;
                        return;
                    }
                    else
                    {
                        optionHandler(displayOptionBuffer[optY / 10]);
                        delay(300);
                    }
                }
                displayDrawMenu("SENSOR_MENU");
                display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.display();
                delay(300);
            }
        }
    }

    else if (menuType == "MEMORY_MENU")
    {
        // Initial rect draw
        display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
        display.display();

        while (true)
        {
            buttonInstruction = buttonPressed();
            if (buttonInstruction != "NO")
            {

                if (buttonInstruction == "BTN_UP")
                {
                    if (optY - 10 < 2)
                    {
                        if (optionStart > 0)
                        {
                            optionStart--;
                        }
                    }
                    else
                    {
                        optY -= 10;
                    }
                }
                else if (buttonInstruction == "BTN_DOWN")
                {
                    if (optY + 10 >= SCREEN_HEIGHT)
                    {
                        if (optionStart < 97)
                        {
                            optionStart++;
                        }
                    }
                    else
                    {
                        optY += 10;
                    }
                }
                else if (buttonInstruction == "BTN_SELECT" && optY >= 2 && optY <= display.height() - 10)
                {
                    delay(100);
                    optionStart = 0;
                    return;
                }
                displayDrawMenu("MEMORY_MENU");
                display.drawRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.fillRect(optX, optY, optH, optH, SSD1306_WHITE);
                display.display();
                delay(50);
            }
        }
    }
}

void displayCaseDetector(String type)
{
    int y = 0;
    int x = 0;
    specialCase = 1;
    uint8_t case_count_arr[5];
    String output = "";
    memoryGetArray(&sensorMemory, displayMemoryArray); // updating the memory array
    count_cases(displayMemoryArray, 00, 100, case_count_arr);
    display.clearDisplay();
    display.setTextSize(1);
    for (int i = 0; i < 5; i++)
    {

        display.setCursor(x, y);
        output = sensorCases[i];
        output.concat(" ");
        output.concat(String(case_count_arr[i]));
        display.println(output);
        if ((y + 20) > SCREEN_HEIGHT)
        {
            x = SCREEN_WIDTH / 2;
            y = 0;
        }
        y += 10;
    }
    display.display();
}

void displayScrollText()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println(F("scroll"));
    display.display(); // Show initial text
    delay(100);

    // Scroll in various directions, pausing in-between:
    // display.startscrollright(0x00, 0x0F);
    // delay(2000);
    // display.stopscroll();
    // delay(1000);
    // display.startscrollleft(0x00, 0x0F);
    // delay(2000);
    // display.stopscroll();
    // delay(1000);
    display.startscrolldiagright(0x00, 0x07);
    delay(2000);
    display.startscrolldiagleft(0x00, 0x07);
    delay(2000);
    display.stopscroll();
    delay(1000);
}
