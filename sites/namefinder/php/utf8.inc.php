<?php

/* this table lists ascii equivalents for utf8 characters so that
   searches including a utf8 character can be located by their
   ascii equivalent(s) and vice versa  - see canon.php */

return array(
  array(
    // ligatures
    chr(0xc3).chr(0x86) => 'ae', // cap
    chr(0xc3).chr(0xa6) => 'ae',
    chr(0xc5).chr(0x92) => 'oe', // cap
    chr(0xc3).chr(0x86) => 'oe',
    chr(0xc3).chr(0x9f) => 'ss', // german B
    chr(0xc5).chr(0x8a) => 'ng', // cap
    chr(0xc5).chr(0x8b) => 'ng',
    chr(0xe1).chr(0xb5).chr(0xab) => 'ue',
    chr(0xef).chr(0xac).chr(0x80) => 'ff',
    chr(0xef).chr(0xac).chr(0x81) => 'fi',
    chr(0xef).chr(0xac).chr(0x82) => 'fl',
    chr(0xef).chr(0xac).chr(0x83) => 'ffi',
    chr(0xef).chr(0xac).chr(0x84) => 'ffl',
    chr(0xef).chr(0xac).chr(0x85) => 'ft',
    chr(0xef).chr(0xac).chr(0x86) => 'st',

    chr(0x00) => '',
    chr(0x01) => '',
    chr(0x02) => '',
    chr(0x03) => '',
    chr(0x04) => '',
    chr(0x05) => '',
    chr(0x06) => '',
    chr(0x07) => '',
    chr(0x08) => '',
    chr(0x09) => '',
    chr(0x0A) => '',
    chr(0x0B) => '',
    chr(0x0C) => '',
    chr(0x0D) => '',
    chr(0x0E) => '',
    chr(0x0F) => '',

    chr(0x10) => '',
    chr(0x11) => '',
    chr(0x12) => '',
    chr(0x13) => '',
    chr(0x14) => '',
    chr(0x15) => '',
    chr(0x16) => '',
    chr(0x17) => '',
    chr(0x18) => '',
    chr(0x19) => '',
    chr(0x1A) => '',
    chr(0x1B) => '',
    chr(0x1C) => '',
    chr(0x1D) => '',
    chr(0x1E) => '',
    chr(0x1F) => '',

    chr(0x20) => ';;', // space
    chr(0x21) => '', // !
    chr(0x22) => '',  // "
    chr(0x23) => '', // #
    chr(0x24) => '', // $
    chr(0x25) => '',  // %
    chr(0x26) => '', // &
    chr(0x27) => '',  // '
    chr(0x28) => '', // (
    chr(0x29) => '', // )
    chr(0x2A) => '', // *
    chr(0x2B) => '',  // +
    chr(0x2C) => '', // ,
    chr(0x2D) => ';;',  // -
    chr(0x2E) => '', // .
    chr(0x2F) => ';;', // /

    chr(0x3A) => ';;', // :
    chr(0x3B) => ';;', // ;
    chr(0x3C) => '',  // <
    chr(0x3D) => ';;', // =
    chr(0x3E) => '',  // >
    chr(0x3F) => '', // ?

    chr(0x41) => 'a', // cap A
    chr(0x42) => 'b',
    chr(0x43) => 'c',
    chr(0x44) => 'd',
    chr(0x45) => 'e',
    chr(0x46) => 'f',
    chr(0x47) => 'g',
    chr(0x48) => 'h',
    chr(0x49) => 'i',
    chr(0x4a) => 'j',
    chr(0x4b) => 'k',
    chr(0x4c) => 'l',
    chr(0x4d) => 'm',
    chr(0x4e) => 'n',
    chr(0x4f) => 'o',
    chr(0x50) => 'p',
    chr(0x51) => 'q',
    chr(0x52) => 'r',
    chr(0x53) => 's',
    chr(0x54) => 't',
    chr(0x55) => 'u',
    chr(0x56) => 'v',
    chr(0x57) => 'w',
    chr(0x58) => 'x',
    chr(0x59) => 'y',
    chr(0x5A) => 'z', // cap Z

    chr(0x5B) => '', // [ 
    chr(0x5C) => '',  // backslash
    chr(0x5D) => '', // ]
    chr(0x5E) => '',  // hat
    chr(0x5F) => '', // _

    chr(0x60) => '', // backtick
    chr(0x7B) => '', // {
    chr(0x7C) => ';;',  // |
    chr(0x7D) => '', // }
    chr(0x7E) => '',  // tilde
    chr(0x7F) => '', // unused ...

    chr(0xc2).chr(0xA0) => ';;', // nbsp
    chr(0xc2).chr(0xA1) => '',  // upside down exclaim
    chr(0xc2).chr(0xA2) => '',  // c stroke
    chr(0xc2).chr(0xA3) => '',  // pound
    chr(0xc2).chr(0xA4) => '',  // 
    chr(0xc2).chr(0xA5) => '',  // yen
    chr(0xc2).chr(0xA6) => '',  // double bar
    chr(0xc2).chr(0xA7) => '',  // para
    chr(0xc2).chr(0xA8) => '',  // umlaut
    chr(0xc2).chr(0xA9) => '',  // copyright
    chr(0xc2).chr(0xAA) => '',
    chr(0xc2).chr(0xAB) => '',  // laquo
    chr(0xc2).chr(0xAC) => '',  // hook
    chr(0xc2).chr(0xAD) => '',  // SHY
    chr(0xc2).chr(0xAE) => '',  // registered
    chr(0xc2).chr(0xAF) => '',  // bar accent

    chr(0xc2).chr(0xB0) => '',  // degrees
    chr(0xc2).chr(0xB1) => '',  // plus or minus
    chr(0xc2).chr(0xB2) => '',  // squared
    chr(0xc2).chr(0xB3) => '',  // cubed
    chr(0xc2).chr(0xB4) => '',  // rsquo ??
    chr(0xc2).chr(0xB5) => '',  // mu
    chr(0xc2).chr(0xB6) => '',  // para
    chr(0xc2).chr(0xB7) => '',  // dot
    chr(0xc2).chr(0xB8) => '',  // cedilla
    chr(0xc2).chr(0xB9) => '',  // power 1
    chr(0xc2).chr(0xBA) => '',  // power 0
    chr(0xc2).chr(0xBB) => '' ,  // raquo
    chr(0xc2).chr(0xBC) => '',  // quarter
    chr(0xc2).chr(0xBD) => '',  // half
    chr(0xc2).chr(0xBE) => '',  // three quarters
    chr(0xc2).chr(0xBF) => '',  // upside down ? mark

    chr(0xc3).chr(0x80) => 'a',  // A grave
    chr(0xc3).chr(0x81) => 'a',  // A acute
    chr(0xc3).chr(0x82) => 'a',  // A circumflex
    chr(0xc3).chr(0x83) => 'a',  // A tilde
    chr(0xc3).chr(0x84) => 'a',  // A umlaut
    chr(0xc3).chr(0x85) => 'aa', // A ring
    chr(0xc3).chr(0x86) => 'ae', // AE dipthong
    chr(0xc3).chr(0x87) => 'c',  // C cedilla
    chr(0xc3).chr(0x88) => 'e',  // E grave
    chr(0xc3).chr(0x89) => 'e',  // E acute
    chr(0xc3).chr(0x8A) => 'e',  // E circumflex
    chr(0xc3).chr(0x8B) => 'e',  // E double dot
    chr(0xc3).chr(0x8C) => 'i',  // I grave
    chr(0xc3).chr(0x8D) => 'i',  // I acute
    chr(0xc3).chr(0x8E) => 'i',  // I circumflex
    chr(0xc3).chr(0x8F) => 'i',  // I umlaut

    chr(0xc3).chr(0x90) => 'd',  // D bar (eth)
    chr(0xc3).chr(0x91) => 'n',  // N tilde
    chr(0xc3).chr(0x92) => 'o',  // O grave
    chr(0xc3).chr(0x93) => 'o',  // O acute
    chr(0xc3).chr(0x94) => 'o',  // O circumflex
    chr(0xc3).chr(0x95) => 'o',  // O tilde
    chr(0xc3).chr(0x96) => 'o',  // O umlaut
    chr(0xc3).chr(0x97) => '',   // multiply
    chr(0xc3).chr(0x98) => 'o',  // O with slash
    chr(0xc3).chr(0x99) => 'u',  // U grave
    chr(0xc3).chr(0x9A) => 'u',  // U acute
    chr(0xc3).chr(0x9B) => 'u',  // U circumflex
    chr(0xc3).chr(0x9C) => 'u',  // U umlaut
    chr(0xc3).chr(0x9D) => 'y',  // y acute
    chr(0xc3).chr(0x9E) => 'th', // D with long straight edge- thorn
    chr(0xc3).chr(0x9F) => 'ss', // german "B" like

    chr(0xc3).chr(0xA0) => 'a',  // a grave
    chr(0xc3).chr(0xA1) => 'a',  // a acute
    chr(0xc3).chr(0xA2) => 'a',  // a circumflex
    chr(0xc3).chr(0xA3) => 'a',  // a tilde
    chr(0xc3).chr(0xA4) => 'a',  // a umlaut
    chr(0xc3).chr(0xA5) => 'aa', // a ring
    chr(0xc3).chr(0xA6) => 'ae', // dipthong
    chr(0xc3).chr(0xA7) => 'c',  // c cedilla
    chr(0xc3).chr(0xA8) => 'e',  // e grave
    chr(0xc3).chr(0xA9) => 'e',  // e acute
    chr(0xc3).chr(0xAA) => 'e',  // e circumflex
    chr(0xc3).chr(0xAB) => 'e',  // e umlaut
    chr(0xc3).chr(0xAC) => 'i',  // i grave
    chr(0xc3).chr(0xAD) => 'i',  // i acute
    chr(0xc3).chr(0xAE) => 'i',  // i circumflex
    chr(0xc3).chr(0xAF) => 'i',  // i umlaut

    chr(0xc3).chr(0xB0) => 'd',  // lower case eth
    chr(0xc3).chr(0xB1) => 'n',  // n tilde
    chr(0xc3).chr(0xB2) => 'o',  // o grave
    chr(0xc3).chr(0xB3) => 'o',  // o acute
    chr(0xc3).chr(0xB4) => 'o',  // o circumflex
    chr(0xc3).chr(0xB5) => 'o',  // o tilde
    chr(0xc3).chr(0xB6) => 'o',  // o umlaut
    chr(0xc3).chr(0xB7) => '',   // divide
    chr(0xc3).chr(0xB8) => 'o',  // o slash (scandinavian)
    chr(0xc3).chr(0xB9) => 'u',  // u grave
    chr(0xc3).chr(0xBA) => 'u',  // u acute
    chr(0xc3).chr(0xBB) => 'u',  // u circumflex
    chr(0xc3).chr(0xBC) => 'u',  // u umlaut
    chr(0xc3).chr(0xBD) => 'y',  // y acute
    chr(0xc3).chr(0xBE) => 'th', // thorn
    chr(0xc3).chr(0xBF) => 'y',   // y umlaut

    chr(0xc4).chr(0x80) => 'a',  
    chr(0xc4).chr(0x81) => 'a',  
    chr(0xc4).chr(0x82) => 'a',  
    chr(0xc4).chr(0x83) => 'a',  
    chr(0xc4).chr(0x84) => 'a',  
    chr(0xc4).chr(0x85) => 'a', 
    chr(0xc4).chr(0x86) => 'c', 
    chr(0xc4).chr(0x87) => 'c',  
    chr(0xc4).chr(0x88) => 'c',  
    chr(0xc4).chr(0x89) => 'c',  
    chr(0xc4).chr(0x8A) => 'c',  
    chr(0xc4).chr(0x8B) => 'c',  
    chr(0xc4).chr(0x8C) => 'c',  
    chr(0xc4).chr(0x8D) => 'c',  
    chr(0xc4).chr(0x8E) => 'd',  
    chr(0xc4).chr(0x8F) => 'd',  

    chr(0xc4).chr(0x90) => 'd',  
    chr(0xc4).chr(0x91) => 'd',  
    chr(0xc4).chr(0x92) => 'e',  
    chr(0xc4).chr(0x93) => 'e',  
    chr(0xc4).chr(0x94) => 'e',  
    chr(0xc4).chr(0x95) => 'e',  
    chr(0xc4).chr(0x96) => 'e',  
    chr(0xc4).chr(0x97) => 'e',   
    chr(0xc4).chr(0x98) => 'e',  
    chr(0xc4).chr(0x99) => 'e',  
    chr(0xc4).chr(0x9A) => 'e',  
    chr(0xc4).chr(0x9B) => 'e',  
    chr(0xc4).chr(0x9C) => 'g',  
    chr(0xc4).chr(0x9D) => 'g',  
    chr(0xc4).chr(0x9E) => 'g', 
    chr(0xc4).chr(0x9F) => 'g',   

    chr(0xc4).chr(0xA0) => 'g',  
    chr(0xc4).chr(0xA1) => 'g',  
    chr(0xc4).chr(0xA2) => 'g',  
    chr(0xc4).chr(0xA3) => 'g',  
    chr(0xc4).chr(0xA4) => 'h',  
    chr(0xc4).chr(0xA5) => 'h',  
    chr(0xc4).chr(0xA6) => 'h',  
    chr(0xc4).chr(0xA7) => 'h',   
    chr(0xc4).chr(0xA8) => 'i',  
    chr(0xc4).chr(0xA9) => 'i',  
    chr(0xc4).chr(0xAA) => 'i',  
    chr(0xc4).chr(0xAB) => 'i',  
    chr(0xc4).chr(0xAC) => 'i',  
    chr(0xc4).chr(0xAD) => 'i',  
    chr(0xc4).chr(0xAE) => 'i', 
    chr(0xc4).chr(0xAF) => 'i',   

    chr(0xc4).chr(0xB0) => 'i',  
    chr(0xc4).chr(0xB1) => 'i',  
    chr(0xc4).chr(0xB2) => 'ij',  
    chr(0xc4).chr(0xB3) => 'ij',  
    chr(0xc4).chr(0xB4) => 'j',  
    chr(0xc4).chr(0xB5) => 'j',  
    chr(0xc4).chr(0xB6) => 'k',  
    chr(0xc4).chr(0xB7) => 'k',   
    // chr(0xc4).chr(0xB8) => '',  kra
    chr(0xc4).chr(0xB9) => 'l',  
    chr(0xc4).chr(0xBA) => 'l',  
    chr(0xc4).chr(0xBB) => 'l',  
    chr(0xc4).chr(0xBC) => 'l',  
    chr(0xc4).chr(0xBD) => 'l',  
    chr(0xc4).chr(0xBE) => 'l', 
    chr(0xc4).chr(0xBF) => 'l',   

    chr(0xc5).chr(0x80) => 'l',
    chr(0xc5).chr(0x81) => 'l',  
    chr(0xc5).chr(0x82) => 'l',  
    chr(0xc5).chr(0x83) => 'n',  
    chr(0xc5).chr(0x84) => 'n',  
    chr(0xc5).chr(0x85) => 'n', 
    chr(0xc5).chr(0x86) => 'n', 
    chr(0xc5).chr(0x87) => 'n',  
    chr(0xc5).chr(0x88) => 'n',  
    chr(0xc5).chr(0x89) => 'n',  
    chr(0xc5).chr(0x8A) => 'n',  // eng
    chr(0xc5).chr(0x8B) => 'n',  // eng
    chr(0xc5).chr(0x8C) => 'o',  
    chr(0xc5).chr(0x8D) => 'o',  
    chr(0xc5).chr(0x8E) => 'o',  
    chr(0xc5).chr(0x8F) => 'o',  

    chr(0xc5).chr(0x90) => 'o',  
    chr(0xc5).chr(0x91) => 'o',  
    chr(0xc5).chr(0x92) => 'oe',  
    chr(0xc5).chr(0x93) => 'oe',  
    chr(0xc5).chr(0x94) => 'r',  
    chr(0xc5).chr(0x95) => 'r',  
    chr(0xc5).chr(0x96) => 'r',  
    chr(0xc5).chr(0x97) => 'r',   
    chr(0xc5).chr(0x98) => 'r',  
    chr(0xc5).chr(0x99) => 'r',  
    chr(0xc5).chr(0x9A) => 's',  
    chr(0xc5).chr(0x9B) => 's',  
    chr(0xc5).chr(0x9C) => 's',  
    chr(0xc5).chr(0x9D) => 's',  
    chr(0xc5).chr(0x9E) => 's', 
    chr(0xc5).chr(0x9F) => 's',   

    chr(0xc5).chr(0xA0) => 's',  
    chr(0xc5).chr(0xA1) => 's',  
    chr(0xc5).chr(0xA2) => 't',  
    chr(0xc5).chr(0xA3) => 't',  
    chr(0xc5).chr(0xA4) => 't',  
    chr(0xc5).chr(0xA5) => 't',  
    chr(0xc5).chr(0xA6) => 't',  
    chr(0xc5).chr(0xA7) => 't',   
    chr(0xc5).chr(0xA8) => 'u',  
    chr(0xc5).chr(0xA9) => 'u',  
    chr(0xc5).chr(0xAA) => 'u',  
    chr(0xc5).chr(0xAB) => 'u',  
    chr(0xc5).chr(0xAC) => 'u',  
    chr(0xc5).chr(0xAD) => 'u',  
    chr(0xc5).chr(0xAE) => 'u', 
    chr(0xc5).chr(0xAF) => 'u',

    chr(0xc5).chr(0xB0) => 'u',  
    chr(0xc5).chr(0xB1) => 'u',  
    chr(0xc5).chr(0xB2) => 'u',  
    chr(0xc5).chr(0xB3) => 'u',  
    chr(0xc5).chr(0xB4) => 'w',  
    chr(0xc5).chr(0xB5) => 'w',  
    chr(0xc5).chr(0xB6) => 'y',  
    chr(0xc5).chr(0xB7) => 'y',   
    chr(0xc5).chr(0xB8) => 'y',
    chr(0xc5).chr(0xB9) => 'z',  
    chr(0xc5).chr(0xBA) => 'z',  
    chr(0xc5).chr(0xBB) => 'z',  
    chr(0xc5).chr(0xBC) => 'z',  
    chr(0xc5).chr(0xBD) => 'z',  
    chr(0xc5).chr(0xBE) => 'z', 
    chr(0xc5).chr(0xBF) => 's',   

    chr(0xc6).chr(0x80) => 'b',
    chr(0xc6).chr(0x81) => 'b',  
    chr(0xc6).chr(0x82) => 'b',  
    chr(0xc6).chr(0x83) => 'b',  
    //chr(0xc6).chr(0x84) => '',  
    //chr(0xc6).chr(0x85) => '', 
    //chr(0xc6).chr(0x86) => '', 
    chr(0xc6).chr(0x87) => 'c',  
    chr(0xc6).chr(0x88) => 'c',  
    chr(0xc6).chr(0x89) => 'd',  
    chr(0xc6).chr(0x8A) => 'd',  
    chr(0xc6).chr(0x8B) => 'd',  
    chr(0xc6).chr(0x8C) => 'd',  
    //chr(0xc6).chr(0x8D) => '',  
    //chr(0xc6).chr(0x8E) => '',  
    //chr(0xc6).chr(0x8F) => '',  

    chr(0xc6).chr(0x90) => 'e',  
    chr(0xc6).chr(0x91) => 'f',  
    chr(0xc6).chr(0x92) => 'f',  
    chr(0xc6).chr(0x93) => 'g',  
    //chr(0xc6).chr(0x94) => '',  
    //chr(0xc6).chr(0x95) => '',  
    chr(0xc6).chr(0x96) => 'i',  
    chr(0xc6).chr(0x97) => 'i',   
    chr(0xc6).chr(0x98) => 'k',  
    chr(0xc6).chr(0x99) => 'k',  
    chr(0xc6).chr(0x9A) => 'l',  
    //chr(0xc6).chr(0x9B) => 'e',  
    //chr(0xc6).chr(0x9C) => 'g',  
    chr(0xc6).chr(0x9D) => 'n',  
    chr(0xc6).chr(0x9E) => 'n', 
    chr(0xc6).chr(0x9F) => 'o',   

    chr(0xc6).chr(0xA0) => 'o',  
    chr(0xc6).chr(0xA1) => 'o',  
    chr(0xc6).chr(0xA2) => 'oi',  
    chr(0xc6).chr(0xA3) => 'oi',  
    chr(0xc6).chr(0xA4) => 'p',  
    chr(0xc6).chr(0xA5) => 'p',  
    chr(0xc6).chr(0xA6) => 'yr',  
    //chr(0xc6).chr(0xA7) => '',   
    //chr(0xc6).chr(0xA8) => '',  
    //chr(0xc6).chr(0xA9) => 'i',  
    //chr(0xc6).chr(0xAA) => 'i',  
    chr(0xc6).chr(0xAB) => 't',  
    chr(0xc6).chr(0xAC) => 't',  
    chr(0xc6).chr(0xAD) => 't',  
    chr(0xc6).chr(0xAE) => 't', 
    chr(0xc6).chr(0xAF) => 'u',   

    chr(0xc6).chr(0xB0) => 'u',  
    //chr(0xc6).chr(0xB1) => '',  
    chr(0xc6).chr(0xB2) => 'v',  
    chr(0xc6).chr(0xB3) => 'y',  
    chr(0xc6).chr(0xB4) => 'y',  
    chr(0xc6).chr(0xB5) => 'z',  
    chr(0xc6).chr(0xB6) => 'z',  
    chr(0xc6).chr(0xB7) => 'k',   

    chr(0xc7).chr(0x84) => 'dz',  
    chr(0xc7).chr(0x85) => 'dz', 
    chr(0xc7).chr(0x86) => 'dz', 
    chr(0xc7).chr(0x87) => 'lj',  
    chr(0xc7).chr(0x88) => 'lj',  
    chr(0xc7).chr(0x89) => 'lj',  
    chr(0xc7).chr(0x8A) => 'nj',
    chr(0xc7).chr(0x8B) => 'nj',
    chr(0xc7).chr(0x8C) => 'nj',  
    chr(0xc7).chr(0x8D) => 'a',  
    chr(0xc7).chr(0x8E) => 'a',  
    chr(0xc7).chr(0x8F) => 'i',  

    chr(0xc7).chr(0x90) => 'i',  
    chr(0xc7).chr(0x91) => 'o',  
    chr(0xc7).chr(0x92) => 'o',  
    chr(0xc7).chr(0x93) => 'u',  
    chr(0xc7).chr(0x94) => 'u',  
    chr(0xc7).chr(0x95) => 'u',  
    chr(0xc7).chr(0x96) => 'u',  
    chr(0xc7).chr(0x97) => 'u',   
    chr(0xc7).chr(0x98) => 'u',  
    chr(0xc7).chr(0x99) => 'u',  
    chr(0xc7).chr(0x9A) => 'u',  
    chr(0xc7).chr(0x9B) => 'u',  
    chr(0xc7).chr(0x9C) => 'u',  
    // chr(0xc7).chr(0x9D) => '',  
    chr(0xc7).chr(0x9E) => 'a', 
    chr(0xc7).chr(0x9F) => 'a',   

    chr(0xc7).chr(0xA0) => 'a',  
    chr(0xc7).chr(0xA1) => 'a',  
    chr(0xc7).chr(0xA2) => 'ae',  
    chr(0xc7).chr(0xA3) => 'ae',  
    chr(0xc7).chr(0xA4) => 'g',  
    chr(0xc7).chr(0xA5) => 'g',  
    chr(0xc7).chr(0xA6) => 'g',  
    chr(0xc7).chr(0xA7) => 'g',   
    chr(0xc7).chr(0xA8) => 'k',  
    chr(0xc7).chr(0xA9) => 'k',  
    chr(0xc7).chr(0xAA) => 'q',  
    chr(0xc7).chr(0xAB) => 'q',  
    chr(0xc7).chr(0xAC) => 'q',  
    chr(0xc7).chr(0xAD) => 'q',  

    chr(0xc7).chr(0xB0) => 'j',  
    chr(0xc7).chr(0xB1) => 'dz',  
    chr(0xc7).chr(0xB2) => 'dz',  
    chr(0xc7).chr(0xB3) => 'dz',  
    chr(0xc7).chr(0xB4) => 'g',  
    chr(0xc7).chr(0xB5) => 'g',  
    chr(0xc7).chr(0xB8) => 'n',
    chr(0xc7).chr(0xB9) => 'n',  
    chr(0xc7).chr(0xBA) => 'a',  
    chr(0xc7).chr(0xBB) => 'a',  
    chr(0xc7).chr(0xBC) => 'ae',  
    chr(0xc7).chr(0xBD) => 'ae',  
    chr(0xc7).chr(0xBE) => 'o', 
    chr(0xc7).chr(0xBF) => 'o',   

    chr(0xc8).chr(0x80) => 'a',
    chr(0xc8).chr(0x81) => 'a',  
    chr(0xc8).chr(0x82) => 'a',  
    chr(0xc8).chr(0x83) => 'a',  
    chr(0xc8).chr(0x84) => 'e',  
    chr(0xc8).chr(0x85) => 'e', 
    chr(0xc8).chr(0x86) => 'e', 
    chr(0xc8).chr(0x87) => 'e',  
    chr(0xc8).chr(0x88) => 'i',  
    chr(0xc8).chr(0x89) => 'i',  
    chr(0xc8).chr(0x8A) => 'i',  
    chr(0xc8).chr(0x8B) => 'i',  
    chr(0xc8).chr(0x8C) => 'o',  
    chr(0xc8).chr(0x8D) => 'o',  
    chr(0xc8).chr(0x8E) => 'o',  
    chr(0xc8).chr(0x8F) => 'o',  

    chr(0xc8).chr(0x90) => 'r',  
    chr(0xc8).chr(0x91) => 'r',  
    chr(0xc8).chr(0x92) => 'r',  
    chr(0xc8).chr(0x93) => 'r',  
    chr(0xc8).chr(0x94) => 'u',  
    chr(0xc8).chr(0x95) => 'u',  
    chr(0xc8).chr(0x96) => 'u',  
    chr(0xc8).chr(0x97) => 'u',   
    chr(0xc8).chr(0x98) => 's',  
    chr(0xc8).chr(0x99) => 's',  
    chr(0xc8).chr(0x9A) => 't',  
    chr(0xc8).chr(0x9B) => 't',  
    //chr(0xc8).chr(0x9C) => '',  
    //chr(0xc8).chr(0x9D) => '',  
    chr(0xc8).chr(0x9E) => 'h', 
    chr(0xc8).chr(0x9F) => 'h',   

    chr(0xc8).chr(0xA0) => 'n',  
    chr(0xc8).chr(0xA1) => 'd',  
    chr(0xc8).chr(0xA2) => 'ou',  
    chr(0xc8).chr(0xA3) => 'ou',  
    chr(0xc8).chr(0xA4) => 'z',  
    chr(0xc8).chr(0xA5) => 'z',  
    chr(0xc8).chr(0xA6) => 'a',  
    chr(0xc8).chr(0xA7) => 'a',   
    chr(0xc8).chr(0xA8) => 'e',  
    chr(0xc8).chr(0xA9) => 'e',  
    chr(0xc8).chr(0xAA) => 'o',  
    chr(0xc8).chr(0xAB) => 'o',  
    chr(0xc8).chr(0xAC) => 'o',  
    chr(0xc8).chr(0xAD) => 'o',  
    chr(0xc8).chr(0xAE) => 'o', 
    chr(0xc8).chr(0xAF) => 'o',   

    chr(0xc8).chr(0xB0) => 'o',  
    chr(0xc8).chr(0xB1) => 'o',  
    chr(0xc8).chr(0xB2) => 'y',  
    chr(0xc8).chr(0xB3) => 'y',  
    chr(0xc8).chr(0xB4) => 'l',  
    chr(0xc8).chr(0xB5) => 'n',  
    chr(0xc8).chr(0xB6) => 't',  
    chr(0xc8).chr(0xB7) => 'j',   
    chr(0xc8).chr(0xB8) => 'db',
    chr(0xc8).chr(0xB9) => 'qp',  
    chr(0xc8).chr(0xBA) => 'a',  
    chr(0xc8).chr(0xBB) => 'c',  
    chr(0xc8).chr(0xBC) => 'c',  
    chr(0xc8).chr(0xBD) => 'l',  
    chr(0xc8).chr(0xBE) => 't', 
    chr(0xc8).chr(0xBF) => 's',   

    chr(0xc9).chr(0x80) => 'z', 

    chr(0xc9).chr(0x93) => 'b',  
    chr(0xc9).chr(0x95) => 'c',  
    chr(0xc9).chr(0x96) => 'd',  
    chr(0xc9).chr(0x97) => 'd',   
    chr(0xc9).chr(0x9B) => 'e',  

    chr(0xc9).chr(0xA0) => 'g',  
    chr(0xc9).chr(0xA1) => 'g',  
    chr(0xc9).chr(0xA2) => 'g',  
    chr(0xc9).chr(0xA6) => 'h',  
    chr(0xc9).chr(0xA7) => 'h',   
    chr(0xc9).chr(0xA8) => 'i',  
    chr(0xc9).chr(0xA9) => 'i',  
    chr(0xc9).chr(0xAA) => 'i',  
    chr(0xc9).chr(0xAB) => 'l',  
    chr(0xc9).chr(0xAC) => 'l',  
    chr(0xc9).chr(0xAD) => 'l',  

    chr(0xc9).chr(0xB1) => 'm',  
    chr(0xc9).chr(0xB2) => 'n',  
    chr(0xc9).chr(0xB3) => 'n',  
    chr(0xc9).chr(0xB4) => 'n',  
    chr(0xc9).chr(0xB5) => 'o',  
    chr(0xc9).chr(0xB6) => 'oe',
    chr(0xc9).chr(0xB9) => 'r',  
    chr(0xc9).chr(0xBA) => 'r',  
    chr(0xc9).chr(0xBB) => 'r',  
    chr(0xc9).chr(0xBC) => 'r',  
    chr(0xc9).chr(0xBD) => 'r',  
    chr(0xc9).chr(0xBE) => 'r', 
    chr(0xc9).chr(0xBF) => 'r'   

    // possibly others from the extended latin sets
  ),
  // alternate replacements:
  array(
    chr(0xc3).chr(0x84) => 'ae',  // A umlaut
    chr(0xc3).chr(0xA4) => 'ae',  // a umlaut

    chr(0xc3).chr(0x96) => 'oe',  // O umlaut
    chr(0xc3).chr(0xB6) => 'oe',  // o umlaut

    chr(0xc3).chr(0x9C) => 'ue',  // U umlaut
    chr(0xc3).chr(0xBC) => 'ue',  // u umlaut

    chr(0xc3).chr(0x85) => 'a', // A ring
    chr(0xc3).chr(0xA5) => 'a', // a ring

    chr(0xc3).chr(0x98) => 'oe',  // O slash (scandinavian)
    chr(0xc3).chr(0xB8) => 'oe',  // o slash 

    chr(0xc3).chr(0xB1) => 'ng',  // n tilde, spanish

    chr(0xc3).chr(0x90) => 'dh',  // D bar (eth)
    chr(0xc3).chr(0xB0) => 'dh',  // lower case eth
  ),
  array(
    chr(0xc3).chr(0xB1) => 'ny',  // n tilde, catalan
  ),
  array(
    chr(0xc3).chr(0xB1) => 'nh',  // n tilde, portugese
  ),
);

?>
