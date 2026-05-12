# LeafMorph — Qt 6 Desktop File Converter

A premium glassmorphic desktop utility built with C++17 + Qt 6 Widgets.

## Requirements

- Qt 6.2+ (Widgets module)
- CMake 3.16+
- GCC 10+ or Clang 12+ (C++17)
- Optional: ffmpeg (for real conversions)
- Optional: exiftool (for metadata stripping)

---

## Build on Linux (Ubuntu/Debian)

### 1. Install Qt 6

```bash
sudo apt update
sudo apt install qt6-base-dev cmake build-essential
```

Or via the Qt online installer at https://www.qt.io/download-qt-installer

### 2. Install optional conversion tools

```bash
sudo apt install ffmpeg libimage-exiftool-perl
```

### 3. Clone / unzip the project

```bash
# Assuming the folder is already at ~/LeafMorph
cd ~/LeafMorph
```

### 4. Configure & build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### 5. Run

```bash
./build/LeafMorph
```

---

## Build on Arch / Manjaro

```bash
sudo pacman -S qt6-base cmake gcc ffmpeg perl-image-exiftool
cmake -B build && cmake --build build -j$(nproc)
./build/LeafMorph
```

---

## Build on Fedora

```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++ ffmpeg perl-Image-ExifTool
cmake -B build && cmake --build build -j$(nproc)
./build/LeafMorph
```

---

## Build on macOS (Homebrew)

```bash
brew install qt cmake ffmpeg exiftool
export PATH="/opt/homebrew/opt/qt/bin:$PATH"
cmake -B build && cmake --build build -j$(sysctl -n hw.ncpu)
./build/LeafMorph
```

---

## Build on Windows (MSVC + Qt 6)

1. Install Qt 6 via Qt online installer (select MSVC 2022 component)
2. Install CMake and Visual Studio 2022
3. Open Qt 6 command prompt:

```cmd
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
build\Release\LeafMorph.exe
```

---

## Project structure

```
LeafMorph/
├── CMakeLists.txt      # Build configuration
├── main.cpp            # Entry point
├── MainWindow.h        # All widget class declarations
├── MainWindow.cpp      # Full implementation
└── resources.qrc      # Qt resource bundle (extensible)
```

## Key design decisions

| Feature | Implementation |
|---|---|
| Frameless window | `Qt::FramelessWindowHint` + mouse drag override |
| Glass card | `GlassCard` custom widget with layered QPainter gradients |
| Leaf icon | `LeafIcon` drawn entirely in QPainter (no PNG needed) |
| Toggle switches | `ToggleSwitch` with `QPropertyAnimation` on thumb position |
| Drop zone | `QWidget` with `setAcceptDrops(true)` + painted dashes |
| Background scene | `MainWindow::paintEvent` — radial blobs + sphere shaders |
| Conversion backend | `QProcess` → ffmpeg + exiftool (graceful fallback if absent) |
