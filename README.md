# godl

[yeah yeah just skip to the installation](#installation-instructions)

<p align=center>
    <img width=128 alt="godl logo" src="https://github.com/sockeye-d/godl/blob/b98e3bd46a260c69a78c8e346d29c67909ccb2b7/src/godl.svg"/>
</p>

A version manager for Godot, written in QML/C++ with the Qt Quick libraries.

![The local versions page](assets/overview.png)

## It lets you manage...

...your projects, including adding tags with a fancy interface:

![managing tags](assets/tags.png)

*(you can also type arbitrary tags into the
combobox)*

...your versions:

![remote versions](assets/remote-versions.png)
![remote versions](assets/remote-versions-download.png)

...including first-class support for forks:
![forks](assets/forks.png)

...your local versions:

![local versions](assets/local-versions.png)

...and finally, your sanity!

## Windows build?

There is a Windows build and I think it works pretty well. However, most of my
testing is on Linux, so please file bugs if you find them! I've got a Windows VM
(it's how I built it for Windows after all...) so I can probably fix bugs.

## Installation instructions

### Windows

Download the latest release zip from the releases section, extract it, and run
`godl.exe` (might be just `godl` if you've got file extensions turned off).
If you want, you can then also right-click the executable, select "Create Shortcut,"
and then copy it to `C:\ProgramData\Microsoft\Windows\Start Menu\Programs`
(you'll need administrator rights to do this).

### Linux

You'll need to install these dependencies (at least — I haven't tried running it
on a non-Plasma desktop, so this is just what I got from ldd)

- kdeclarative
- kirigami-addons
- breeze-icons
- kirigami
- qqc2-desktop-style

Then, you should be able to download, extract, and run the executable.

## Templates

The create project page operates on templates, which you can find in
`~/.local/share/fishy/godl/templates` (Linux)
`%AppData%\fishy\godl\templates` (Windows).
You'll find the default template there, which if deleted will be regenerated on
app startup, but you can also create a new template by creating a new folder and
including a `metatemplate.json` file in it.

The metatemplate file defines which keys are available for replacement and how
they should be shown to the user. The basic structure is

```json
{
  "replacements": [
    // ...
  ]
}
```

A replacement key is an object that contains the following keys:

```json
{
  "type": /* ... */,
  "label": /* ... */
}
```

The available types are `header`, `string`, `multistring`, and `enum`. All the
types except `header` get another key called `template`; this is the key that
will get replaced in the source files of the template. For example, if you had
this file:

```gdscript
extends Node

func _ready() -> void:
  print("{node_name}")
```

and a metatemplate like this:

```json
{
  "replacements": [
    {
      "type": "string",
      "label": "Node name",
      "template": "node_name"
    }
  ]
}
```

when the template gets generated, the `{node_name}` in the script will be
replaced with whatever the user typed into the field labeled "Node name".

`multistring` is like `string`, except it gets a
[TextArea](https://doc.qt.io/qt-6/qml-qtquick-controls-textarea.html) instead of
a [TextField](https://doc.qt.io/qt-6/qml-qtquick-controls-textfield.html) so you
can enter multi-line strings.

`enum` allows you to define a ComboBox. It has a structure like this:

```json
{
  "type": "enum",
  "label": /* ... */,
  "template": /* ... */,
  "values": [
    {
      "label": /* the text that will be shown to the user */,
      "key": /* the value that will be inserted when the template gets generated */
    }
  ]
}
```

For example, a widget for selecting the renderer of a Godot 4 project:

```json
{
    "type": "enum",
    "label": "Renderer",
    "template": "renderer",
    "values": [
        {
            "label": "Forward+",
            "key": "forward_plus"
        },
        {
            "label": "Mobile",
            "key": "mobile"
        },
        {
            "label": "Compatibility",
            "key": "gl_compatibility"
        }
    ]
}
```

You can look at the default templates at [src/templates](src/templates/).

## Building from source

### Building on Windows

pre-build requisites:

- git
- Qt Creator, installed through the [Qt Online installer](https://www.qt.io/download-qt-installer-oss)
  - It does ask for you to make an account, it's really annoying, but QtC is the
  easiest way I've found 😔
- CMake & Ninja, which you can also install through the Qt Online installer
  - I'm pretty sure you don't actually need any Qt binaries from the installer,
  since we'll be building those ourselves with Craft
- rcedit, which you can get through winget: `winget install rcedit`

To be honest I'm not entirely sure how I got it building on Windows.
But basically,

1. Install [KDE Craft](https://community.kde.org/Get_Involved/development/Windows)
2. Make sure you've activated the craft environment!
3. Set the Qt version to 6.9.1 if it's still at 6.8.3 with
`craft --set version=6.9.1 libs/qt6`
4. Install libs/qt6 and dependencies with
`craft libs/qt6 kirigami kirigami-addons extra-cmake-modules karchive kiconthemes
kconfigwidgets qqc2-desktop-style qqc2-breeze-style`, and make sure to set MinGW
as the default compiler, not MSVC
5. Open Qt Creator, go to Preferences > Kits > Compilers, click add, then set up
the C compiler path to be the one you installed through Craft. It's normally at
"C:\CraftRoot\mingw64\bin\gcc.exe". Then, set the target triple to
"x86_64-w64-mingw32" (probably — I have no idea what this is/does).
![compiler settings](assets/building/windows/compiler.png)
6. Go to Qt Versions, click add, name it "Qt %{Qt:Version} (CraftRoot)", and set
the qmake path to the Craft qmake path (normally "C:\CraftRoot\bin\qmake.exe").
![qt versions](assets/building/windows/qt-versions.png)
7. Go to Kits (the tab), click add, name it "Craft Qt", and set the Qt version
to be the Craft version you just added. Set the name to be "Craft Qt"
![qt versions](assets/building/windows/kits.png)
8. Clone the repository with git: `git clone https://github.com/sockeye-d/godl`
9. Open the CMakeLists.txt file with Qt Creator. Once you're at the
configuration screen, select the new Craft Qt kit we just made.
Check Minimum Size Release and uncheck Debug.
![configure](assets/building/windows/configure.png)
10. Wait for the 'configuring "godl"' progress bar to complete, then press
Ctrl+R to run the project. With any luck, after 30 seconds, it should open.
If it looks like it didn't open but there weren't any errors, it might have just
opened behind other windows.
11. The build artifacts are copied to the `build/<target>/bin`. You can just copy
them from there, and zip them up, and distribute your freshly minted binaries.

### Building on Linux

Note: all of the package names are for Arch.

You'll need to install

- extra-cmake-modules
- qt6
- kdeclarative
- breeze-icons
- kirigami
- qtcreator
- qqc2-desktop-style

(I think)

If you get configure errors you probably need to install more stuff. Then, it's
basically the same thing, except Qt Creator should auto-detect the qmake on your
path and it should automatically set up the kit for you. You shouldn't need to
install QtC through the Qt Online installer — I did this and it broke a bunch of
stuff and I ended up uninstalling it and just installing it through the package
manager like I should have done to begin with.

Linux builds are just a single executable in the `build/<target>/deploy` path.

If you want the full experience, make a desktop entry like this one:

```ini
[Desktop Entry]
Name=godl
Exec={the executable}
Icon=godl
Type=Application
Terminal=false
Categories=Utility
```

and put it in `~/.local/share/applications/`,
and also copy [godl.svg](src/godl.svg) to
`/usr/share/icons/hicolor/scalable/apps/godl.svg` or similar.
