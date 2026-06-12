Add-Type -AssemblyName System.Drawing
Get-ChildItem "assets/*.png" | ForEach-Object {
    $img = [System.Drawing.Image]::FromFile($_.FullName)
    Write-Host ("{0,-35} {1}x{2}" -f $_.Name, $img.Width, $img.Height)
    $img.Dispose()
}
