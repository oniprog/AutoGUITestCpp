#coding : UTF-8

UT_SetChekcBox(0)
UT_Sleep(1)
UT_CaptureScreen(u"test1.png")

UT_SetRadio(1)
UT_Sleep(1)
UT_CaptureScreen(u"test2.png")

UT_SetEditBox(u"test 2")
UT_Sleep(1)
UT_CaptureScreen(u"test3.png")

UT_SetComboBox(1)
UT_Sleep(1)
UT_CaptureScreen(u"test4.png")

UT_SetPicture(1)
UT_Sleep(1)
UT_CaptureScreen(u"test5.png")

UT_Sleep(5)
UT_Terminate()
