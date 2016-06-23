@echo off
REM Known bug from forfiles. Need double put .exe name when invoking execution command
forfiles /m *.ttf /s /c "MakeSpriteFont.exe MakeSpriteFont.exe /FontSize:28 /DebugOutputSpriteSheet:@file.bmp @file ../../../FullSail/Resources/GUI/Fonts/@fname.spritefont"
