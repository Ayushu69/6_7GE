Add-Type -AssemblyName System.Drawing
$img = [System.Drawing.Image]::FromFile("c:\six_sevenGE\assets\Assets.png")
Write-Host "$($img.Width)x$($img.Height)"
$img.Dispose()
