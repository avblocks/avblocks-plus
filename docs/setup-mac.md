# Setup for macOS

> Scripts are `bash`

## Xcode

Install Command Line Tools for Xcode:

```bash
xcode-select --install
```

## Homebrew

Install Homebrew:

```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

## cmake, ninja

Install via Homebrew:

```bash
brew install cmake ninja
```

## Visual Studio Code

> The project comes with a pre-configured Visual Studio Code workspace which uses CMake and Ninja as a build system. That is the preferred way to build and work with the code.   

Download and install from [Visual Studio Code](https://code.visualstudio.com/download) site.

Open Visual Studio Code and press `Cmd + Shift + p`. 

Select `Shell Command: Install 'code' command in PATH`. 

