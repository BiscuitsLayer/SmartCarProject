# Auxiliary function to stop on failure
function Invoke-Call {
    param (
        [scriptblock]$ScriptBlock,
        [string]$ErrorAction = $ErrorActionPreference
    )
    & @ScriptBlock
    if (($lastexitcode -ne 0) -and $ErrorAction -eq "Stop") {
        Pop-Location
        exit $lastexitcode
    }
}

# Pull changes from remote repository
Invoke-Call -ScriptBlock { git pull } -ErrorAction Stop

# Save initial location
Push-Location

# Build OOGL submodule .a (library) file
Set-Location include/OOGL -ErrorAction Stop
Invoke-Call -ScriptBlock { make } -ErrorAction Stop
Set-Location ../.. -ErrorAction Stop

# Build smartcar
New-Item ./build -ItemType Directory -ErrorAction Ignore
Set-Location build -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake .. -G "MinGW Makefiles" } -ErrorAction Stop
Invoke-Call -ScriptBlock { make -j8 } -ErrorAction Stop

# Run smartcar
Invoke-Call -ScriptBlock { ./SmartCarMain.exe } -ErrorAction Stop
Pop-Location -ErrorAction Stop