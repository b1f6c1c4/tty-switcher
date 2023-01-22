# tty-switcher

> Switch b/w TTYs (Linux Console), call vlock(1), etc. using Adafruit MACROPAD RP2040

## Functions

```
                                  +---+
                                  |RST|
+--------------------------------++---+---+
|+-------+-------+-------+-------+        |
|| tty1  | tty2  | tty3  | tty4  | +-----++
||       |       |       |       | |  O  ||
||B: Prev|V: ^Wk |B: Next|       | |  L  ||
|+-------+-------+-------+-------+ |  E  ||
|| tty5  | tty6  | tty7  | tty8  | |  D  ||
||       |       |       |       | +-----++
||V: ^Wh |V: ^Wj |V: ^Wl |       |        |
|+-------+-------+-------+-------+ +-----++
|| vlock |browser|  VIM  | Go to | | tty ||
||  ALL  | tabs  |       | prev- | |roter||
|| ttys! |       |V: ^W^W|  tty  | |/BOOT||
|+-------+-------+-------+-------+ +-----+|
+--------------------------------+-+------+
```

- `V:` for VIM mode, `B:` for browser tab mode
- `prev-tty`+`tty*` is temporary tty preview.
- rotating `tty roter` does not change the "prev" tty.
- press `BOOT` and then rotate changes only the "prev" tty.
- `VIM`+`prev-tty` sets a tty to vim-mode, issuing `:wq<CR>` when switching from it to another tty using `prev-tty` key.

LEDs:
- Red: vlocked
- Green: current tty
- Blue: prev tty
- Amber: temporary tty

## Install

1. Pre-requisite: A computer running GNU/Linux, preferably Arch Linux, with systemd.
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
    1. `sudo pacman -U <path-to-pkg.tar.zst>`. In the case you don't have `pacman`:
        - install udev rules, systemd unit, and daemon binary manually.
        - `sudo udevadm control --reload`
        - `sudo systemd daemon-reload`

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

