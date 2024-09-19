#ifndef DetectWindowsTitle_H
#define DetectWindowsTitle_H
#include <fstream>
#include <string>

void Msg_Windows_Title();
bool TitleWindows(LPCSTR WindowTitle);
void DetectWindowsTitleScanner();
void DetectWindowsTitleScan();
void ThreadDetectWindowsTitle();
#endif