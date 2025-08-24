# XenonDumper
Custom Xbox 360 application designed to dump the files & data required to use the [Xenon Xbox 360 Low Level Emulator](https://github.com/xenon-emu/xenon).

> [!NOTE]
> This app is **unofficial** and is in no way affiliated with the [xenon-emu](https://github.com/xenon-emu) project or creators.

## Features
Dumps the following files:
- **fuses.txt** - Contains the systems unique fusesets in the same format as they are when dumping via xell.
- **1bl.bin** - The first bootloader stored in the Secure ROM of the systems CPU.
- **flashdmp.bin** - The systems nand flash. This is handled automatically by booting into Simple360NandFlasher & utilising it's built in "AutoMode" set to dump.

## Requirements
This app supports both retail & developer kernels with the required patches to allow the install and use of HV expansions.<br>
The following setups should work:
- Retail consoles modified using hardware modifications (**RGH or JTAG**) and running either a patched retail 17559 kernel (freeboot) or a patched developer 17489 (or 17559 when spoofed) kernel ([RGLoader](https://github.com/RGLoader)). 
- Retail consoles exploited using [BadUpdate](https://github.com/grimdoomer/Xbox360BadUpdate) & the patches provided by [XeUnshackle](https://github.com/Byrom90/XeUnshackle).
- Development Kit consoles running a 17489 recovery & shadowboot such as [RGLoader](https://github.com/RGLoader).

## Credits
- [Swizzy](https://github.com/Swizzy) - Simple 360 NAND Flasher.