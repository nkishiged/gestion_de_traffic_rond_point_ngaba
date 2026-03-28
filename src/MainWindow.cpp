// Inclut une dependance necessaire a ce fichier.
#include "MainWindow.h"

// Inclut une dependance necessaire a ce fichier.
#include "IntersectionWidget.h"
// Inclut une dependance necessaire a ce fichier.
#include "SimulationEngine.h"

// Inclut une dependance necessaire a ce fichier.
#include <QShortcut>

// Definit le comportement d une methode du simulateur.
MainWindow::MainWindow(QWidget* parent)
    // Execute cette instruction dans le flux normal du programme.
    : QMainWindow(parent)
    // Poursuit la liste d initialisation ou d arguments en cours.
    , m_engine(new SimulationEngine(this))
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    setWindowTitle("Simulation du rond-point Ngaba - Qt");
    // Declare ou utilise une fonction necessaire au programme.
    resize(1380, 860);
    // Declare ou utilise une fonction necessaire au programme.
    setMinimumSize(1000, 680);

    // Initialise ou met a jour une valeur du programme.
    m_intersectionWidget = new IntersectionWidget(m_engine, this);
    // Declare ou utilise une fonction necessaire au programme.
    setCentralWidget(m_intersectionWidget);

    // Execute cette instruction dans le flux normal du programme.
    setStyleSheet("QMainWindow { background: white; }");

    // Initialise ou met a jour une valeur du programme.
    auto* toggleShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    // Initialise ou met a jour une valeur du programme.
    auto* resetShortcut = new QShortcut(QKeySequence(Qt::Key_R), this);
    // Declare ou utilise une fonction necessaire au programme.
    connect(toggleShortcut, &QShortcut::activated, m_engine, &SimulationEngine::toggleRunning);
    // Declare ou utilise une fonction necessaire au programme.
    connect(resetShortcut, &QShortcut::activated, m_engine, &SimulationEngine::reset);

    // Execute cette instruction dans le flux normal du programme.
    m_engine->start();
// Ferme le bloc d instructions courant.
}
