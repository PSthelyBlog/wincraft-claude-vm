# WinCraft VM

[WinCraft OS](https://github.com/PSthelyBlog/wincraft) in a virtual machine on
your computer, run for you by [Claude Code](https://claude.com/claude-code).
You say what you want, and Claude does it with QEMU: "set it up", "start it",
"give it more memory", "share my Downloads folder", "install this program",
"take a snapshot", "is there an update?".

## Open it

You need Linux on an x86-64 computer, Claude Code and Python 3.8 or newer.
QEMU too, but Claude checks for it and tells you how to install it.

```
cd wincraft-vm
./wincraft-vm          # or simply: claude
```

The first time, Claude checks your computer and asks a few questions
(memory, disk size). It then downloads WinCraft OS, about 270 MB, checked
against the SHA-256 published with it, installs it on a virtual disk and
starts it in a window. Nothing on your own disks is touched: the VM lives in
`machine/`, in this folder.

Claude Code first asks whether you trust this folder. Say yes, so that it
may run its start-up hook and `./wcvm` without asking each time. It still
asks before anything that erases data: reinstalling, deleting the VM,
restoring or deleting a snapshot, powering off at once.

## What you can ask

| Ask | Claude runs |
|---|---|
| "start it", "stop it", "restart it" | `./wcvm start`, `stop`, `restart` |
| "give it 8 GB", "make the screen 1600x900", "no sound" | `./wcvm config mem=8192`, `res=1600x900`, `audio=off` |
| "share my Downloads folder" | `./wcvm config share=~/Downloads`: drive H: in WinCraft, read-only unless you allow more |
| "install ~/Downloads/setup.exe" | `./wcvm push`, then the installer, as you, in the VM |
| "what's on the screen?", "click OK" | `./wcvm screenshot`, `click` |
| "take a snapshot", "go back to it" | `./wcvm snapshot save`, `restore` |
| "is there an update?" | `./wcvm update check`, `update install` |
| "it won't start" | the logs, and a fix |

## By hand

`./wcvm` does everything Claude does; `./wcvm --help` lists it all. In
short: `doctor`, `setup`, `install`, `start`, `stop`, `status`, `config`,
`screenshot`, `sh`, `push`, `pull`, `snapshot`, `update`, `destroy`.

## How it works

- **`wcvm`**, one Python file with no dependencies, runs this computer's
  QEMU (`qemu-system-x86_64` and `qemu-img` on the PATH, or `$WCVM_QEMU` and
  `$WCVM_QEMU_IMG`), with KVM when it can, and 3D graphics (virgl) when
  QEMU and the GPU allow. It drives the VM through QEMU's QMP socket (the
  screen, the keyboard and the mouse) and through a root shell on the VM's
  serial port. A file channel, a virtio serial port, carries files and
  screenshots.
- That shell comes from WinCraft OS itself. When the host asks with an
  SMBIOS OEM string (`wincraft.shell`), its start-up opens root shells on the
  serial port and on the VM's second console (Ctrl+Alt+F2), as the boot
  menu's rescue entry does. Anyone at the VM's keyboard can be root: the VM
  is yours alone, as WinCraft OS assumes anyway. Programs in the VM cannot
  reach the serial shell, which only root may use.
- Installing runs WinCraft's own installer, the one behind *Install WinCraft
  OS*, through its system service as that window does. Updating runs WinCraft's
  updater, as *Update WinCraft* does.
- **Claude Code**: `CLAUDE.md` tells Claude its job and its rules;
  `.claude/settings.json` approves `./wcvm` in advance (except the commands
  that erase), runs `./wcvm hook session-start` at the start of each
  session, which gives Claude the VM's state, and shows the VM in the status
  line. The skills in `.claude/skills` cover the first setup, working inside
  WinCraft OS, and troubleshooting.
- `xshot.c` reads the VM's screen from inside, for QEMU versions that cannot
  take a screenshot of a 3D display; `wcvm` compiles it when needed.

## Files

| Path | Holds |
|---|---|
| `machine/config.json` | the settings (change them with `./wcvm config`) |
| `machine/disk.qcow2` | the virtual disk, with its snapshots |
| `machine/downloads/` | the WinCraft OS ISOs |
| `machine/run/` | QEMU's sockets, its log and the serial console's |
| `machine/screenshots/` | the latest 30 screenshots |

`./wcvm destroy --yes` deletes the VM and keeps the downloads; `--all`
deletes those too. Deleting this folder deletes everything.

## Limits

- Tested on Linux with KVM. macOS should work (QEMU from Homebrew, without
  3D graphics, and slowly on Apple silicon, where the processor is
  emulated), but is untested. Windows is not supported.
- The virtual disk's size cannot change once it is made.
- Snapshots are taken with the VM stopped, and do not include UEFI settings.
- After WinCraft restarts itself from inside, drive H: needs `./wcvm share`
  (Claude knows).
- WinCraft OS's own limits are in its
  [README](https://github.com/PSthelyBlog/wincraft#what-it-is-not).
