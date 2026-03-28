# Arrete immediatement le script si une erreur survient.
$ErrorActionPreference = 'Stop'

# Recupere le dossier qui contient ce script.
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
# Place le shell a la racine du projet.
Set-Location $root

# Definit le dossier des binaires Qt et MinGW.
$qtBin = 'C:\msys64\mingw64\bin'
# Definit le chemin vers l executable a lancer.
$exe = Join-Path $root 'build-manual\gestion_trafic_rond_point_ngaba.exe'

# Ajoute les binaires Qt au PATH de la session courante.
$env:PATH = "$qtBin;$env:PATH"

# Verifie si l executable existe deja.
if (-not (Test-Path $exe)) {
    # Lance d abord la compilation si l executable est absent.
    & (Join-Path $root 'build.ps1')
}

# Demarre l application compilee.
& $exe
