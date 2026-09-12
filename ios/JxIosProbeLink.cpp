//---------------------------------------------------------------------------
// [IOS 11/09] PHEP THU LINK - khong phai diem vao that cua game.
// Muc dich: ep trinh lien ket nap MOI tep .o cua moi thu vien (-all_load) de lo ra
//   (a) ky hieu TRUNG TEN giua cac thu vien - tren Android moi thu la .so nen loai loi nay
//       chi sai lang le luc chay (bai hoc GameScriptFuns, BANGIAO_ANDROID_PHA4_0809.md muc 9.1),
//       con tren iOS link tinh thi trinh lien ket bao to ngay luc dung;
//   (b) ky hieu THIEU - tuc nhung ham chi co o ban Android ma ban iOS chua cung cap.
// Diem vao that (JxIosMain.mm) la viec cua buoc C.
//---------------------------------------------------------------------------
extern int JxPosixMain(int argc, char* argv[]);   // KHONG extern "C": ban Android khai y het (JxAndroidMain.cpp:20)
extern "C" int main(int argc, char** argv) { return JxPosixMain(argc, argv); }
