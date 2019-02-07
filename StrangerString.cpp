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


#include "StrangerString.h"


bool StrangerString::dynamically_created = false;

StrangerString::StrangerString() {
}

StrangerString::StrangerString(String message) {
    m_message = message;
    m_message.toUpperCase();
    m_message_index = 0;
    m_letter_on = true;
    m_fg_color = -1;
    m_bg_color = CRGB::Black;
    m_is_deletable = dynamically_created;
    dynamically_created = false;
}


StrangerString::StrangerString(String message, int32_t fg_color) {
    m_message = message;
    m_message.toUpperCase();
    m_message_index = 0;
    m_letter_on = true;
    m_fg_color = fg_color;
    m_bg_color = CRGB::Black;
    m_is_deletable = dynamically_created;
    dynamically_created = false;
}


StrangerString::StrangerString(String message, int32_t fg_color, int32_t bg_color) {
    m_message = message;
    m_message.toUpperCase();
    m_message_index = 0;
    m_letter_on = true;
    m_fg_color = fg_color;
    m_bg_color = bg_color;
    m_is_deletable = dynamically_created;
    dynamically_created = false;
}


char StrangerString::get_letter() {
    static uint32_t pm = 0; // previous millis
    static uint16_t delay_ms = 0;
    m_letter = -1;

    if ( (millis() - pm) > delay_ms ) {
        pm = millis();
        if (m_letter_on) {
            m_letter_on = false;
            delay_ms = CHARACTER_ON_TIME;
            m_letter = m_message[m_message_index];
            if (m_letter == 32) { // space character
                delay_ms = SPACE_ON_TIME;
            }
            m_message_index = (m_message_index+1) % (m_message.length()+1);
        }
        else {
            m_letter_on = true;
            delay_ms = CHARACTER_OFF_TIME;
            m_letter = '-';
        }
    }

    return m_letter;
}


CRGB StrangerString::get_color() {
    CRGB color;
    if ('A' <= m_letter && m_letter < 'Z') {
        if (m_fg_color == -1) {
            color = letter_colors[m_letter-ASCII_OFFSET];
        }
        else {
            color = m_fg_color;
        }
    }
    else {
        color = m_bg_color;
    }
    return color;
}


void * StrangerString::operator new(size_t size) { 
    void * p = ::new StrangerString();  
    dynamically_created = true;
    return p; 
}



