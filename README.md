# tty-switcher

> Switch b/w TTYs (Linux Console), call vlock(1), etc. using Adafruit MACROPAD RP2040

## TL;DR

1. Purchase and assemble parts:
    - PCB from [Adafruit](https://www.adafruit.com/product/5100)
    - Mechanical key switches
    - Key caps
    - Rotary encoder cap
    - USB-C cable
1. Flash firmware:
    1. Download [U2F File](https://github.com/b1f6c1c4/git-get/releases/download/latest/adafruit_macropad_tty-switcher.uf2)
        - Optionally, you can [compile](#build-from-source) yourself
    1. Enter the bootloader by pressing BOOT (rotary encoder) + RESET
    1. `sudo fdisk -l` to locate the new disk
    1. `mount /dev/sdX1  /mnt`, `cp` the `.u2f` file, `sync`, `umount /mnt`
1. Install software:
    1. Download [pkg.tar.zst File](https://github.com/b1f6c1c4/git-get/releases/download/latest/tty_switcher.pkg.tar.zst)
        - Optionally, you can [build](#build-from-source) yourself
    1. `sudo pacman -U <path-to-pkg.tar.zst>`
    1. `sudo udevadm control --reload`

## Build from Source

### For firmware:

1. `sudo pacman -S qmk`
1. `git gets -X https://github.com/b1f6c1c4/tty-switcher`
1. `ln -s <path-to-tty-switcher>/qmk_firmware ~/qmk_firmware`
1. `qmk setup -b tty-switcher`
1. `qmk config compile.keyboard=adafruit/macropad`
1. `qmk config compile.keymap=tty-switcher`
1. `qmk compile -j 8`
1. Result file is `~/qmk_firmware/adafruit_macropad_tty-switcher.uf2`

### For software:

1. `git get -X https://github.com/b1f6c1c4/tty-switcher`
1. `makepkg -sefi`

