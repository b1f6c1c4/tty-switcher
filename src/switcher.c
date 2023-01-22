#include <errno.h>
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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>
#include <wchar.h>
#include "switcher.h"

static bool g_use_syslog;
#define LOG(lvl, ...) do { \
        if (g_use_syslog) \
            syslog(LOG_ ## lvl, __VA_ARGS__); \
        else { \
            fprintf(stderr, "[%s] ", # lvl); \
            fprintf(stderr, __VA_ARGS__); \
            fprintf(stderr, "\n"); \
        } \
    } while (false)

static uid_t uid;
static gid_t gid;

static void parse_conf(void) {
    FILE *fin = fopen("/etc/tty-switcher.conf", "r");
    if (!fin) {
        LOG(ERR, "Cannot open /etc/tty-switcher.conf (%s), using root as vlock user", strerror(errno));
        return;
    }

    while (true) {
        char ch = fgetc(fin);
        switch (ch) {
            case '#': while (fgetc(fin) != '\n'); continue;
            case '\n': continue;
        }
        ungetc(ch, fin);
        break;
    }
    char buf[128];
    if (!fgets(buf, sizeof(buf), fin)) goto bad_conf;
    char *eol = strchr(buf, '\n');
    if (eol) *eol = '\0';
    char *grpn = strchr(buf, ':');
    if (grpn) *grpn++ = '\0';

    LOG(DEBUG, "User: %s", buf);
    struct passwd *pwd = getpwnam(buf);
    if (!pwd) goto bad_conf;
    uid = pwd->pw_uid;
    gid = pwd->pw_gid;

    if (grpn) {
        LOG(DEBUG, "Group: %s", grpn);
        struct group *grp = getgrnam(grpn);
        if (!grp) goto bad_conf;
        gid = grp->gr_gid;
    }

    fclose(fin);
    return;

bad_conf:
    LOG(ERR, "Cannot recognize format in /etc/tty-switcher.conf");
    fclose(fin);
}

int main(void) {
    g_use_syslog = false; // !getenv("SUDO_UID");

    uid = getuid(), gid = getgid();
    if (true || g_use_syslog) {
        openlog("tty-switcher", LOG_PID, LOG_DAEMON);
        parse_conf();
    } else {
        uid = atoi(getenv("SUDO_UID"));
        gid = atoi(getenv("SUDO_GID"));
    }
    LOG(INFO, "Waiting for input as UID=%d GID=%d", uid, gid);

    int fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        LOG(ERR, "Cannot open /dev/tty: %s", strerror(errno));
        return 1;
    }

    hid_init();

    hid_device *handle = NULL;
    struct hid_device_info *devs = hid_enumerate(0, 0);
    for (struct hid_device_info *ptr = devs; ptr; ptr = ptr->next) {
        if (ptr->usage_page == 0xFFB1 && ptr->usage == 0xF6) {
            handle = hid_open_path(ptr->path);
            if (!handle) {
                LOG(ERR, "Unable to open hidraw device, did you run this program w/ sudo?");
                hid_free_enumeration(devs);
                hid_exit();
                return 1;
            }
            break;
        }
    }
    hid_free_enumeration(devs);
    if (!handle) {
        LOG(ERR, "Unable to find hidraw device, did you connect & flashed the hardware?");
        hid_exit();
        return 1;
    }

    while (true) {
        struct switcher_packet pkt;
        int res = hid_read(handle, (unsigned char*)&pkt, sizeof(pkt));
        if (res != 32) {
            LOG(WARNING, "hid_read yields res=%d: %ls", res, hid_error(handle));
            break;
        }
        switch (pkt.id) {
            case SWTCHR_LOCK:
                if (ioctl(fd, VT_ACTIVATE, 63)) {
                    LOG(ERR, "ioctl(/dev/tty, VT_ACTIVATE, 63): %s", strerror(errno));
                    continue;
                }
                LOG(INFO, "Invoking vlock(1) with UID=%d GID=%d", uid, gid);
                pkt.payload = SWTCHR_ON;
                hid_write(handle, (unsigned char*)&pkt, sizeof(pkt));
                pid_t pid;
                if (!(pid = fork())) {
                    char *args[] = { "/usr/bin/vlock", "-a", NULL };
                    if (setgid(gid)) {
                        LOG(WARNING, "Cannot setgid to %d", gid);
                        return 2;
                    }
                    if (setuid(uid)) {
                        LOG(WARNING, "Cannot setuid to %d", uid);
                        return 2;
                    }
                    execv("/usr/bin/vlock", args);
                    LOG(ERR, "execv /usr/bin/vlock: %s", strerror(errno));
                    return 1;
                }
                int wstatus;
                if (waitpid(pid, &wstatus, 0) == -1) {
                    LOG(ERR, "waitpid(%d): %s", pid, strerror(errno));
                } else if (WIFEXITED(wstatus)) {
                    LOG(INFO, "vlock(1) %d finished with %d", pid, WEXITSTATUS(wstatus));
                } else if (WIFSIGNALED(wstatus)) {
                    LOG(INFO, "vlock(1) %d killed by %d", pid, WTERMSIG(wstatus));
                } else {
                    LOG(INFO, "vlock(1) %d quitted by unknown factor", pid);
                }
                pkt.payload = SWTCHR_OFF;
                hid_write(handle, (unsigned char*)&pkt, sizeof(pkt));
                break;
            default:
                LOG(NOTICE, "Unknown packet with ID=%d", pkt.id);
                break;
        }
    }

    hid_close(handle);
    hid_exit();
}
