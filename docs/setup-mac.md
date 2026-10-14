# Setup for macOS

> Scripts are `bash`

## Apple Silicon (arm64) and Intel (x64)

Both architectures are supported. Check your machine with `uname -m`: `arm64` is Apple Silicon, `x86_64` is Intel. Download the matching AVBlocks Core build (see [Download AVBlocks Core and Assets on macOS](./download-avblocks-core-and-assets-mac.md)).

## Xcode

Install Command Line Tools for Xcode:

```bash
xcode-select --install
```

## Homebrew

Install Homebrew:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

On Apple Silicon Homebrew installs to `/opt/homebrew`. Add it to your `PATH` as the installer instructs, e.g.:

```bash
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
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

