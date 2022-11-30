/*
Sample Code to test Arduino Board on CRUMB V1.0
Reference: CRUMB V1.0 Built-in Example

- Modified Ball, score, paddle color
- Added "You Win" or "You Lose"
- Add ball line trace

Not6e: Sample code only, Code is not optimized

*/

int CS = 6;
int DC = 5;
int RES = 4;
int MOSI = 11;
int SCLK = 13;
int START = 9;

#include "SPI.h"
#include "TFT.h"

long gameStart;
bool playing;
bool lost;
int playerPadPosMin;
int playerPadPosMax;
int cpuPadPosMin;
int cpuPadPosMax;
byte winner = 0;
byte targetScore = 5;
TFT screen(CS, DC, RES); // Create new TFT called screen, setting the CD, DC and RESET pins - SPI Based
// TFT screen(CS, DC, MOSI, SCLK, RES); // Software based

void setup()
{

    Serial.begin(9600);
	Serial.println("Testing Serial Monitor!");
    digitalWrite(RES, HIGH);
    pinMode(START, INPUT);

    screen.begin();                               // Initialise TFT screen
    screen.background(0, 0, 0);                   // Set background color to black
    screen.stroke(255, 111, 0);                   // Set stroke to white
    screen.setTextSize(1);                        // Set text size to 0
    screen.text("Electrogics PH Test", 20, 50);   // Display start message on screen
    screen.setTextSize(0);                        // Set text size to 0
    screen.stroke(255, 255, 255);                 // Set stroke to white
    screen.text("Press START to begin", 20, 100); // Display start message on screen

    pulseIn(START, HIGH); // Wait for START input

    DrawGameArea(); // Initialise Game Area

    CPU = 0;
    PLAYER = 0;
    UpdateScores(); // Set Scores

    playing = false;
    ResetBall();                 // Reset ball to centre
    gameStart = millis() + 1000; // 1 Second delay until game start
}

void UpdatePlayerPad()
{

    int raw = analogRead(7);

    int rawCon = constrain(raw, 493, 593);

    int val = map(rawCon, 493, 593, 0, 84);

    screen.noStroke();

    screen.fill(0, 0, 0);
    screen.rect(screen.width() - 8, 0, 8, screen.height());

    screen.fill(0, 0, 255);

    screen.rect(screen.width() - 8, val, 4, 28);

    playerPadPosMin = val;
    playerPadPosMax = val + 28;
}

double cpuPadPos;
double cpuPadSpeed = 0.7;

void UpdateCPUPad()
{

    if (ballPos.y > cpuPadPos + 14)
    {
        cpuPadPos += cpuPadSpeed;
    }

    if (ballPos.y < cpuPadPos + 14)
    {
        cpuPadPos -= cpuPadSpeed;
    }

    cpuPadPos = constrain(cpuPadPos, 0, 84);

    screen.noStroke();

    screen.fill(0, 0, 0);
    screen.rect(0, 0, 8, screen.height());

    screen.fill(0, 255, 0);

    screen.rect(4, cpuPadPos, 4, 28);

    cpuPadPosMin = cpuPadPos;
    cpuPadPosMax = cpuPadPos + 28;
}

struct Vec2
{
    double x;
    double y;
};

struct Vec2 ballDir;
struct Vec2 ballPos;

void ResetBall()
{

    cpuPadPos = 42;

    ballPos.x = screen.width() / 2;
    ballPos.y = screen.height() / 2;

    double randomDir = -0.5 + (random(1000) / 1000.0);

    ballDir.x = 3 * cos(randomDir);
    ballDir.y = 3 * sin(-randomDir);
}

void UpdateBall()
{

    screen.noStroke();

    screen.fill(67, 67, 67);
    screen.rect(ballPos.x, ballPos.y, 4, 4);

    ballPos.x += ballDir.x;
    ballPos.y += ballDir.y;

    if (ballPos.y <= 1 || ballPos.y >= (screen.height() - 20))
    {
        ballDir.y *= -1;
    }

    if (ballPos.x > screen.width() - 14 && !lost)
    {
        if (ballPos.y < playerPadPosMin || ballPos.y > playerPadPosMax)
        {
            lost = true;
        }
        else
        {
            double paddleHit = -0.5 + (ballPos.y - playerPadPosMin) / 28;
            paddleHit *= 1.5;
            ballDir.x = -3 * cos(paddleHit);
            ballDir.y = -3 * sin(-paddleHit);
        }
    }

    if (ballPos.x < 12 && !lost)
    {
        if (ballPos.y < cpuPadPosMin || ballPos.y > cpuPadPosMax)
        {
            lost = true;
        }
        else
        {
            double paddleHit = -0.5 + (ballPos.y - cpuPadPosMin) / 28;
            paddleHit *= 1.5;
            ballDir.x = 3 * cos(paddleHit);
            ballDir.y = -3 * sin(-paddleHit);
        }
    }

    if (lost && ballPos.x > screen.width() || lost && ballPos.x < -2)
    {

        if (ballPos.x > screen.width())
            CPU++;

        if (ballPos.x < 0)
            PLAYER++;

        lost = false;
        playing = false;
        ResetBall();
        gameStart = millis() + 1000;

        UpdateScores();
    }
    else
    {

        screen.fill(255, 0, 0);
        screen.rect(ballPos.x, ballPos.y, 4, 4);
    }
}

void loop()
{

    delay(20); // 50Hz delay

    if (winner > 0)
    {
        if (playing == false)
        {
            screen.background(0, 0, 0); // Set background color to black
            screen.stroke(255, 0, 0);   // Set stroke to white
            screen.setTextSize(2);      // Set text size to 2
            if (winner == 1)
            {
                screen.text("You Lose!", 40, 50);
            }
            else
            {
                screen.stroke(0, 200, 0); // Set stroke to green
                screen.text("You Win!", 50, 50);
            }
            playing = true;
        }
    }
    else
    {
        UpdatePlayerPad();
        UpdateCPUPad();
        if (millis() > gameStart)
        {
            playing = true;
        }

        if (playing)
        {
            UpdateBall();
        }
    }
}

void DrawGameArea()
{

    screen.background(0, 0, 0);
}

int CPU = 0;
int PLAYER = 0;

void UpdateScores()
{

    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect(0, screen.height() - 20, screen.width(), 20);

    screen.stroke(200, 200, 200);

    screen.setTextSize(1);

    char buffer[33];

    screen.stroke(0, 200, 0);
    screen.text(itoa(CPU, buffer, 10), 35, 0);

    screen.stroke(0, 0, 200);
    screen.text(itoa(PLAYER, buffer, 10), 115, 0);
    screen.setTextSize(0);

    screen.noStroke();

    if (targetScore <= CPU)
    {
        winner = 1;
        playing = false;
        CPU = 0;
        PLAYER = 0;
    }
    else if (targetScore <= PLAYER)
    {
        winner = 2;
        playing = false;
    }
}

// Function to swap two numbers
void swap(char *x, char *y)
{
    char t = *x;
    *x = *y;
    *y = t;
}

// Function to reverse `buffer[i…j]`
char *reverse(char *buffer, int i, int j)
{
    while (i < j)
    {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

// Iterative function to implement `itoa()` function in C
char *itoa(int value, char *buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32)
    {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10)
        {
            buffer[i++] = 65 + (r - 10);
        }
        else
        {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0)
    {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10)
    {
        buffer[i++] = '-';
    }

    buffer[i] = 0; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}
