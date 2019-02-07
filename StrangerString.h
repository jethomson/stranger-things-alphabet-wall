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

#ifndef StrangerString_H
#define StrangerString_H

#include <Arduino.h>
#include <FastLED.h>

#define ASCII_OFFSET 65 // number corresponding to ASCII letter A

#define CHARACTER_ON_TIME 1500
#define CHARACTER_OFF_TIME 300
#define SPACE_ON_TIME 400

                                 // A,            B,            C,            D,            E,            F,            G,            H,
const uint32_t letter_colors[26] = {CRGB::Yellow, CRGB::Purple, CRGB::Red,    CRGB::Green,  CRGB::Blue,   CRGB::Yellow, CRGB::Red,    CRGB::Green,
                                 // I,            J,            K,            L,            M,            N,            O,            P,           Q,
                                    CRGB::Blue,   CRGB::Purple, CRGB::Blue,   CRGB::White,  CRGB::Yellow, CRGB::Red,    CRGB::Red,    CRGB::White, CRGB::Red,
                                 // R,            S,            T,            U,            V,            W,            X,            Y,         Z
                                    CRGB::Green,  CRGB::Yellow, CRGB::Yellow, CRGB::Purple, CRGB::Red,    CRGB::Blue,   CRGB::Yellow, CRGB::Red, CRGB::Red};

class StrangerString {

    String m_message;
    uint8_t m_message_index;
    char m_letter;
    bool m_letter_on;
    int32_t m_fg_color; // int32_t is used instead of uint32_t so we can use -1 to indicate a default color should be used
    int32_t m_bg_color;

  public:
    bool m_is_deletable;
    StrangerString();
    StrangerString(String message);
    StrangerString(String message, int32_t fg_color);
    StrangerString(String message, int32_t fg_color, int32_t bg_color);
    char get_letter();
    CRGB get_color();
    void * operator new(size_t size);

  private:
    static bool dynamically_created;
};

#endif
