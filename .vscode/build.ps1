
# This script handles killing the emulator, building the ROM, and running it.

# --- Configuration ---
$emulatorProcessName = "mGBA"
# $emulatorPath = "c:\Users\jgens\Downloads\mGBA-0.10.5-win64\mGBA-0.10.5-win64\mGBA.exe"
$emulatorPath = "C:\Program Files\mGBA\mGBA.exe"
$makeTarget = "train-game.gb"
# ---------------------

Write-Host "Attempting to stop any running '$emulatorProcessName' processes..."
# Get all processes by name and stop them forcefully to avoid any prompts.
# -ErrorAction SilentlyContinue prevents errors if the process isn't running.
Get-Process -Name $emulatorProcessName -ErrorAction SilentlyContinue | Stop-Process -Force

Write-Host "--- Starting Build ---"
# Run make to build the ROM and get usage stats.
make $makeTarget usage

# Check the exit code of the last command (`make`). $? is true if the exit code was 0.
if ($?) {
    Write-Host "--- Build Successful. Launching Emulator ---"
    # Instead of 'make run', we launch the emulator directly as a background process.
    # This allows the script and the VS Code task to terminate immediately.
    Start-Process -FilePath $emulatorPath -ArgumentList $makeTarget
} else {
    Write-Host "--- Build Failed. Not running emulator. ---"
    # Exit with a non-zero status code to signal failure to VS Code.
    exit 1
}
