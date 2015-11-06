# cocos2d-console-gui

# How to test it

0. Download [`ricardoquesada's template_2`](https://github.com/ricardoquesada/cocos2d-x/tree/template_2) cocos2d-x branch and make sure that `cocos` is the path
1. Download and compile the Cocos2d-Console-GUI source code or download the precompiled binaries (see below)
2. Copy cocos2d-console-gui to the `cocos2d-x/tools/cocos2d-console/bin` directory
3. Use [`ricardoquesada's cocos2dproj`](https://github.com/ricardoquesada/cocos2d-console/tree/cocos2dproj) cocos-console branch
4. Make sure to use latest [sdkbox.py](https://github.com/sdkbox/sdkbox-core)

## Download ricardoquesada's cocos2d-x
```
$ git clone https://github.com/ricardoquesada/cocos2d-x.git
$ cd cocos2d-x
$ git checkout template_2
```

## Download and Compile Cocos2d-Console-GUI source code...

* Download Cocos2d-Console-GUI: `git clone https://github.com/ricardoquesada/cocos2d-console-gui/`
* Download latest [QtCreator](http://www.qt.io/download/)
* Open `src/cocos2d-console-gui.pro` file from QtCreator
* Compile it

## ... or Download pre-compiled binaries

. [Mac: Cocos2d-Console-GUI v0.0.5.dmg](https://www.dropbox.com/s/8hxyhyibm6bitmv/cocos2d-console-gui-v0.0.5.dmg?dl=1)
. [Win32: Cocos2d-Console-GUI v0.0.5.zip](https://www.dropbox.com/s/b8oavs231bnm03i/cocos2d-console-gui-v0.0.5.zip?dl=1)

## Copy Cocos-console-gui to bin

```
$ cp -R ~/Download/Cocos2d-Console-GUI.app ~/Users/cocos2d-x/tools/cocos2d-console/bin
```

## Use ricardoquesada's cocos-console.py version

```
$ cd ~/Users/cocos2d-x/tools/cocos2d-console
$ git remote add ricardoquesada https://github.com/ricardoquesada/cocos2d-console.git
$ git pull ricardoquesada cocos2dproj
```

## Use latest sdkbox.py version and build it and replace old one

```
$ git clone https://github.com/sdkbox/sdkbox-core
$ cd sdkbox-core/tools/installer/sdkbox
$ ./build.py
$ cp -R bin/* ~/cocos2d-x/tools/cocos2d-console/bin
```

## Run the Console GUI

```
$ cocos gui
```

## Known bugs

. "Stop" button doesn't actually stop the compilation of Mac/iOS builds. The compilation keeps running as a background process. This bug, actually belongs to cocos.py
. ".cocosproj" files are not associated with "cocos2d-cocos2d-gui" on Windows. But they are on Mac


## How to add Chinese translations

. Download Qt Linguist, which is part of [Qt](http://www.qt.io/download/)
. edit the file [translations.pro](https://github.com/ricardoquesada/cocos2d-console-gui/blob/master/translations/translations.pro)
. ... and add the `cn` in `LANGUAGES`, right after `es`
. Then copy `cc_console_gui_es.ts` to `cc_console_gui_cn.ts` and edit it with Linguist
. Then send me a PR with those changes... done.
