#include <fcntl.h>
#include <grp.h>
#include <hidapi/hidapi.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>
#include "switcher.h"

int main(void) {
    int fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    hid_init();

    hid_device *handle = NULL;
    struct hid_device_info *devs = hid_enumerate(0, 0);
    for (struct hid_device_info *ptr = devs; ptr; ptr = ptr->next) {
        if (ptr->usage_page == 0xFFB1 && ptr->usage == 0xF6) {
            handle = hid_open_path(ptr->path);
            break;
        }
    }
    hid_free_enumeration(devs);

    if (!handle) {
        fprintf(stderr, "Unable to open device, did you forget sudo?\n");
        hid_exit();
        return 1;
    }

    while (true) {
        struct switcher_packet pkt;
        int res = hid_read(handle, (unsigned char*)&pkt, sizeof(pkt));
        if (res != 32) {
            printf("hid_read res=%d\n", res);
            printf("error: %ls\n", hid_error(handle));
            break;
        }
        switch (pkt.id) {
            // case SWTCHR_TTY:
            //  if (ioctl(fd, VT_ACTIVATE, pkt.payload))
            //      perror("ioctl");
            //  break;
            case SWTCHR_LOCK:
                if (ioctl(fd, VT_ACTIVATE, 63))
                    perror("ioctl");
                pkt.payload = SWTCHR_ON;
                hid_write(handle, (unsigned char*)&pkt, sizeof(pkt));
                pid_t pid;
                if (!(pid = fork())) {
                    char *args[] = { "/usr/bin/vlock", "-a", NULL };
                    int uid = getuid(), gid = getgid();
                    if (getenv("SUDO_UID"))
                        uid = atoi(getenv("SUDO_UID"));
                    else if (getenv("SUDO_USER")) {
                        struct passwd *pwd = getpwnam(getenv("SUDO_USER"));
                        if (pwd) uid = pwd->pw_uid;
                    }
                    if (getenv("SUDO_GID"))
                        gid = atoi(getenv("SUDO_GID"));
                    setgid(gid);
                    setuid(uid);
                    execv("/usr/bin/vlock", args);
                    perror("execv");
                    return 1;
                }
                waitpid(pid, NULL, 0);
                pkt.payload = SWTCHR_OFF;
                hid_write(handle, (unsigned char*)&pkt, sizeof(pkt));
                break;
        }
    }

    hid_close(handle);
    hid_exit();
}
