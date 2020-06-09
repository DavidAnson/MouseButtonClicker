# MouseButtonClicker

> MouseButtonClicker clicks the mouse so you don't have to!

## Motivation

_The nearly universal pattern of mouse use is: move/click/wait... move/click/wait... move/click/wait...._
_In the vast majority of cases, the only reason the mouse moves is to position the pointer over the next user interface element that needs to be clicked._
_Because every move is immediately followed by a click, it's possible to simplify the process by performing the click automatically when the mouse stops moving (i.e., it moves to a new location and stays still for a few moments)._
_This automatic click saves a tiny bit of effort and eliminates an unnecessary, repetitive motion that's carried out many, many times over the course of a day._
_For added ergonomic benefit, automatic clicking lets the user hold the mouse in a variety of new ways because it's no longer necessary to keep a finger over the mouse button._

MouseButtonClicker is a simple Windows utility that implements automatic clicking.
If you're on a different platform, there are links below to similar tools for other operating systems.

## Use

1. Download the latest ZIP from the [Releases page](https://github.com/DavidAnson/MouseButtonClicker/releases)
2. Unzip the `x86` (32-bit) or `x64` (64-bit) version according to your OS
3. Run `MouseButtonClicker.exe`
4. Move the mouse and wait a moment for the automatic click

There's no user interface for MouseButtonClicker.
When it's running, it waits for the mouse to stop moving, then clicks the primary mouse button.
If you click any button first or scroll the mouse wheel, that suppresses the automatic click.
If you want to stop automatic clicking, run `taskkill /IM MouseButtonClicker.exe` or find MouseButtonClicker in Task Manager's "Details" tab and end the task.
(When run on a host OS, MouseButtonClicker works for virtual machine guests; otherwise, run it in each guest OS.)

## Compiling

1. Install [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)
2. Install the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/)
3. Open a `Developer Command Prompt for VS 2019`
4. Clone the repository from <https://github.com/DavidAnson/MouseButtonClicker>
5. Run `BuildAll.cmd`

## References

- ["MouseButtonClicker clicks the mouse so you don't have to!" [Releasing binaries and source for a nifty mouse utility]](https://dlaa.me/blog/post/8964592)
- [Control the pointer using Dwell on Mac](https://support.apple.com/guide/mac-help/control-the-pointer-using-dwell-mchl437b47b0/mac)
- [Automatically click objects on your Chromebook](https://support.google.com/chromebook/answer/9441453?hl=en)
- [Simulate clicking by hovering](https://help.ubuntu.com/stable/ubuntu-help/a11y-dwellclick.html.en) (Ubuntu Linux)
- [C/C++ Building Reference](https://docs.microsoft.com/en-us/cpp/build/reference/c-cpp-building-reference?view=vs-2019)
