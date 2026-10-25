---
name: wincraft-guest
description: Working inside WinCraft OS in the VM - running commands and Windows programs as the person, installing programs, files and drives, lasting system changes (features), languages and keyboard, updates, logs. Read before changing or looking into anything inside the VM.
---

# Inside WinCraft OS

## The system

- Linux 6.18 with a BusyBox userland: no package manager, no systemd. Use
  `/usr/bin/setpriv`, never `setpriv` alone: BusyBox's applet lacks the
  options. `wcinit` is process 1 and supervises the services listed in
  `/etc/wincraft/services`; each logs to `/run/log/<name>.log` (`desktop`,
  `net`, `syslog`, and `Xorg.0.log`); `dmesg` has the kernel's.
- The system is one read-only image (`/live/medium/wincraft/system.sfs`)
  with a writable layer over it: memory in a live session, where
  `/run/wincraft/live` exists and everything is lost at shutdown; the data
  partition once installed, mounted at `/live/data`, and kept. An update
  replaces the image and keeps the layer.
- Don't customise WinCraft by editing its own files (`/usr`, `/etc/wincraft`):
  the edited copies would hide the next update's. Lasting system changes are
  features (below).
- `cat /etc/wincraft/version` gives the version.

## The person and their desktop

- One account, `user` (uid 1000, home `/home/user`), logged in
  automatically. The desktop is a Windows program, `wcshell.exe` (icons,
  taskbar, Start menu), run by Wine; `wcwm` manages the windows.
- The Wine prefix is `/home/user/.wine`. `C:` is
  `/home/user/.wine/drive_c`, with the person's folders in
  `C:\users\user\` (`Desktop`, `Documents`, `Downloads`...). `Z:` is the
  whole Linux file system. `H:` is the shared folder of this computer, when
  there is one (`/mnt/host`; `./wcvm share` mounts it again after a restart
  from inside the VM).
- `./wcvm sh --user` runs a command in the desktop's environment. Windows
  programs: `wine notepad`, `wine "C:\\Program Files\\App\\app.exe"`, or
  the path of a `.exe` alone (Linux hands Windows programs to Wine). One that
  opens a window goes in the background, `setsid ... >/dev/null 2>&1 &`, or
  `./wcvm sh` waits for it to close. A console program can run in the
  foreground and print: `./wcvm sh --user 'wine cmd /c ver'`.
- Files you create under `/home/user` as root belong to root: create them
  with `--user`, or `chown 1000:1000` them. `./wcvm push` does it for you.

## Installing a Windows program

1. The installer: a file of this computer (`./wcvm push
   ~/Downloads/setup.exe 'C:\users\user\Downloads\'`), or a download in the
   VM, where *Download a Program* on the desktop offers Firefox among others.
2. Start it as the person, in the background: `./wcvm sh --user 'setsid wine
   "C:\\users\\user\\Downloads\\setup.exe" >/dev/null 2>&1 &'` (an `.msi`:
   `wine msiexec /i FILE`). Its silent switch (`/S`, `/quiet`,
   `/VERYSILENT`), when it has one, spares the clicks.
3. Otherwise go through its windows with screenshots and clicks, or let the
   person do it in the VM's window. Shortcuts land in the Start menu.

Compatibility is Wine's: <https://appdb.winehq.org> tells how a program
fares. Kernel drivers, kernel-level anti-cheat, UWP and Store apps don't
work. Wine Mono stands in for the .NET Framework, though not for all of it;
.NET 5 and later need Microsoft's runtime, installed like any program. No
media codecs, no printing, no Vulkan.

## The system service

WinCraft's own windows ask root for system changes through wcinit, each with
a request of its own: `wcctl priv <helper> request=<16 to 32 hex digits>
ARGS`, with the progress in `/run/wincraft/requests/<name>`, whose last line
is `done`, `checked` or `error: ...`. The helpers are `install`, `update`,
`feature` and `agent` (the WinCraft Agent's). `./wcvm install` and `./wcvm
update` use them; for features, see below.

## Features: lasting changes to the system

A feature adds what WinCraft lacks, on this computer: a setting, a script at
every boot or login, a service, Windows registry settings, variables for the
desktop session. It lives on the data partition, replaces no system file and
survives updates. *WinCraft Features* in the Start menu lists them, and the
boot menu's *without your features* entry starts without them. A live
session forgets them at shutdown.

A feature is a folder of root's, `/var/lib/wincraft/features/<id>/`, the id
in lowercase letters, digits and `-`:

```
feature.conf                 name=, description=, created=YYYY-MM-DD,
                             by=Claude (WinCraft VM), version= (of WinCraft)
files/settings.d/*.conf      key=value, read by /usr/lib/wincraft/setting KEY [DEFAULT]
files/hooks/boot.d/*.sh      run as root at every boot, before the services
files/hooks/session.d/*.sh   run as the person at every login, before the desktop
files/services.d/*.service   "<name> <respawn|once> <command>" lines for wcinit
files/registry.d/*.reg       registry settings, imported at login when they change
files/environment.d/*.conf   KEY=value for the whole desktop session
```

Everything in it is root's, writable by no one else, with no symbolic links:
the helper refuses anything else, since an enabled feature runs as root.
Each hook has 30 seconds. In `environment.d`, `PATH`, `HOME`, `USER`,
`LOGNAME`, `DISPLAY`, `XDG_RUNTIME_DIR`, `WINEPREFIX` and `LD_*` are
refused. Enable it, then restart for it to take full effect:

```
./wcvm sh 'r=$(od -An -N8 -tx1 /dev/urandom | tr -d " \n"); wcctl priv feature request=$r enable ID; sleep 3; cat /run/wincraft/requests/$r'
```

`disable ID` and `remove ID` work the same way. Every change is logged in
`/var/lib/wincraft/features.log`.

- **Language**: `LANG=fr_FR.UTF-8` in `environment.d` shows Windows programs
  in French. The image has C.UTF-8 and de_AT, de_CH, de_DE, en_GB, es_ES,
  es_MX, fr_BE, fr_CA, fr_CH, fr_FR, it_IT, nl_BE, nl_NL, pl_PL, pt_BR, pt_PT
  and sv_SE, each `.UTF-8`.
- **Keyboard layout**: a session hook running `setxkbmap fr` (US otherwise).
- **Time zone**: `TZ=Europe/Paris` in `environment.d` (UTC otherwise), once
  `/usr/share/zoneinfo/Europe/Paris` is confirmed to exist.

## Updates

`./wcvm update check` compares the installed version with the newest
release. `./wcvm update install` downloads it inside the VM (about 270 MB,
checked against its published SHA-256) for the next start, and keeps the
version in use as the previous one. `./wcvm update previous` goes back to it
at the next start. A live session has nothing to update.

## Handy commands

- `./wcvm sh 'tail -50 /run/log/desktop.log'`, `ps`, `free -m`, `df -h /live/data`
- The network: `ip addr`, `nslookup github.com`. The VM sits behind QEMU's
  NAT as 10.0.2.15; this computer is 10.0.2.2 from inside.
- The clipboard is shared both ways for text (`wcclip` in the VM), when the
  `clipboard` setting is on.
