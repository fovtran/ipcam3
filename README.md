IP Cam recording and object group editing tool
=============

This application has the basic structure for a laboratory snapshotting and video recording tool for multiple
videocams using the IPcam protocol (MJPEG).

etcetera

1. This application operative tools are...
1. For adding OpenCV routines please view x file... etcetera


Please see our [contributing guidelines](CONTRIBUTING.md) before reporting an issue.

Markups
-------

The following markups are supported.  The dependencies listed are required if
you wish to run the library. You can also run `script/bootstrap` to fetch them all.

* [.markdown, .mdown, .mkdn, .md](http://daringfireball.net/projects/markdown/) -- `gem install commonmarker` (https://github.com/gjtorikian/commonmarker)
* [.textile](https://www.promptworks.com/textile) -- `gem install RedCloth` (https://github.com/jgarber/redcloth)



Installation
-----------

```
Just unzip
```

Configuration
-----------
```
Edit config.xml for ipcam target ip address
```

Usage
-----

Basic form:

```Windows
Execute Run.bat

```Other
Python ipcam.py

GitHub::Markup.render('README.markdown', "* One\n* Two")
```

Troubleshooting
-------------
```
- Remove config.ini file when application hangs too many times
- Verify network speed to target device

Contributing
------------

See [Contributing](CONTRIBUTING.md).
