---
name: troubleshoot
description: Diagnosing WinCraft VM problems - QEMU that will not start or stops, no KVM, a black or frozen screen, no desktop, no network or sound, a shutdown that hangs, a failed download or install. Use when a wcvm command fails or the person says the VM misbehaves.
---

# When something goes wrong

Look first:

- `./wcvm status`: running or not, live or installed, the desktop, the network.
- `./wcvm logs qemu`: QEMU's command line and its own messages.
- `./wcvm screenshot`: what the person sees. WinCraft shows its start-up on
  the screen; the serial port (`./wcvm logs serial`) only carries `wcvm`'s
  own shell sessions.
- Inside: `./wcvm sh 'tail -50 /run/log/desktop.log'`, `/run/log/Xorg.0.log`,
  `/run/log/net.log`, `dmesg | tail -30`.

| Symptom | Likely cause | What to do |
|---|---|---|
| doctor: QEMU missing, or too old | | the `fix:` command, run by the person; QEMU 6.2 or newer is needed |
| "QEMU stopped as it started" | an option or module this QEMU lacks; the log names it | `graphics=basic` for 3D, `audio=off`, `clipboard=off` for the clipboard, `share=` for the shared folder; start again |
| QEMU stops once the desktop draws | the host's GPU driver and 3D (virgl), e.g. some NVIDIA laptops | `./wcvm config graphics=basic` |
| Everything is very slow | no KVM: `status` says "without KVM" | doctor's fix for hardware virtualisation |
| "the VM's shell does not answer" | still starting, or the VM hangs | `./wcvm wait shell`; a screenshot; with the person's OK, `stop --force` |
| No desktop | the display server or the desktop failed | a screenshot; the desktop's and Xorg's logs; try `graphics=basic` |
| `stop` times out | a program asks to save its work | a screenshot, then answer the dialog with the person, or `stop --force` with their OK |
| No network in the VM | | `sh 'ip addr; cat /run/log/net.log'`; QEMU's network gives 10.0.2.15 |
| No sound | no sound server QEMU can use, or `audio=off` | doctor's Sound line; `./wcvm config audio=on`, restart |
| The download fails, or doesn't match its checksum | network, proxy, a broken download | run it again; else download the ISO from the GitHub releases by hand and `./wcvm setup --iso FILE` |
| The installer stops | its last line says why; a failed install saves a screenshot and names it | read it, fix the cause, then `./wcvm reinstall --yes` |
| Running, but no window | started headless, or the window is on another workspace | `status`; `./wcvm restart --window` |
| Drive H: missing | started before `share` was set, or restarted from inside | `./wcvm share`, or `./wcvm restart` |
| No screenshot with 3D graphics | an older QEMU, and no C compiler here | install what the error names, or `graphics=basic` |
| The virtual disk is full | its size is fixed | free space in the VM; or copy files out (`pull`), `destroy`, `setup --disk GB`, `install` |

When nothing helps: with the person's OK, `./wcvm stop --force` and start
again. A snapshot, if they took one, gets the disk back to a known state;
the boot menu's *without your features* entry (press a key at the WinCraft
boot menu, in the window) starts without features someone added.
