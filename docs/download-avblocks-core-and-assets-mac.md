# Download AVBlocks Core and Assets on macOS

Change to the directory where you cloned this repository:

```bash
cd avblocks-plus
```

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

Builds are available for Apple Silicon (`darwin-arm64`) and Intel (`darwin`, x64). Use `uname -m` to check your machine: `arm64` means Apple Silicon, `x86_64` means Intel.

```bash
# select version and platform
# "darwin-arm64" for Apple Silicon (M1/M2/M3/M4/M5), "darwin" for Intel (x64)
tag="v3.4.0-demo.1"
platform="darwin-arm64"

# download
mkdir -p ./sdk
cd ./sdk

# sdk
curl \
  --location \
  --output ./avblocks-$tag-$platform.zip \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip
  
# sha256 checksum
curl \
  --location \
  --output ./avblocks-$tag-$platform.zip.sha256 \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip.sha256

# verify sha256 checksum
shasum --check ./avblocks-$tag-$platform.zip.sha256

# unzip
unzip avblocks-$tag-$platform.zip

cd ..
```

## Assets

These demo audio and video assets are used as input for the AVBlocks samples.

```bash
mkdir -p ./assets
cd ./assets

curl \
  --location \
  --output ./avblocks_assets_v5.zip \
  https://github.com/avblocks/avblocks-assets/releases/download/v5/avblocks_assets_v5.zip
  
# unzip
unzip avblocks_assets_v5.zip

cd ..
```
