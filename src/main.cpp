#include <TFT_eSPI.h>
#include <vector>
#include <TJpg_Decoder.h>
#include <OneButton.h>
#include <FastLED.h>
#include "SD_MMC.h"
#include "EEPROM.h"
#include "pinconfig.h"

char path[] = "/";
const int maxFrameSize = 8*1024;
uint8_t frameBuffer[maxFrameSize];

std::vector<String> mjpgFileNames;
std::vector<int> mjpgFrameRates;

TFT_eSPI screen = TFT_eSPI();
OneButton button(BTN_PIN, true);
CRGB leds;

uint8_t currentFileIndex;
uint8_t currentRotation;
File currentFile;
uint32_t currentFrameDelay;
uint32_t nextFrameTime;
bool warningLightOn;

const int eepromAddress_FileIndex = 0;
const int eepromAddress_Rotation = 1;

bool RenderJPEGBlock(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
    screen.pushImage(x, y, w, h, bitmap);
    return 1;
}

void nextFile()
{
    currentFileIndex = (currentFileIndex + 1) % mjpgFileNames.size();
    EEPROM.write(eepromAddress_FileIndex, currentFileIndex);
    EEPROM.commit();
    currentFile.close();
    currentFile = SD_MMC.open(mjpgFileNames[currentFileIndex]);
    currentFrameDelay = 1000/mjpgFrameRates[currentFileIndex];
    nextFrameTime = millis();
}

void rotate()
{
    currentRotation ^= 1;
    EEPROM.write(eepromAddress_Rotation, currentRotation);
    EEPROM.commit();
    screen.setRotation(1 + currentRotation*2);
}

int getFPS(const String& fileName)
{
    String keyword = "-fps";
    int fpsIndex = fileName.indexOf(keyword);

    if (fpsIndex != -1)
    {
        fpsIndex +=keyword.length();
        String number = "";

        for (int i = fpsIndex; i < fileName.length(); ++i)
        {
            char c = fileName.charAt(i);
            if (isDigit(c))
            {
                number += c;
            }
            else
            {
                break;
            }
        }

        if (number.length() > 0)
        {
            return number.toInt();
        }
    }

    return 30;
}

void setup()
{
    pinMode(TFT_LEDA_PIN, OUTPUT);
    digitalWrite(TFT_LEDA_PIN, 1);

    Serial.begin(115200);

    EEPROM.begin(2);

    currentRotation = EEPROM.read(eepromAddress_Rotation) % 2;

    screen.begin();
    screen.setRotation(1 + currentRotation*2);
    screen.setSwapBytes(true); // We need to swap the colour bytes (endianess)
    screen.fillScreen(TFT_BLACK);

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(RenderJPEGBlock);

    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
    SD_MMC.begin();

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(&leds, 1);

    File root = SD_MMC.open(path);
    while(File file = root.openNextFile())
    {
        if(!file.isDirectory())
        {
            String fileName = file.name();
            if (fileName.endsWith(".mjpg") || fileName.endsWith(".mjpeg"))
            {
                mjpgFileNames.push_back(path+fileName);
                mjpgFrameRates.push_back(getFPS(fileName));
                Serial.print(mjpgFileNames[mjpgFileNames.size()-1]); Serial.print(" "); Serial.println(mjpgFrameRates[mjpgFileNames.size()-1]); 
            }
        }
    }

    currentFileIndex = EEPROM.read(eepromAddress_FileIndex) % mjpgFileNames.size();

    currentFile = SD_MMC.open(mjpgFileNames[currentFileIndex]);
    currentFrameDelay = 1000/mjpgFrameRates[currentFileIndex];
    nextFrameTime = millis();

    button.attachClick(nextFile);
    button.attachLongPressStart(rotate);

    leds = CRGB(0,0,0);
    FastLED.show();
    warningLightOn = false;

    digitalWrite(TFT_LEDA_PIN, 0);
}

uint32_t loadFrame(File file)
{
    if (!file.available()) return 0;

    int pos = 2;
    file.read(frameBuffer, maxFrameSize);
    while (frameBuffer[pos-2] != 0xFF || frameBuffer[pos-1] != 0xD9)
    {
        ++pos;
    }
    if (file.available())
    {
        file.seek(file.position()-(maxFrameSize-pos));
    }
    return pos;
}

void loop()
{
    button.tick();

    uint32_t size = loadFrame(currentFile);
    if (size)
    {
        TJpgDec.drawJpg(0, 0, frameBuffer, size);
        nextFrameTime += currentFrameDelay;

        if (millis() > nextFrameTime)
        {
            if (!warningLightOn)
            {
                leds = CRGB(255,0,0);
                FastLED.show();
                warningLightOn = true;
            }
        }
        else
        {
            if (warningLightOn)
            {
                leds = CRGB(0,0,0);
                FastLED.show();
                warningLightOn = false;
            }
        }

        while (millis() < nextFrameTime)
        {
            button.tick();
        }
    }
    else
    {
        currentFile.seek(0);
    }
}