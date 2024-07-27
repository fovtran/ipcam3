* QT_QPA_PLATFORM=offscreen /tmp/pyenv/bin/python3 ipcam.py
* fix 1: use os.environ.pop("QT_QPA_PLATFORM_PLUGIN_PATH") in code
* export QT_DEBUG_PLUGINS=1

* strace -e openat -e status=failed  python3 ipcam.py

* ldd /usr/lib/qt/plugins/platforms/libqxcb.so
* ldd /usr/lib/x86_64-linux-gnu/qt5/plugins/platforms/libqxcb.so 
* ldd /tmp/pyenv/lib/python3.11/site-packages/PyQt5/Qt5/plugins/platforms/libqxcb.so | grep -i "not found"
* strings /usr/lib/x86_64-linux-gnu/libQt5XcbQpa.so.5 | grep Qt | more

* apt-get install libxcb-randr0-dev libxcb-xtest0-dev libxcb-xinerama0-dev libxcb-shape0-dev libxcb-xkb-dev
* apt install libxcb-cursor0

```
Put qt.conf near to the executable with next lines:
[Paths]
Prefix = /tmp/pyenv/lib/python3.11/site-packages/PyQt5/Qt5/
```

* git config --global user.email "diego@d.c"
* git config --global user.name "Diego"