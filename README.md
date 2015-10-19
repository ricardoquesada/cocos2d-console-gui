# cocos2d-console-gui
cocos launcher

# How to test it

1. Compile it or download the [precompiled binary](https://www.dropbox.com/s/toqicyztydn50iu/Cocos2d-Console-GUI-v0.0.1.dmg?dl=1)
2. Copy cocos2d-console-gui to the `cocos2d-console/bin` directory
2. Make sure to use the latest version of cocos-console
3. Make sure to use latest sdkbox.py 

## Compile ...

. Download latest [QtCreator](http://www.qt.io/download/)
. Open 

## ... or Download pre-compiled binary

. [Cocos2d-Console-GUI v0.0.1.dmg](https://www.dropbox.com/s/toqicyztydn50iu/Cocos2d-Console-GUI-v0.0.1.dmg?dl=1)

## Copy Cocos-console-gui to bin

```
$ cp -R ~/Download/Cocos2d-Console-GUI.app ~/Users/cocos2d-x/tools/cocos2d-console/bin

## Use latest cocos-console.py version

```
$ cd ~/Users/cocos2d-x/tools/cocos2d-console
$ git pull origin v3
```

## Use latest sdkbox.py version and build it and replace old one

```
$ git clone https://github.com/chukong/cocos-service-center.git
$ cd cocos-service-center/tools/installer/sdkbox
$ ./build.py
$ cp -R bin/* ~/cocos2d-x/tools/cocos2d-console/bin
```

## Run it.

```
$ cocos gui
```
