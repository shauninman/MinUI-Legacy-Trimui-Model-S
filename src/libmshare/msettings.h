#ifndef msettings_h__
#define msettings_h__

void InitSettings(void); // host/client
void QuitSettings(void); // host-only

int GetBrightness(void);
int GetVolume(void);

void SetRawBrightness(int value); // 0-120
void SetRawVolume(int value); // 0-100%

void SetBrightness(int value);
void SetVolume(int value);

#endif  // msettings_h__
