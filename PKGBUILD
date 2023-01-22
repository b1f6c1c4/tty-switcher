# Maintainer: b1f6c1c4 <b1f6c1c4@gmail.com>
pkgname=tty_switcher
pkgver=0.1
pkgrel=1
pkgdesc="Switch b/w TTYs (Linux Console), call vlock(1), etc."
arch=('i686' 'x86_64')
url="https://github.com/b1f6c1c4/tty-switcher"
license=('MIT')
depends=('udev' 'systemd' 'hidapi')
source=()
sha256sums=()

build() {
    make
}

package() {
    mkdir -p "$pkgdir/usr/bin/"
    mkdir -p "$pkgdir/usr/lib/udev/rules.d/"
    mkdir -p "$pkgdir/usr/lib/systemd/system/"
    install -m 0755 "$srcdir/tty-switcherd" "$pkgdir/usr/bin/"
    install -m 0644 "$srcdir/11-tty-switcher.rules" "$pkgdir/usr/lib/udev/rules.d/"
    install -m 0644 "$srcdir/tty-switcher.service" "$pkgdir/usr/lib/systemd/system/"
}
