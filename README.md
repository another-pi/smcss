# Introduction

This is smcss(step motor control system server).

It runs under a STC89C52 MCU, which controls a stepping motor according command
sent from [a client program](https://github.com/cddwx/smcsc) runs under PC
through serial port.

It can read command, parse, operate output pins, then wait for next command.


## Command design

The commands is HEX data, serval format are defined for command use, commands
supported at present are:

    Function                            HEX code
    --------------------------------    ------------------
    Emergency reset                     EAEAEAEAEA
    Clear recieve strings               EEEEEEEEEE
    Show command in RAM                 E0E0E0E0E0
    Show command in EEPROM              E1E1E1E1E1
    Store command to EEPROM             E5E5E5E5E5
    Load command in EEPROM to RAM       E6E6E6E6E6
    Run command in RAM                  EFEFEFEFEF
    Run this command                    111111{main command}

# Develop environment

* STC89C52 MCU
* Keil
* STC flash tool in offical website

The program use MCU ISP instructions and xdata type memery.

# Quick guide

## Deploy

In my situation:

* Keil to compile
* STC flash tool in offical website to download program to MCU

## Usage

### Use STC flash tool

STC flash tool in offical website to send serial port data

### Use [smcsc client program](https://github.com/cddwx/smcsc)

This is a python program, compile to binary use pyinstaller is also OK, see
project page for detail.

The program define a more human readable commands, when send to MCU, it
transform the commands to HEX.

The program also support batch mode, that is to say, you can write many commands
by lines, then send together.
The program can transform them one by one, and then send to MCU one by one.
