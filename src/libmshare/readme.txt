Just a quick proof of concept for a shared memory with persistence for settings not supported by stocks system.json

copy libmsettings.so to your LD_LIBRARY_PATH

basically just dumps a three member struct to memory/file

noticed the device doesn't have a /dev/shm it writes to /tmp instead (same difference)