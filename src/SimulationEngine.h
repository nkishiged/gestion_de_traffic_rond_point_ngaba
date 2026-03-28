// Evite les inclusions multiples de ce fichier.
#pragma once

// Inclut une dependance necessaire a ce fichier.
#include "TrafficModel.h"

// Inclut une dependance necessaire a ce fichier.
#include <QObject>
// Inclut une dependance necessaire a ce fichier.
#include <QTimer>

// Inclut une dependance necessaire a ce fichier.
#include <random>

// Declare une structure de donnees partagee par le simulateur.
struct SimulationSnapshot {
    // Initialise ou met a jour une valeur du programme.
    ngaba::Phase phase = ngaba::Phase::NsGreen;
    // Definit le comportement d une methode du simulateur.
    ngaba::TrafficArray traffic = ngaba::initialTraffic();
    // Execute cette instruction dans le flux normal du programme.
    ngaba::MovementArray movements{};
    // Execute cette instruction dans le flux normal du programme.
    ngaba::AxisLoads loads{};
    // Initialise ou met a jour une valeur du programme.
    int timeRemaining = 0;
    // Initialise ou met a jour une valeur du programme.
    int elapsedSeconds = 0;
    // Initialise ou met a jour une valeur du programme.
    int cycleIndex = 1;
    // Initialise ou met a jour une valeur du programme.
    double animationProgress = 0.0;
    // Initialise ou met a jour une valeur du programme.
    QString adaptation = "Flux nominal";
    // Initialise ou met a jour une valeur du programme.
    bool running = false;
// Execute cette instruction dans le flux normal du programme.
};

// Declare une classe utilisee par l application.
class SimulationEngine : public QObject
// Ouvre un nouveau bloc d instructions.
{
    // Active les mecanismes meta objet de Qt pour cette classe.
    Q_OBJECT

// Definit le niveau d acces des membres qui suivent.
public:
    // Declare un constructeur explicite pour eviter les conversions implicites.
    explicit SimulationEngine(QObject* parent = nullptr);

    // Declare ou utilise une fonction necessaire au programme.
    const SimulationSnapshot& snapshot() const;
    // Declare ou utilise une fonction necessaire au programme.
    bool isRunning() const;

// Declare les slots publics relies aux actions de l application.
public Q_SLOTS:
    // Declare ou utilise une fonction necessaire au programme.
    void start();
    // Declare ou utilise une fonction necessaire au programme.
    void pause();
    // Declare ou utilise une fonction necessaire au programme.
    void toggleRunning();
    // Declare ou utilise une fonction necessaire au programme.
    void reset();

// Declare les signaux emis par cet objet Qt.
Q_SIGNALS:
    // Declare ou utilise une fonction necessaire au programme.
    void stateChanged();
    // Declare ou utilise une fonction necessaire au programme.
    void runningChanged(bool running);

// Declare les slots internes relies a la logique de simulation.
private Q_SLOTS:
    // Declare ou utilise une fonction necessaire au programme.
    void onFrameTick();

// Definit le niveau d acces des membres qui suivent.
private:
    // Declare ou utilise une fonction necessaire au programme.
    void prepareSecond();
    // Declare ou utilise une fonction necessaire au programme.
    void advanceOneSecond();

    // Execute cette instruction dans le flux normal du programme.
    QTimer m_frameTimer;
    // Execute cette instruction dans le flux normal du programme.
    std::mt19937 m_rng;
    // Initialise ou met a jour une valeur du programme.
    int m_logicAccumulatorMs = 0;
    // Initialise ou met a jour une valeur du programme.
    ngaba::Phase m_lastGreenPhase = ngaba::Phase::NsGreen;
    // Execute cette instruction dans le flux normal du programme.
    SimulationSnapshot m_snapshot;
// Execute cette instruction dans le flux normal du programme.
};
