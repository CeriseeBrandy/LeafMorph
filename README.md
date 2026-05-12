# 🍃 LeafMorph

A premium minimalist desktop file conversion utility built with Qt6/C++.

LeafMorph focuses on fast local conversion with a clean modern UI inspired by native macOS/Linux utilities.

---

# ✨ Features

## 📂 File Conversion

Supports:

### 🖼️ Images

* PNG
* JPEG / JPG
* WEBP
* BMP
* TIFF
* ICO
* AVIF

### 🎵 Audio

* MP3
* WAV
* FLAC
* OGG
* AAC

### 🎬 Video

* MP4
* MKV
* AVI
* MOV
* WEBM

### 📄 Documents

* PDF
* DOCX
* ODT
* RTF
* TXT

---

# 🔒 Privacy First

LeafMorph is fully local.

Your files never leave your device.

No cloud.
No telemetry.
No tracking.

---

# ⚡ Features Included

* Drag & Drop support
* Multi-file conversion queue
* Metadata cleaning
* Lossless compression options
* File preview
* Output folder selection
* Modern glassmorphism UI
* Native Qt6 desktop experience

---

# 📄 Document Conversion

Document conversion uses LibreOffice in headless mode.

This allows LeafMorph to reliably convert Office and OpenDocument files while keeping everything fully offline.

## Required Dependency

Linux:

```bash
sudo dnf install libreoffice
```

or:

```bash
sudo apt install libreoffice
```

LeafMorph automatically uses LibreOffice in the background for document conversion.

---

# 🧰 Dependencies

## Linux

Required:

* Qt6
* FFmpeg
* ExifTool
* LibreOffice

Fedora:

```bash
sudo dnf install qt6-qtbase-devel ffmpeg perl-Image-ExifTool libreoffice
```

Ubuntu/Debian:

```bash
sudo apt install qt6-base-dev ffmpeg libimage-exiftool-perl libreoffice
```

---

# 🛠️ Build

```bash
git clone https://github.com/CeriseeBrandy/LeafMorph.git
cd LeafMorph

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Run:

```bash
./build/LeafMorph
```

---

# 📦 Planned Releases

Planned:

* Linux AppImage
* Windows portable build
* Dark mode
* Better packaging for dependencies

---

# 🖼️ Screenshots

Coming soon.

---

# 📜 License

MIT License

---

# ❤️ Credits

Built by CeriseeBrandy.
****
