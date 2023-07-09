# Wordle Helper

This project is a command-line program written in C, intended to provide assistance in solving Wordle puzzles. Wordle is a game where the player attempts to guess a five-letter word within a certain number of attempts. The game provides feedback for each guess in the form of color-coded hints: gray signifies that the letter doesn't exist in the word at all, yellow means the letter is present but not in the right position, and green means the letter is in the correct position.

## Table of Contents

- [Overview](#overview)
- [How it Works](#how-it-works)
- [System Programming Features](#system-programming-features)
- [Compatibility](#compatibility)
- [How to Run the Program](#how-to-run-the-program)

## Overview

This Wordle Helper program essentially narrows down the list of possible answers based on the user's input. It uses a given dictionary of words, filters it according to the game's color-coded hints, and then returns a reduced list of possible words. The program continuously narrows down the word list based on each subsequent round of input until the correct word is found or the word list is exhausted.

## How it Works

The program executes in several steps. Here is a detailed description of the functions involved and the logic behind them:

1. **Dictionary Initialization:** The program copies a file containing a dictionary of words (the source file path is hardcoded as "/usr/share/dict/words") to a local file named "./wordlewords". This is done with standard C File I/O functions such as `fopen`, `fscanf`, `fprintf`, and `fclose`.

2. **User Input:** The user is prompted to input grey, yellow, and green characters. If the user doesn't input anything for grey, it defaults to '0'; for yellow, it defaults to '00000'; and for green, it defaults to '.....'. The program uses Terminal I/O manipulations to set non-canonical mode for character-by-character input.

3. **Regex Generation:** A regular expression (regex) is generated for each round of input using the function `genEx()`. This regex is designed to match the rules of the game. For example, it excludes capital letters, punctuation, grey letters, and position-specific yellow characters. The function uses dynamic memory allocation with `malloc` and string manipulation functions like `sprintf` to build the regex.

4. **Word Checking:** The program reads each word from the dictionary file and checks it against the generated regex and yellow letters using the `check()` function. This function compiles the regex using `regcomp` and uses `regexec` to match the regex against each word. If a word matches the criteria, it passes the check and is considered a possible solution. The function additionally checks for the presence of yellow letters in each word. If all checks pass, the word is printed to the console and saved to an output file named "./wordleansrs".

5. **Loop Control:** After all the words in the dictionary have been processed, the user is prompted to run the program again. If they choose not to, temporary files are deleted, and the program exits. The user's choice is handled in non-canonical mode for an intuitive 'y/n' input.

## System Programming Features

The program utilizes several systems programming concepts, including:

- **File I/O operations:** The program reads a word list from a file and writes output to another file. This practice is common in data processing tasks.

- **Regular expressions (Regex):** Regular expressions are used to generate patterns that match the rules of the game. This allows for efficient filtering of the word list.

- **Terminal I/O modes (TTY):** The terminal mode is switched to non-canonical mode to allow for character-by-character input, enhancing the interaction with the program.

- **Memory management:** The program handles dynamic memory allocation and deallocation to store and manage data efficiently. Functions like `malloc` and `free` are used for this purpose.

## Compatibility

This program is designed to run on Unix-based systems like Linux or MacOS. This is due to the specific dictionary path ("/usr/share/dict/words") used in the program, and the terminal I/O manipulations that are Unix-specific.

To run this program on a Windows system, the path to the dictionary file would need to be changed to a suitable location, and the terminal I/O manipulations may require alternative approaches.

## How to Run the Program

To run this program, follow these steps:

1. Clone the repository or download the source code.
2. Navigate to the directory containing the source code.
3. Compile the program using a C compiler. For instance, using gcc:
    gcc wordlehelper.c -o wordlehelper
4. Run the program:
    ./wordlehelper
5. Follow the prompts to input grey, yellow, and green characters as per your Wordle game state. The program will output a list of possible words for each round of input.
Remember to ensure that you have the correct permissions to read from and write to the necessary files and that the dictionary file is located in the expected path or adjust the path in the program accordingly.
