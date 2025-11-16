# Build on Linux

> Scripts are `bash`

The project comes with a pre-configured Visual Studio Code workspace which uses `cmake` and `ninja` as a build system. 

## Setup

Make sure you have done the [setup for Linux](./setup-linux.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

#### Configure

```bash
source ./configure.sh
```

#### Build

```bash
invoke build --type release
```

#### Clean

```bash
invoke clean --type release
```

#### Edit

> Start Visual Studio Code. Make sure you install the recommended workspace extensions when asked.

```bash
code .
```

