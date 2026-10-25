---
name: setup
description: First launch of WinCraft VM - check this computer, settle the VM's settings with the person, download WinCraft OS, install it on the virtual disk and start it. Use when the VM is not set up yet or not installed yet, or when the person asks to set it up or to start over.
---

# Setting up WinCraft VM

Guide the person through it in short messages. They should end with WinCraft
OS on their screen and an idea of what they can ask you next. Skip the steps
already done (`./wcvm status` tells).

## 1. Say what will happen

In two or three sentences: you will check their computer, download WinCraft
OS (about 270 MB, from its GitHub releases, checked against the checksum
published with it), make a virtual disk that only takes the space actually
used, install the system on it and start it in a window. Nothing on their own
disks is touched; everything stays in this folder.

## 2. Check the computer

Run `./wcvm doctor` and explain the result plainly, without listing every
line.

- Something marked ✗ (usually QEMU missing): give them the `fix:` command, to
  run in a terminal of their own since it asks for their password. Wait until
  they say it is done, then run `./wcvm doctor` again. With no `fix:` line,
  find the package for their system and say how sure you are.
- No hardware virtualisation (KVM): the VM still works, many times slower.
  Give the fix and let them choose to fix it first or go on.
- Warnings about 3D graphics, sound, UEFI or the clipboard are optional:
  one line about what they would miss, no more.

## 3. Settle the settings

Ask with AskUserQuestion, recommended values first (doctor's last line):

- memory and processor cores, e.g. "4 GB and 4 cores (Recommended)", a
  smaller and a larger choice;
- the virtual disk's size: 32 GB recommended; it grows as it fills, up to
  that size, and cannot be changed later;
- 3D graphics when doctor found them (recommended; games need them).

Don't ask about BIOS or UEFI, screen size, sound or windows unless they bring
them up.

## 4. Set up

Run `./wcvm setup --mem MB --cpus N --disk GB` (`--graphics basic` if they
said no to 3D) with `run_in_background`. It downloads the ISO and checks its
SHA-256, then makes the disk. Say it is downloading; when it ends, say which
version they got.

## 5. Install

Ask how, with AskUserQuestion:

- **"Install it for me" (Recommended)**: `./wcvm install` with
  `run_in_background`. It starts the ISO without a window, runs WinCraft's
  installer on the virtual disk (all of it; the disk is blank) and shuts
  down. Under a minute with KVM.
- **"I'll go through the installer myself"**: `./wcvm start --live --wait`.
  Tell them to open *Install WinCraft OS* on the desktop, choose the only
  disk, confirm, and restart from the Start menu when it says so; the VM
  then starts from the disk. Afterwards `./wcvm status` notes that it is
  installed.
- **"Just try it first"**: `./wcvm start --live --wait`. Nothing is kept
  when it stops; they can ask you to install it any time.

## 6. Start it

`./wcvm start --wait`. When it says the desktop is up, take a screenshot,
look at it, and tell them it is ready, in the window that opened.

## 7. Hand over

Tell them briefly how to use it and what they can ask you, for example:

- "stop the VM", "start it again"
- "give it more memory", "make the screen bigger", "turn the sound off"
- "share my Downloads folder with it" (it becomes drive H: in WinCraft)
- "install this program: ~/Downloads/setup.exe"
- "take a snapshot before I try something", "go back to the snapshot"
- "is there a WinCraft update?"

And that closing the window turns the VM off at once, like pulling the plug:
better to ask you to stop it.

If a step fails, read the `troubleshoot` skill, explain the problem simply,
and fix it or say what you need from them.
