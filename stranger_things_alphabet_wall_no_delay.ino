/*
  This code is copyright 2018 Jonathan Thomson, jethomson.wordpress.com

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

//#include <Arduino.h>
//#include <FastLED.h>
#include "StrangerString.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN 2  //this is the data pin connected to the LED strip.  If using WS2801 you also need a clock pin
#define NUM_LEDS 50 //change this for the number of LEDs in the strip
#define COLOR_ORDER RGB
#define LED_STRIP_VOLTAGE 5
#define LED_STRIP_INITIAL_MILLIAMPS 250
#define FRAMES_PER_SECOND  120

#define RGB_STYGIAN_BLACK 0x050505
#define LED_OFFSET 5  // LED number corresponding to letter Z
#define MESSAGE_BREAK 2000
#define PUSHBUTTON1 4

enum Mode {STRANGER_THINGS_THEME = 0, DEATH_RATTLE = 1, SPOOKY_LIST = 2, SERIAL_INPUT = 3, COSTUME_IS_LIT_INTRO = 4, COSTUME_IS_LIT = 5};
enum Effect {WRITE, WRITE_TRAIL, FLICKER, BREATH, NO_EFFECT};

                                // A            , B            , C            , D            , E            , F            , G            , H            ,
const uint16_t letter_indices[] = {29+LED_OFFSET , 28+LED_OFFSET , 27+LED_OFFSET , 26+LED_OFFSET , 25+LED_OFFSET , 24+LED_OFFSET , 23+LED_OFFSET , 22+LED_OFFSET ,
                                // I            , J            , K            , L            , M            , N            , O            , P            , Q           ,
                                   11+LED_OFFSET, 12+LED_OFFSET, 13+LED_OFFSET, 14+LED_OFFSET, 15+LED_OFFSET, 16+LED_OFFSET, 17+LED_OFFSET, 18+LED_OFFSET , 19+LED_OFFSET,
                                // R            , S            , T            , U            , V            , W            , X            , Y            , Z
                                   8+LED_OFFSET, 7+LED_OFFSET, 6+LED_OFFSET, 5+LED_OFFSET, 4+LED_OFFSET, 3+LED_OFFSET, 2+LED_OFFSET, 1+LED_OFFSET, 0+LED_OFFSET};

extern const uint32_t letter_colors[26];

CRGB leds[NUM_LEDS];

StrangerString spooky_list[] = {StrangerString("HAPPY HALLOWEEN", CRGB::OrangeRed),
                                StrangerString("TRICK OR TREAT", CRGB::Red),
                                StrangerString("BAT", CRGB::Blue),
                                StrangerString("BLACK CAT", RGB_STYGIAN_BLACK),
                                StrangerString("CORPSE", CRGB::Green),
                                StrangerString("CRYPT", CRGB::Red),
                                StrangerString("DEATH", RGB_STYGIAN_BLACK),
                                StrangerString("EERIE", CRGB::Blue),
                                StrangerString("EVIL", CRGB::Red),
                                StrangerString("EYEBALLS", CRGB::OrangeRed),
                                StrangerString("FEAR", CRGB::Red),
                                StrangerString("FRIGHTFUL", CRGB::OrangeRed),
                                StrangerString("GHASTLY", CRGB::Red),
                                StrangerString("GHOST", CRGB::White),
                                StrangerString("GHOUL", CRGB::Green),
                                StrangerString("GRAVE", CRGB::Blue),
                                StrangerString("GRIM", RGB_STYGIAN_BLACK),
                                StrangerString("HAUNTED", CRGB::OrangeRed),
                                StrangerString("HOCUS POCUS", CRGB::Red),
                                StrangerString("JACK O LANTERN", CRGB::OrangeRed),
                                StrangerString("MIDNIGHT", CRGB::Purple),
                                StrangerString("MONSTER", CRGB::Green),
                                StrangerString("MUMMY", CRGB::White),
                                StrangerString("PHANTASMAGORIA", RGB_STYGIAN_BLACK),
                                StrangerString("POLTERGEIST", CRGB::Purple),
                                StrangerString("RIP", CRGB::Red),
                                StrangerString("SCARECROW", CRGB::OrangeRed),
                                StrangerString("SCREAM", CRGB::Red),
                                StrangerString("SKELETON", CRGB::White),
                                StrangerString("SPECTER", CRGB::Blue),
                                StrangerString("SPOOKY", CRGB::OrangeRed),
                                StrangerString("VAMPIRE", CRGB::Red),
                                StrangerString("WEREWOLF", CRGB::OrangeRed),
                                StrangerString("WITCH", RGB_STYGIAN_BLACK),
                                StrangerString("ZOMBIE", CRGB::Green)};


// My battery pack needs a minimum load or it turns itself off, so turn on some LEDs and keep them on.
// These LEDs are behind the wall so they provide backlighting.
// If your battery pack doesn't require a minimum load you can replace this function with just a call to
// FastLED.clear() only.
void backlight_only() {
    FastLED.clear();
    leds[3] = CRGB::Red;
    leds[4] = CRGB::Red;
    leds[14] = CRGB::Red;
    leds[15] = CRGB::Red;
    leds[25] = CRGB::Red;
    leds[26] = CRGB::Red;
}


bool display_letter(char letter, CRGB color) {

    static uint8_t li = LED_OFFSET;
    bool end_of_message = false;

    if ('A' <= letter && letter <= 'Z') {
        li = letter_indices[letter-ASCII_OFFSET];
        leds[li] = color;
    }
    else { // not a letter so set the previous LED index to the background color
        leds[li] = color;

        if (letter == '\0')
        {
            end_of_message = true;
        }
    }
    return end_of_message;

}


bool breath_red(uint16_t interval) {

    const uint8_t min_brightness = 2;
    static uint8_t delta = 0; // goes up to 255 then overflows back to 0
    bool complete_cycle = false; 

    static uint32_t pm = 0; // previous millis
    if ( (millis() - pm) > interval ) {
        pm = millis();

        for (unsigned int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Red;
        }

        // for the LEDs in the current state setting the brightness higher than max_brightness will not actually increase the brightness displayed
        uint8_t max_brightness = calculate_max_brightness_for_power_vmA(leds, NUM_LEDS, 255, LED_STRIP_VOLTAGE, LED_STRIP_INITIAL_MILLIAMPS);
        uint8_t b = scale8(triwave8(delta), max_brightness-min_brightness)+min_brightness;

        FastLED.setBrightness(b);

        delta++;
        if (delta == 0) {
            complete_cycle = true;
        }
    }
    return complete_cycle;
}


void fade_randomly(uint8_t chance_of_fade, uint8_t decay) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (chance_of_fade > random8()) {
            leds[i].fadeToBlackBy(decay);
        }
    }
}


void demogorgon_flicker(uint16_t interval) {

    fade_randomly(10, 40);

    // an on or off period less than 16 ms probably can't be perceived
    static uint32_t pm = 0; // previous millis
    if ( (millis() - pm) > interval ) {
        pm = millis();

        // set all the LEDs colors
        for (unsigned int i = 0; i < NUM_LEDS; i++) {
            leds[i] = letter_colors[25-(i%26)];
        }

        // set all the alphabet LEDs colors
        for (unsigned int  i = 0; i < 26; i++) {
            leds[letter_indices[i]] = letter_colors[i];
        }

        FastLED.setBrightness((random8(1,11) > 4)*255);
    }
}


void setup() {
    Serial.begin(57600);

    FastLED.setMaxPowerInVoltsAndMilliamps(LED_STRIP_VOLTAGE, LED_STRIP_INITIAL_MILLIAMPS);
    FastLED.setCorrection(TypicalPixelString);
    FastLED.addLeds<WS2811, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

    pinMode(PUSHBUTTON1, INPUT_PULLUP);
    randomSeed(analogRead(0));
}


void loop() {
    static int prev_reading = HIGH;
    static int button_state = HIGH;
    static uint32_t pm = 0; // previous millis
    static uint16_t debounce_period = 50;
    static String serial_input = "BOO";

    static bool idle = true;
    static Mode selected_mode = STRANGER_THINGS_THEME;
    static int8_t saved_selected_mode = -1;
    static uint8_t st_theme_step = 0;
    static StrangerString *ptrStrangerString = nullptr;
    static Effect effect;
    static bool message_finished = true;
    static bool trail_finished = true;
    static uint16_t delay_ms = 0;
    static uint8_t breath_cycles = 0;
    static char letter;
    static uint16_t delta = 0;
    static uint8_t sli = 0;


    while (true) {
        int reading = digitalRead(PUSHBUTTON1);

        if (reading != prev_reading) {
            // button hasn't stopped boucing yet so restart the timer
            pm = millis();
        }

        prev_reading = reading;

        if ( (millis() - pm) > debounce_period ) {
            // the button has stopped bouncing
            if (reading != button_state) {
                // reading is different from last time we took a settled reading
                // so we know it is a real button press and not a glitch

                if (reading == LOW) {
                    idle = true;
                    selected_mode = (Mode)((uint8_t)selected_mode + 1);
                }

                button_state = reading;
            }
            break;
        }
    }


    while (Serial.available()) {
        serial_input = Serial.readString();
        serial_input.toUpperCase();

        if (millis() > 15000) { // at start up ignore spurious AT commands from using /dev/ttyACM
            idle = true;
            saved_selected_mode = selected_mode;
            selected_mode = SERIAL_INPUT;
        }
        else {
            serial_input = "BOO";
        }
    }


    if (idle) {
        if (ptrStrangerString != nullptr && ptrStrangerString->m_is_deletable) {
            delete ptrStrangerString;
            ptrStrangerString = nullptr;
        }

        switch(selected_mode)
        {
            case STRANGER_THINGS_THEME:
                switch(st_theme_step)
                {
                    case 0:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("RIGHT HERE");
                        delay_ms = MESSAGE_BREAK;
                        break;
                    case 1:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("RUN");
                        delay_ms = 300;
                        break;
                    case 2:
                        backlight_only();
                        idle = false;
                        effect = FLICKER;
                        pm = millis();
                        delay_ms = 5000;
                        break;
                    case 3:
                        backlight_only();
                        idle = false;
                        effect = BREATH;
                        break;
                    case 4:
                        backlight_only();
                        fill_solid(leds, NUM_LEDS, CRGB::Red); // set the background color
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("STRANGER THINGS", RGB_STYGIAN_BLACK, CRGB::Red);
                        delay_ms = MESSAGE_BREAK;
                        break;
                    case 5:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("ELEVEN", 0xFF0088);
                        delay_ms = MESSAGE_BREAK;
                        break;
                    case 6:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("DEMOGORGON", RGB_STYGIAN_BLACK);
                        delay_ms = MESSAGE_BREAK;
                        break;
                    case 7:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("THE UPSIDE DOWN", CRGB::Red);
                        delay_ms = MESSAGE_BREAK;
                        break;
                    case 8:
                        backlight_only();
                        idle = false;
                        effect = WRITE;
                        message_finished = false;
                        ptrStrangerString = new StrangerString("EGGOS", CRGB::Yellow);
                        delay_ms = MESSAGE_BREAK;
                        break;
                    default:
                        effect = NO_EFFECT;
                        st_theme_step = 0;
                        break;
                }
                break;
            case DEATH_RATTLE:
                backlight_only();
                idle = false;
                effect = BREATH;
                break;
            case SPOOKY_LIST:
                backlight_only();
                ptrStrangerString = &(spooky_list[sli]);
                sli = (sli+1) % ARRAY_SIZE(spooky_list);
                message_finished = false;
                delay_ms = MESSAGE_BREAK;
                idle = false;
                effect = WRITE;
                break;
            case SERIAL_INPUT:
                backlight_only();
                idle = false;
                message_finished = false;
                if (serial_input.indexOf("!") == 0) {
                    effect = WRITE_TRAIL;
                    ptrStrangerString = new StrangerString(serial_input, CRGB::White);
                }
                else {
                    effect = WRITE;
                    ptrStrangerString = new StrangerString(serial_input, CRGB::White);
                }
                delay_ms = MESSAGE_BREAK;
                break;
            case COSTUME_IS_LIT_INTRO:
                backlight_only();
                idle = false;
                effect = WRITE;
                message_finished = false;
                ptrStrangerString = new StrangerString("MY COSTUME IS LIT");
                delay_ms = 300;
                break;
            case COSTUME_IS_LIT:
                backlight_only();
                idle = false;
                effect = FLICKER;
                pm = millis();
                delay_ms = 5000;
                break;
            default:
                effect = NO_EFFECT;
                selected_mode = STRANGER_THINGS_THEME;
                break;
        }
    }


    switch (effect) {
        case WRITE:
            FastLED.setBrightness(255);
            if (!message_finished) {
                letter = ptrStrangerString->get_letter();
                if (letter > -1) {
                    Serial.println(letter);
                    message_finished = display_letter(letter, ptrStrangerString->get_color());
                    pm = millis();
                }
            }
            else {
                if ((millis() - pm) > delay_ms) {
                    idle = true;
                    if (selected_mode == STRANGER_THINGS_THEME) {
                        st_theme_step++;
                    }
                    else if (selected_mode == COSTUME_IS_LIT_INTRO) {
                        selected_mode = (Mode)((uint8_t)selected_mode + 1);
                    }
                    else if (saved_selected_mode != -1 && selected_mode == SERIAL_INPUT) {
                        selected_mode = (Mode)saved_selected_mode;
                        saved_selected_mode = -1;
                    }
                }
            }
            break;
        case WRITE_TRAIL:
            FastLED.setBrightness(255);
            if (!message_finished) {
                if (trail_finished) {
                    letter = ptrStrangerString->get_letter();
                    if ('A' <= letter && letter <= 'Z') {
                        backlight_only();
                        trail_finished = false;
                    }
                    else if (letter == '\0') {
                        backlight_only();
                        message_finished = true;
                        trail_finished = true;
                        pm = millis();
                    }
                }
                else {
                    if ((millis() - pm) > 50) {
                        pm = millis();
                        uint16_t li = letter_indices[letter-ASCII_OFFSET];
                        delta++;
                        leds[NUM_LEDS-delta] = ptrStrangerString->get_color();
                        if (NUM_LEDS-delta == li) {
                            trail_finished = true;
                            delta = 0;
                        }
                    }
                }
            }
            else {
                if ((millis() - pm) > delay_ms) {
                    idle = true;
                    if (selected_mode == STRANGER_THINGS_THEME) {
                        st_theme_step++;
                    }
                    else if (selected_mode == COSTUME_IS_LIT_INTRO) {
                        selected_mode = (Mode)((uint8_t)selected_mode + 1);
                    }
                    else if (saved_selected_mode != -1 && selected_mode == SERIAL_INPUT) {
                        selected_mode = (Mode)saved_selected_mode;
                        saved_selected_mode = -1;
                    }
                }
            }
            break;
        case FLICKER:
            demogorgon_flicker(150);
            if ((millis() - pm) > delay_ms) {
                idle = true;
                if (selected_mode == STRANGER_THINGS_THEME) {
                    st_theme_step++;
                }
                else if (selected_mode == COSTUME_IS_LIT_INTRO) {
                    selected_mode = (Mode)((uint8_t)selected_mode + 1);
                }
            }
            break;
        case BREATH:
            if (breath_red(10)) {
                breath_cycles++;
            }
            if (breath_cycles > 2) {
                breath_cycles = 0;
                idle = true;
                if (selected_mode == STRANGER_THINGS_THEME) {
                    st_theme_step++;
                }
                else if (selected_mode == COSTUME_IS_LIT_INTRO) {
                    selected_mode = (Mode)((uint8_t)selected_mode + 1);
                }
            }
            break;
        default:
            break;
    }


    //FastLED.show();
    FastLED.delay(1000/FRAMES_PER_SECOND); 
}

