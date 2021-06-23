#ifndef msettings_h__
#define msettings_h__

void InitSettings(void);
void QuitSettings(void);

int GetBrightness(void);
int GetVolume(void);

void SetRawBrightness(int value); // 0-120
void SetRawVolume(int value); // 0-100%

void SetBrightness(int value); // 0-10
void SetVolume(int value); // 0-20

#endif  // msettings_h__
