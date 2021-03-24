MinUI is a minimal launcher for the Trimui Model S

Source: https://github.com/shauninman/minui

----------------------------------------
Features

- No settings or configuration
- A no frills file browser with an
  almost 1:1 relationship to the 
  contents of your SD card
- Quick access to save states and 
  full access to each emulator's 
  unique settings
- Instant sleep, just press MENU in
  the file browser or menu (hold L 
  and R and then press any other 
  button to wake)
- Emulator defaults curated to match
  the unique strengths and limitations
  of the device

----------------------------------------
Roms

Included in this zip is a Roms folder containing folders for each console MinUI currently supports. MinUI will create these folders for you when installed or updated but they'll be empty. You should probably preload them with roms and copy each folder to the Roms folder on your SD card before installing. Or not, I'm not the boss of you.

----------------------------------------
Install

Without unzipping it, copy TrimuiUpdate_MinUI.zip to the root of your SD card. Insert your SD card into the device, power it on, navigate to FILE, and open TrimuiUpdate_MinUI.zip. Welcome to MinUI.

A quick note: The MinUI installer is also an updater and becomes a launcher once installed. So don't remove it from your SD card. 

Once an update is available, just replace TrimuiUpdate_MinUI.zip with the latest version. From the stock UI, updating is the same as installing or launching. From MinUI, just launch Update, which will appear below Tools when an update is detected on the SD card.

----------------------------------------
Return to stock

Navigate to Tools and launch Stock UI. See you soon!

While unlikely to occur, if you ever have a problem with MinUI and cannot boot or return to stock, delete the hidden .tmp_update directory at the root of your SD card using your computer.

----------------------------------------
Return to MinUI

Navigate to FILE and open TrimuiUpdate_MinUI.zip. Welcome back!

----------------------------------------
Thanks

To eggs for his early and frequent hacking and porting, LCD initialization code, screen tearing patch, under- and over-clocking; the list continues to grow every day, but above all for sharing these constant discoveries.

Check out eggs' releases: https://www.dropbox.com/sh/5e9xwvp672vt8cr/AAAkfmYQeqdAalPiTramOz9Ma?dl=0

To neonloop for putting together the Trimui buildroot and discovering that the device shipped with adbd, which together turned hacking on the device into developing for the device, literally overnight.

Check out neonloop's repos: https://git.crowdedwood.com

And to the entire Retro Game Handhelds Trimui Model S and dev Discord community for their enthusiasm and encouragement. 

Checkout the channel on the Retro Game Handhelds Discord: https://discord.com/channels/529983248114122762/780563189434941460