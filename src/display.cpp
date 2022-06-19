#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// On an arduino MEGA 2560: 20(SDA), 21(SCL)
#define OLED_RESET 10       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// External Functions in use
extern String buttonPressed();
//--------- External important functions---------------------------------------------------------
extern void memorySaveMotorVariables();
extern void generateThreshold();
extern void readSensors();
extern void generateBinary();
extern void Run();
extern void Stop(double del);
extern int sonarSearch();
extern void Forward(double del, int vel);
//--------- External important variables----------------------------------------------------------
extern float motorVariables[4];

const int numOfSensors = 8;
extern int sensorThreshold[numOfSensors];
extern int sensorRawReading[numOfSensors];
extern boolean sensorBinaryReading[numOfSensors];

// Global variables related to Option Selector
int optX = 2;
int optY = 2;
int optH = 7;
byte optionStart = 0; // For tracking options
const char *optionBuffer[3];
const char *mainMenuOptions[6] = {
    "PID_MENU",
    "SENSOR_MENU",
    "SERVO_MENU",
    "SONAR_MENU",
    "MEMORY_MENU",
    "RUN"};
const char *PIDoptions[6] =
    {
        "M_SP",
        "Kp",
        "Ki",
        "Kd",
        "SAVE",
        "BACK"};
const char *sensorMenuOptions[6] = {
    "G_THRESHOLD",
    "V_THRESHOLD",
    "S_R_BIN",
    "S_R_RAW",
    "",
    "BACK"};

void displaySetOptionBuffer(String menuType)
{
    for (int i = 0; i < 3; i++)
    {
        if (menuType == "MAIN_MENU")
            optionBuffer[i] = mainMenuOptions[optionStart + i];
        else if (menuType == "PID_MENU")
            optionBuffer[i] = PIDoptions[optionStart + i];
        else if (menuType == "SENSOR_MENU")
            optionBuffer[i] = sensorMenuOptions[optionStart + i];
    }
}
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
void displayBootScreen()
{

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 10);
    display.println(F("THUNDER"));
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
    delay(500);
}
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
    else if (option == "RUN")
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 10);
        display.println(F("THUNDER"));
        display.display();
        delay(300);
        Forward(500, 100);
        while (true)
        {
            Run();
            if (buttonPressed() != "NO")
            {
                Stop(100);
                break;
            }
        }
    }
}

void displayMenu(String menu_type)
{
    delay(300);
    displayDrawMenu(menu_type);
    displayOptionSelector(menu_type);
}

void displayDrawMenu(String menuType)
{
    int x = 14;
    int y = 2;
    display.clearDisplay();              // Clear the display
    display.setTextSize(1);              // Set the text size
    display.setTextColor(SSD1306_WHITE); // Set the text color

    if (menuType == "MAIN_MENU")
    {
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        // drawing the options

        // Push all the options to the display buffer
        for (int optionsIterator = 0; optionsIterator < SCREEN_HEIGHT / 10; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                        // Set the cursor position . The top left position is 0,0
            display.println(optionBuffer[optionsIterator]); // Print from the cursor position
        }
    }

    else if (menuType == "PID_MENU")
    {
        displayDrawGrid(menuType);
        displaySetOptionBuffer(menuType);

        for (int optionsIterator = 0; optionsIterator < SCREEN_HEIGHT / 10; optionsIterator++, y += 10)
        {
            display.setCursor(x, y);                        // Set the cursor position . The top left position is 0,0
            display.println(optionBuffer[optionsIterator]); // Print from the cursor position
            display.setCursor(SCREEN_WIDTH / 2 + 4, y);
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
            display.setCursor(x, y);                        // Set the cursor position . The top left position is 0,0
            display.println(optionBuffer[optionsIterator]); // Print from the cursor position
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
        display.setCursor(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5);
        display.println(sonarSearch());
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
                    if (strcmp(optionBuffer[optY / 10], "RUN") != 0)
                        displayMenu(optionBuffer[optY / 10]);
                    else
                        optionHandler(optionBuffer[optY / 10]);
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
                    if ((strcmp(optionBuffer[optY / 10], "SAVE") != 0) && (strcmp(optionBuffer[optY / 10], "BACK") != 0))
                        motorVariables[(optY / 10) + optionStart] -= 0.2;
                }
                else if (buttonInstruction == "BTN_RIGHT")
                {
                    if ((strcmp(optionBuffer[optY / 10], "SAVE") != 0) && (strcmp(optionBuffer[optY / 10], "BACK") != 0))
                        motorVariables[optY / 10 + optionStart] += 0.2;
                }
                else if (buttonInstruction == "BTN_SELECT")
                {
                    if (strcmp(optionBuffer[optY / 10], "BACK") == 0)
                    {
                        delay(300);
                        return;
                    }
                    else
                    {
                        optionHandler(optionBuffer[optY / 10]);
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
                    if (strcmp(optionBuffer[optY / 10], "BACK") == 0)
                    {
                        delay(300);
                        return;
                    }
                    else
                    {
                        optionHandler(optionBuffer[optY / 10]);
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
