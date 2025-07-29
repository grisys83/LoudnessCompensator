# Windows Installation Guide - LoudnessCompensator Plugin

## System Requirements

- **Operating System**: Windows 10 or later (64-bit)
- **Architecture**: x64 (Intel/AMD 64-bit processors)
- **DAW**: Any VST3-compatible Digital Audio Workstation
- **RAM**: Minimum 4GB (8GB+ recommended)
- **Disk Space**: ~50MB for plugin installation

## Supported DAWs

The plugin has been tested with:
- **Reaper** (Recommended - excellent VST3 support)
- **Cubase/Nuendo** (Steinberg - VST3 creators)
- **Studio One** (PreSonus)
- **FL Studio** (Image-Line)
- **Ableton Live** (VST3 support in Live 10+)
- **Pro Tools** (VST3 support in 2021.6+)
- **Waveform** (Tracktion)
- **Cakewalk by BandLab** (Free DAW with VST3 support)

## Installation Steps

### Method 1: Automatic Installation (Recommended)

1. **Download** the latest Windows release:
   - Go to: https://github.com/grisys83/LoudnessCompensator/releases
   - Download `LoudnessCompensator-v1.0.0-Windows.zip`

2. **Extract** the downloaded ZIP file to a temporary folder

3. **Run** the installer (if provided) or follow manual installation below

### Method 2: Manual Installation

1. **Download and Extract** as above

2. **Copy VST3 Plugin** to one of these locations:

   **System-wide installation (recommended):**
   ```
   C:\Program Files\Common Files\VST3\
   ```
   
   **User-specific installation:**
   ```
   C:\Users\[YourUsername]\AppData\Roaming\VST3\
   ```

3. **Folder Structure** should look like:
   ```
   VST3\
   └── LoudnessCompensator.vst3\
       └── Contents\
           ├── x86_64-win\
           │   └── LoudnessCompensator.vst3
           └── Resources\
               └── (plugin resources)
   ```

4. **Restart your DAW** completely (close and reopen)

5. **Rescan VST3 plugins** in your DAW:
   - **Reaper**: Options → Preferences → Plug-ins → VST → Re-scan
   - **Cubase**: Studio → VST Plug-in Manager → Refresh
   - **Studio One**: Studio One → Options → Locations → VST Plug-ins → Scan
   - **FL Studio**: Options → Manage Plugins → Find Plugins
   - **Ableton Live**: Options → Preferences → Plug-ins → VST3 → Rescan

## Troubleshooting

### Plugin Not Appearing in DAW

**Check Installation Path:**
```cmd
dir "C:\Program Files\Common Files\VST3\LoudnessCompensator.vst3"
```

**Alternative Installation Paths to Try:**
- `C:\Program Files\VSTPlugins\` (VST2 - not applicable)
- `C:\Program Files\Steinberg\VSTPlugins\` (Legacy)
- Check your DAW's custom VST3 scan paths

**Force Plugin Rescan:**
1. Close your DAW completely
2. Delete plugin cache files (DAW-specific locations)
3. Restart DAW and force rescan

### Access Permission Issues

If you get "Access Denied" errors:

1. **Run as Administrator:**
   - Right-click Command Prompt → "Run as administrator"
   - Navigate to plugin folder and copy manually

2. **Change Permissions:**
   - Right-click VST3 folder → Properties → Security
   - Add your user account with Full Control

### DAW-Specific Issues

**Reaper:**
- Go to: Options → Preferences → Plug-ins → VST
- Make sure "Enable VST3 support" is checked
- Add VST3 path manually if needed

**Cubase/Nuendo:**
- Plugin must be in official VST3 folder
- Use VST Plug-in Manager to refresh
- Check Plugin Information window for errors

**FL Studio:**
- Options → Manage Plugins → Plugin Database
- Right-click → "Find plugins" → Start scan
- Check "VST3" checkbox in plugin database

**Ableton Live:**
- Must be Live 10.1 or later for VST3 support
- Preferences → Plug-ins → VST3 folder path
- Click "Rescan" button

### Performance Issues

**High CPU Usage:**
1. Increase audio buffer size in DAW (512-1024 samples)
2. Reduce plugin's Filter Quality in Expert Mode
3. Close unnecessary applications

**Audio Dropouts:**
1. Check Windows audio driver settings
2. Use ASIO drivers if available
3. Increase DAW's audio buffer size

**Plugin Loading Slowly:**
- First load calculates FIR filters (normal behavior)
- Subsequent loads should be faster
- Consider lower Filter Quality for real-time use

### Compatibility Notes

**Windows Audio Drivers:**
- **ASIO** drivers recommended for professional use
- **DirectSound** works but may have higher latency
- **WASAPI** good for Windows 10/11 systems

**Visual C++ Runtime:**
If plugin fails to load, install:
- Microsoft Visual C++ 2015-2022 Redistributable (x64)
- Download from Microsoft official website

## Verification

After successful installation:

1. **Open your DAW**
2. **Create a new project**
3. **Add the plugin** to an audio track:
   - Look for "Hyang" or "LoudnessCompensator" in effects list
   - Category: EQ, Dynamics, or Utility (DAW-dependent)
4. **Test the plugin**:
   - Load "Home Listening" preset
   - Play audio and adjust Loudness knob
   - Verify bypass function works

## Uninstallation

To remove the plugin:

1. **Close all DAWs**
2. **Delete the plugin folder:**
   ```
   C:\Program Files\Common Files\VST3\LoudnessCompensator.vst3
   ```
3. **Clear DAW plugin cache** (optional but recommended)
4. **Restart DAW** and rescan plugins

## Support

**For Windows-specific issues:**
- Check Windows Event Viewer for error details
- Verify VST3 SDK compatibility
- Test with different DAWs to isolate issues

**Get Help:**
- GitHub Issues: https://github.com/grisys83/LoudnessCompensator/issues
- Include: Windows version, DAW name/version, error messages
- Attach: System info, plugin scan logs if available

## Technical Notes

**Plugin Format**: VST3 (cross-platform binary)
**Architecture**: x64 only (32-bit Windows not supported)
**Dependencies**: Microsoft Visual C++ Runtime
**Digital Signature**: Code-signed for Windows security
**Compatibility**: VST3 SDK 3.7+ compatible DAWs

---

**Enjoy precise loudness compensation on Windows! 🎧**