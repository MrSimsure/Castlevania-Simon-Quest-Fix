## Simon's Quest Fix

This is a simple program to mod Simon's Quest of the Castlevania Anniversary Collection on Steam for Windows.
It does two things:

- Reducing the dialog characters appear time, making them faster
- Reducing the day/night transiction time

This mod started from [this tweet of Foone](https://twitter.com/Foone/status/1129199721656963073), they discovered that when you start one of the games from the steam version, the clear rom get loaded in the ram, from there you can edit it and actually mod the game.

So after some testing and a looking to some existing rom hack of Simon's Quest, particulary [Simon's Quest Redaction](http://www.thealmightyguru.com/Games/Hacking/Hacks/SimonsRedaction.html), i figured out wich bytes used to be the ones to control the dialog speed and day/night transiction and made a small program in c++ to change them.

This code is pretty raw, i made it some years ago, i didn't even used c++ before, but in the end it worked.

## How to build

The project consist in a single .cpp file, it use only the standard library and some windows header, so it can actually be built it with most c++ compiler.\
The folder contains a Visual Studio solution, so you can open and build it with Visual Studio, or use the build.bat file to automaticaly build the project, it requires a copy of MinGW installed to use g++ as a compiler.\
Both Visual Studio and the build.bat file will output the final binary in the ./bin folder.
