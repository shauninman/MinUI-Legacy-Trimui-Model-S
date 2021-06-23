Just a quick proof of concept for a shared memory with persistence for settings not supported by stocks system.json

copy libmsettings.so to your LD_LIBRARY_PATH

basically just dumps a three member struct to memory/file

noticed the device doesn't have a /dev/shm it writes to /tmp instead (same difference)

if we modify the Settings struct we make a copy and name it Settings_v1 then when loading we check the version and translate any values from older version to the current version of Settings

could we add an SetSettingCallback() that calls a function with key,oldValue,newValue args? that would be better than polling
