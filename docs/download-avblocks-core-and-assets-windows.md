# Download AVBlocks Core and Assets on Windows

> Scripts are PowerShell

Change to the directory where you cloned this repository:

```bash
cd avblocks-plus
```

## Download script

Run [download-sdk-and-assets.ps1](../download-sdk-and-assets.ps1) from the repository root. It detects the host architecture (`arm64` or `x64`), downloads the matching SDK into `sdk/` and the demo assets into `assets/`:

```powershell
./download-sdk-and-assets.ps1
```

Alternatively, follow the manual steps below.

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

Builds are available for ARM64 (`windows-arm64`) and x64 (`windows`). Use `$env:PROCESSOR_ARCHITECTURE` to check your machine: `ARM64` means ARM64, `AMD64` means x64.

```powershell
# select version and platform
# "windows-arm64" for ARM64, "windows" for x64
$tag='v3.4.0-demo.1'
$platform='windows'

# download
new-item -Force -ItemType Directory ./sdk
cd ./sdk

# sdk
curl.exe `
  --location `
  --output ./avblocks-$tag-$platform.zip `
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip
  

# sha256 checksum
curl.exe `
  --location `
  --output ./avblocks-$tag-$platform.zip.sha256 `
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip.sha256

# verify checksum
$downloadedHash = (Get-FileHash -Algorithm SHA256 ./avblocks-$tag-$platform.zip).Hash.ToLower()
$expectedHash = (Get-Content ./avblocks-$tag-$platform.zip.sha256).Split(' ')[0].ToLower()
if ($downloadedHash -eq $expectedHash) { Write-Host "Checksum OK!"; } else { Write-Host "Checksum failed!"; }

# unzip
expand-archive -Force -Path avblocks-$tag-$platform.zip -DestinationPath .

cd ..
```

## Assets

These demo audio and video assets are used as input for the AVBlocks samples.

```powershell
new-item -Force -ItemType Directory ./assets
cd ./assets

curl.exe `
  --location `
  --output ./avblocks_assets_v5.zip `
  https://github.com/avblocks/avblocks-assets/releases/download/v5/avblocks_assets_v5.zip
  
# unzip
expand-archive -Force -Path avblocks_assets_v5.zip -DestinationPath .

cd ..
```

