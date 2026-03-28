# Arrete immediatement le script des qu une erreur survient.
$ErrorActionPreference = 'Stop'

# Recupere le dossier qui contient ce script.
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
# Place le shell a la racine du projet.
Set-Location $root

# Definit le dossier des binaires Qt et MinGW.
$qtBin = 'C:\msys64\mingw64\bin'
# Definit le chemin vers l outil Meta Object Compiler de Qt.
$moc = 'C:\msys64\mingw64\share\qt6\bin\moc.exe'
# Definit le chemin vers pkg-config pour recuperer les drapeaux Qt.
$pkgConfig = Join-Path $qtBin 'pkg-config.exe'
# Definit le chemin vers le compilateur C++.
$compiler = Join-Path $qtBin 'g++.exe'
# Definit le dossier temporaire de construction manuelle.
$buildDir = Join-Path $root 'build-manual'
# Definit le chemin final de l executable compile.
$outputExe = Join-Path $buildDir 'gestion_trafic_rond_point_ngaba.exe'

# Verifie que le compilateur MinGW est disponible.
if (-not (Test-Path $compiler)) {
    # Signale clairement l absence du compilateur attendu.
    throw 'g++.exe introuvable dans C:\msys64\mingw64\bin'
}
# Verifie que l outil moc de Qt est disponible.
if (-not (Test-Path $moc)) {
    # Signale clairement l absence de l outil moc attendu.
    throw 'moc.exe introuvable dans C:\msys64\mingw64\share\qt6\bin'
}

# Ajoute les binaires Qt au PATH de la session courante.
$env:PATH = "$qtBin;$env:PATH"

# Supprime l ancien dossier de build s il existe deja.
if (Test-Path $buildDir) {
    # Nettoie completement le dossier de build precedent.
    Remove-Item -LiteralPath $buildDir -Recurse -Force
}
# Recree un dossier de build vide.
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Genere le fichier moc pour le moteur de simulation.
& $moc 'src/SimulationEngine.h' -o (Join-Path $buildDir 'moc_SimulationEngine.cpp')
# Genere le fichier moc pour le widget de l intersection.
& $moc 'src/IntersectionWidget.h' -o (Join-Path $buildDir 'moc_IntersectionWidget.cpp')
# Genere le fichier moc pour la fenetre principale.
& $moc 'src/MainWindow.h' -o (Join-Path $buildDir 'moc_MainWindow.cpp')

# Recupere les drapeaux de compilation Qt depuis pkg-config.
$qtFlags = ((& $pkgConfig --cflags Qt6Widgets).Trim() -split ' ')
# Recupere les bibliotheques Qt a l edition de liens.
$qtLibs = ((& $pkgConfig --libs Qt6Widgets).Trim() -split ' ')

# Prepare les options du compilateur C++.
$compilerOptions = @(
    # Demande le standard C++17.
    '-std=c++17'
    # Active une optimisation raisonnable pour l executable.
    '-O2'
    # Desactive les mots cles Qt redefinis pour rester explicite.
    '-DQT_NO_KEYWORDS'
)

# Prepare la liste des sources a compiler.
$sources = @(
    # Ajoute le point d entree principal du programme.
    'src/main.cpp'
    # Ajoute la definition de la fenetre principale.
    'src/MainWindow.cpp'
    # Ajoute la logique du moteur de simulation.
    'src/SimulationEngine.cpp'
    # Ajoute le rendu graphique de l intersection.
    'src/IntersectionWidget.cpp'
    # Ajoute le modele de trafic et les regles metier.
    'src/TrafficModel.cpp'
    # Ajoute le fichier moc genere pour la fenetre principale.
    (Join-Path $buildDir 'moc_MainWindow.cpp')
    # Ajoute le fichier moc genere pour le moteur de simulation.
    (Join-Path $buildDir 'moc_SimulationEngine.cpp')
    # Ajoute le fichier moc genere pour le widget d intersection.
    (Join-Path $buildDir 'moc_IntersectionWidget.cpp')
)

# Lance la compilation complete du projet Qt en mode manuel.
& $compiler @compilerOptions @qtFlags @sources '-o' $outputExe @qtLibs

# Verifie le code retour du compilateur.
if ($LASTEXITCODE -ne 0) {
    # Interrompt le script si la compilation a echoue.
    throw "Echec de compilation, code $LASTEXITCODE"
}

# Affiche le chemin de l executable genere a la fin du build.
Write-Host 'Build termine : build-manual\gestion_trafic_rond_point_ngaba.exe'
