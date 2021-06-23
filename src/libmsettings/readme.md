# Usage:

1. link `-lmsettings` (probably `-ltinyalsa` too?)
2. include `<msettings.h>`
3. call `InitSettings()`
4. get brightness and volume with `GetBrightness()` and `GetVolume()`
5. set with `SetBrightness(0-10)` and `SetVolume(0-20)`
6. to set without saving the new values call `SetRawBrightness(0-120ish)` or `SetRawVolume(0-100%)` (eg. for sleep or restoring on launch)
7. call `QuitSettings()` when you're done

The shared memory is created in `/tmp/SharedSettings` (there is no `/dev/shm` on this device I guess?) and settings are persisted in `/mnt/settings.bin`.

The volume functions will set, store, and return the appropriate value depending on whether headphones are plugged in or not.

To test copy `libmsettings.so` to `/System/lib` and `Settings.pak` to `/Tools`.

When you launch `Settings.pak` it launches a keymon-like process and a GUI that displays the current values that change in real time. The keymon-like changes brightness with UP/DOWN and volume with X/B. Plugging and unplugging headphones will show the volume switching between the separate speaker and headphones settings.

Rudimentary future proofing is in place. The Settings struct's first member is `int version` and has 4 unused slots for an even 8 members. If we need more than that for some reason we can bump the version number, rename the old Settings to Settings_v1, and have some logic to handle converting old to new.
