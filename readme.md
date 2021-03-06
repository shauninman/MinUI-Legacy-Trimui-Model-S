# About MinUI

_MinUI is a custom launcher for the [Trimui Model S](http://www.trimui.com/) (aka the PowKiddy A66, sigh)._

I love this device. I think the hardware is great and that the official firmware has a lot of ambitious ideas. I even like its design. I just wish it were a more focused device. Focused on its strengths. I don't think playing console games on a 2" screen with frameskip at 12fps is a great experience. I think selling it as something that plays up to and including PS1 does it a disservice. Where it shines is portable consoles. Game Boy and Game Boy Color. Pokémon Mini. Neo Geo Pocket and Neo Geo Pocket Color. Game Gear. (Sorry Japanese friends, I have no experience with the Wonder Swan!) I love that it offers a consistent in-game menu with access to save states (and system-wide volume and brightness shortcuts). But I think locking players out of each emulator's unique settings was a mistake. MinUI aims to address those shortcomings. 

## Before you install

Because the stock firmware automatically recreates folders on the SD card I would recommend using a separate SD card for your MinUI installation. It's not necessary but it makes navigating the SD card on your computer or in Commander.pak a little easier because it's less cluttered. I will refer to these two SD cards as "stock" and "custom".

## Installing

Copy TrimuiUpdate_installMinUI.zip and TrimuiUpdate_uninstallMinUI.zip to the root of your stock SD card. Copy the contents of SDCARD onto your custom SD card. (If you are using a single SD card, copy everything onto it instead.) Add your roms to the Roms folder in the corresponding console folder. Insert your stock SD card into the device, power it on, navigate to FILE, and open TrimuiUpdate_installMinUI.zip. Once completed, power off the device, eject the stock SD card, insert your custom SD card, and power the device back on. (Unless you're using a single SD card.) Welcome to MinUI!

## Features

MinUI is a simple browser for launching roms. Every emulator that ships with MinUI has a custom in-game menu that also provides access to save states plus each emulator's default settings. That's kind of it. It is called _Min_UI after all.

## About paks

MinUI uses paks. A pak is just a folder with the pak extension containing a launch script. It may also contain an executable and resources required by that executable. 

When you open a rom from a console folder in Roms, MinUI runs the launch script in the corresponding pak in the Emus folder. For example, when you open /Roms/Game Boy/Tetris.gb, MinUI runs /Emus/Game Boy.pak/launch.sh which in turn launches /Emus/Game Boy.pak/gambatte-dms. You can rename the consoles in the Roms folder freely _but you must also rename the corresponding pak in the Emus folder._

Non-emulator programs live in Tools. MinUI ships with a Commander.pak (i.e. DinguxCommander) and Power Off.pak (to safely power off the device). Native games live in Games. MinUI does not ship with any native games.

## Additional paks

Any emulator, game, or program that will run on the device can be turned into a pak. Even the official ones. These additional paks are things that I don't personally keep on my device but might be perfectly serviceable or serve as an example to create new paks. <!-- TODO: add link -->

# For developers

I'd love for you to add the custom in-game menu to your emulator. It can be done in a way the same binary can run on non-MinUI devices too. Check out my libmmenu repo for simple implementation details. <!-- TODO: add link -->