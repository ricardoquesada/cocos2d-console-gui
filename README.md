# cocos2d-console-gui
cocos launcher

# How to test it

0. Download [`ricardoquesada's templates_fix`](https://github.com/ricardoquesada/cocos2d-x/tree/template_fixes) cocos2d-x branch and make sure that `cocos` is the path
1. Compile Cocos2d-Console-GUI or download the [precompiled binary](https://www.dropbox.com/s/dhbrku4g7rrbty5/Cocos2d-Console-GUI-v0.0.3.dmg?dl=1)
2. Copy cocos2d-console-gui to the `cocos2d-x/tools/cocos2d-console/bin` directory
3. Use [`ricardoquesada's cocos2dproj`](https://github.com/ricardoquesada/cocos2d-console/tree/cocos2dproj) cocos-console branch
4. Make sure to use latest [sdkbox.py](https://github.com/chukong/cocos-service-center/)

## Download ricardoquesada's cocos2d-x
```
$ git clone https://github.com/ricardoquesada/cocos2d-x.git
$ cd cocos2d-x
$ git checkout template_fixes
```

## Compile Cocos2d-Console-GUI...

* Download Cocos2d-Console-GUI: `git clone https://github.com/ricardoquesada/cocos2d-console-gui/`
* Download latest [QtCreator](http://www.qt.io/download/)
* Open `src/cocos2d-console-gui.pro` file from QtCreator
* Compile it

## ... or Download pre-compiled binary

. [Cocos2d-Console-GUI v0.0.3.dmg](https://www.dropbox.com/s/dhbrku4g7rrbty5/Cocos2d-Console-GUI-v0.0.3.dmg?dl=1)

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
$ git clone https://github.com/chukong/cocos-service-center.git
$ cd cocos-service-center/tools/installer/sdkbox
$ ./build.py
$ cp -R bin/* ~/cocos2d-x/tools/cocos2d-console/bin
```

## Run the Console GUI

```
$ cocos gui
```
