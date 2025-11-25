# ZChat / ZTranslator

Simple C++ console chat and translation application.

## Overview

ZChat provides a minimal command-line interface that uses a translator
component to process and translate text.\
The translator permits writing messages with a special prefix to
automatically translate text using external APIs.

-   Entry point (CLI): `ZChat.cpp`
-   Translator implementation: `ZTranslator.cpp`
-   Public translator API: `ZTranslator.h`

## Repository structure

-   `ZChat.cpp` --- application entry point and CLI handling.
-   `ZTranslator.cpp` --- implementation of translation logic and API
    calls.
-   `ZTranslator.h` --- public API and translation utilities.

## Requirements

-   C++17-compatible compiler (g++, clang, MSVC)
-   Optional: CMake for easier builds

## Build

Compile with g++:

``` sh
g++ -std=c++17 -O2 -Wall ZChat.cpp ZTranslator.cpp -o zchat
```

------------------------------------------------------------------------

# 🗣️ Translation Usage

The translator accepts **inline commands** to translate text from one
language to another.

### 📌 Format

    :FROM:TO Your text here

Where:

-   `FROM` = language code of the original text\
-   `TO` = language code to translate to\
-   `Your text here` = the message you want to translate

## ✔ Supported examples

### 🇪🇸 Spanish → 🇺🇸 English

    :es:en Hola cómo estás

### 🇺🇸 English → 🇪🇸 Spanish

    :en:es Hello friend

### 🇧🇷 Portuguese → 🇪🇸 Spanish

    :pt:es Bom dia meu amigo

### 🇫🇷 French → 🇺🇸 English

    :fr:en Bonjour, je suis prêt

### 🇯🇵 Japanese → 🇺🇸 English

    :ja:en こんにちは

------------------------------------------------------------------------

# 🌍 Supported Language Codes (ISO-639-1)

  Code   Language
  ------ ------------
  es     Spanish
  en     English
  pt     Portuguese
  fr     French
  it     Italian
  de     German
  ja     Japanese
  ru     Russian
  ko     Korean
  zh     Chinese

------------------------------------------------------------------------

# 🔧 How it works internally

1.  The user types a message with the `:from:to` prefix.\
2.  `ZTranslator::IsTranslationRequest()` checks if the format is
    valid.\
3.  `TranslateWithAPI()` performs the translation asynchronously.\
4.  The translated message replaces the original message.\
5.  ZChat sends it normally through the game's chat system.

------------------------------------------------------------------------

# 📄 Example output in chat

User writes:

    :es:en Hola mi amigo

Final message sent to chat:

    [ES→EN] Hello my friend
    
![§uhil_20251124_201645](https://github.com/user-attachments/assets/b454e1da-495f-498c-a0c7-a16d75e96e1e)
![§uhil_20251124_201037](https://github.com/user-attachments/assets/c7892026-2163-48c8-8109-064e02b6e853)
