# tty-switcher

> Switch b/w TTYs (Linux Console), call vlock(1), etc. using Adafruit MACROPAD RP2040

1. Purchase and assemble parts:
    - PCB from [Adafruit](https://www.adafruit.com/product/5100)
    - Mechanical key switches
    - Key caps
    - Rotary encoder cap
    - USB-C cable
1. Flash firmware:
    1. Download [U2F File](https://github.com/b1f6c1c4/git-get/releases/download/latest/adafruit_macropad_tty-switcher.uf2)
        - Optionally, you can [compile](https://github.com/b1f6c1c4/qmk_firmware) yourself
    1. Enter the bootloader by pressing BOOT (rotary encoder) + RESET
    1. `sudo fdisk -l` to locate the new disk
    1. `mount /dev/sdX1  /mnt`, `cp` the `.u2f` file, `sync`, `umount /mnt`
1. Install software:
    1. Download [pkg.tar.zst File](https://github.com/b1f6c1c4/git-get/releases/download/latest/tty_switcher.pkg.tar.zst)
    1. `sudo pacman -U <path-to-pkg.tar.zst>`
    1. `sudo udevadm control --reload`

