# Definit la version minimale de CMake necessaire au projet.
cmake_minimum_required(VERSION 3.21)

# Declare le projet, sa version et le langage utilise.
project(gestion_trafic_rond_point_ngaba VERSION 1.0 LANGUAGES CXX)

# Fixe le standard C++ a utiliser pour toute la compilation.
set(CMAKE_CXX_STANDARD 17)
# Rend l usage du standard C++17 obligatoire.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Interdit les extensions specifiques au compilateur.
set(CMAKE_CXX_EXTENSIONS OFF)

# Active la generation automatique des fichiers moc de Qt.
set(CMAKE_AUTOMOC ON)
# Active la prise en charge automatique des ressources Qt.
set(CMAKE_AUTORCC ON)
# Active la prise en charge automatique des formulaires Qt.
set(CMAKE_AUTOUIC ON)

# Recherche une installation Qt compatible contenant Widgets.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# Charge le paquet Qt correspondant a la version detectee.
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Ouvre la liste des sources du projet.
set(PROJECT_SOURCES
    # Ajoute le point d entree principal.
    src/main.cpp
    # Ajoute l implementation de la fenetre principale.
    src/MainWindow.cpp
    # Ajoute la declaration de la fenetre principale.
    src/MainWindow.h
    # Ajoute l implementation du moteur de simulation.
    src/SimulationEngine.cpp
    # Ajoute la declaration du moteur de simulation.
    src/SimulationEngine.h
    # Ajoute l implementation du widget de rendu.
    src/IntersectionWidget.cpp
    # Ajoute la declaration du widget de rendu.
    src/IntersectionWidget.h
    # Ajoute l implementation du modele de trafic.
    src/TrafficModel.cpp
    # Ajoute la declaration du modele de trafic.
    src/TrafficModel.h
)

# Verifie si la version de Qt est au moins egale a 6.
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    # Cree l executable avec l API Qt 6 moderne.
    qt_add_executable(gestion_trafic_rond_point_ngaba
        # Demande une finalisation manuelle de l executable.
        MANUAL_FINALIZATION
        # Injecte toute la liste des sources du projet.
        ${PROJECT_SOURCES}
    )
# Gere le cas des versions plus anciennes de Qt.
else()
    # Cree l executable avec la commande CMake standard.
    add_executable(gestion_trafic_rond_point_ngaba
        # Injecte toute la liste des sources du projet.
        ${PROJECT_SOURCES}
    )
# Ferme le test sur la version de Qt.
endif()

# Lie l executable final a la bibliotheque Widgets de Qt.
target_link_libraries(gestion_trafic_rond_point_ngaba PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Ajoute une definition de compilation partagee par le projet.
target_compile_definitions(gestion_trafic_rond_point_ngaba PRIVATE QT_NO_KEYWORDS)

# Charge les chemins d installation standards GNU.
include(GNUInstallDirs)
# Ouvre la regle d installation de l executable.
install(TARGETS gestion_trafic_rond_point_ngaba
    # Place le binaire dans le dossier d installation des executables.
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Verifie a nouveau si la version de Qt est au moins egale a 6.
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    # Finalise l executable avec les traitements Qt 6 necessaires.
    qt_finalize_executable(gestion_trafic_rond_point_ngaba)
# Ferme le test final sur la version de Qt.
endif()
