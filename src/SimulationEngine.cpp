// Inclut une dependance necessaire a ce fichier.
#include "SimulationEngine.h"

// Inclut une dependance necessaire a ce fichier.
#include <algorithm>
// Inclut une dependance necessaire a ce fichier.
#include <random>

// Ouvre un espace de noms pour organiser le code.
namespace {

// Declare une constante ou une valeur partagee par le systeme.
constexpr int kFrameIntervalMs = 33;

// Ferme le bloc d instructions courant.
}  // namespace

// Definit le comportement d une methode du simulateur.
SimulationEngine::SimulationEngine(QObject* parent)
    // Execute cette instruction dans le flux normal du programme.
    : QObject(parent)
    // Poursuit la liste d initialisation ou d arguments en cours.
    , m_rng(std::random_device{}())
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    m_snapshot.movements.fill(0);
    // Declare ou utilise une fonction necessaire au programme.
    connect(&m_frameTimer, &QTimer::timeout, this, &SimulationEngine::onFrameTick);
    // Execute cette instruction dans le flux normal du programme.
    m_frameTimer.setInterval(kFrameIntervalMs);
    // Declare ou utilise une fonction necessaire au programme.
    reset();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
const SimulationSnapshot& SimulationEngine::snapshot() const
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return m_snapshot;
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
bool SimulationEngine::isRunning() const
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return m_snapshot.running;
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::start()
// Ouvre un nouveau bloc d instructions.
{
    // Verifie une condition avant d executer ce bloc.
    if (m_snapshot.running) {
        // Retourne la valeur calculee par la fonction.
        return;
    // Ferme le bloc d instructions courant.
    }

    // Initialise ou met a jour une valeur du programme.
    m_snapshot.running = true;
    // Execute cette instruction dans le flux normal du programme.
    m_frameTimer.start();
    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT runningChanged(true);
    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT stateChanged();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::pause()
// Ouvre un nouveau bloc d instructions.
{
    // Verifie une condition avant d executer ce bloc.
    if (!m_snapshot.running) {
        // Retourne la valeur calculee par la fonction.
        return;
    // Ferme le bloc d instructions courant.
    }

    // Initialise ou met a jour une valeur du programme.
    m_snapshot.running = false;
    // Execute cette instruction dans le flux normal du programme.
    m_frameTimer.stop();
    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT runningChanged(false);
    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT stateChanged();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::toggleRunning()
// Ouvre un nouveau bloc d instructions.
{
    // Verifie une condition avant d executer ce bloc.
    if (m_snapshot.running) {
        // Declare ou utilise une fonction necessaire au programme.
        pause();
    // Execute cette instruction dans le flux normal du programme.
    } else {
        // Declare ou utilise une fonction necessaire au programme.
        start();
    // Ferme le bloc d instructions courant.
    }
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::reset()
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const bool wasRunning = m_snapshot.running;

    // Initialise ou met a jour une valeur du programme.
    m_logicAccumulatorMs = 0;
    // Initialise ou met a jour une valeur du programme.
    m_lastGreenPhase = ngaba::Phase::NsGreen;
    // Initialise ou met a jour une valeur du programme.
    m_snapshot.phase = ngaba::Phase::NsGreen;
    // Definit le comportement d une methode du simulateur.
    m_snapshot.traffic = ngaba::initialTraffic();
    // Initialise ou met a jour une valeur du programme.
    m_snapshot.elapsedSeconds = 0;
    // Initialise ou met a jour une valeur du programme.
    m_snapshot.cycleIndex = 1;
    // Initialise ou met a jour une valeur du programme.
    m_snapshot.running = wasRunning;
    // Definit le comportement d une methode du simulateur.
    m_snapshot.timeRemaining = ngaba::adaptiveDuration(m_snapshot.phase, m_snapshot.traffic);
    // Declare ou utilise une fonction necessaire au programme.
    prepareSecond();
    // Initialise ou met a jour une valeur du programme.
    m_snapshot.animationProgress = 0.0;

    // Verifie une condition avant d executer ce bloc.
    if (wasRunning) {
        // Execute cette instruction dans le flux normal du programme.
        m_frameTimer.start();
    // Execute cette instruction dans le flux normal du programme.
    } else {
        // Execute cette instruction dans le flux normal du programme.
        m_frameTimer.stop();
    // Ferme le bloc d instructions courant.
    }

    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT stateChanged();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::prepareSecond()
// Ouvre un nouveau bloc d instructions.
{
    // Definit le comportement d une methode du simulateur.
    m_snapshot.loads = ngaba::axisLoads(m_snapshot.traffic);
    // Definit le comportement d une methode du simulateur.
    m_snapshot.movements = ngaba::allowedMovements(m_snapshot.traffic, m_snapshot.phase);
    // Definit le comportement d une methode du simulateur.
    m_snapshot.adaptation = ngaba::adaptationHint(m_snapshot.phase, m_snapshot.traffic);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::advanceOneSecond()
// Ouvre un nouveau bloc d instructions.
{
    // Definit le comportement d une methode du simulateur.
    m_snapshot.traffic = ngaba::updateTraffic(m_snapshot.traffic, m_snapshot.movements, m_rng);
    // Execute cette instruction dans le flux normal du programme.
    ++m_snapshot.elapsedSeconds;
    // Execute cette instruction dans le flux normal du programme.
    --m_snapshot.timeRemaining;

    // Verifie une condition avant d executer ce bloc.
    if (m_snapshot.timeRemaining <= 0) {
        // Declare une constante ou une valeur partagee par le systeme.
        const ngaba::Phase previousPhase = m_snapshot.phase;
        // Verifie une condition avant d executer ce bloc.
        if (ngaba::isGreenPhase(previousPhase)) {
            // Initialise ou met a jour une valeur du programme.
            m_lastGreenPhase = previousPhase;
        // Ferme le bloc d instructions courant.
        }

        // Definit le comportement d une methode du simulateur.
        m_snapshot.phase = ngaba::nextPhase(previousPhase, m_lastGreenPhase);
        // Verifie une condition avant d executer ce bloc.
        if (previousPhase == ngaba::Phase::Safety && m_snapshot.phase == ngaba::Phase::NsGreen) {
            // Execute cette instruction dans le flux normal du programme.
            ++m_snapshot.cycleIndex;
        // Ferme le bloc d instructions courant.
        }

        // Definit le comportement d une methode du simulateur.
        m_snapshot.timeRemaining = ngaba::adaptiveDuration(m_snapshot.phase, m_snapshot.traffic);
    // Ferme le bloc d instructions courant.
    }

    // Declare ou utilise une fonction necessaire au programme.
    prepareSecond();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void SimulationEngine::onFrameTick()
// Ouvre un nouveau bloc d instructions.
{
    // Verifie une condition avant d executer ce bloc.
    if (!m_snapshot.running) {
        // Retourne la valeur calculee par la fonction.
        return;
    // Ferme le bloc d instructions courant.
    }

    // Initialise ou met a jour une valeur du programme.
    m_logicAccumulatorMs += kFrameIntervalMs;
    // Repete le traitement tant que la condition reste vraie.
    while (m_logicAccumulatorMs >= ngaba::kLogicStepMs) {
        // Initialise ou met a jour une valeur du programme.
        m_logicAccumulatorMs -= ngaba::kLogicStepMs;
        // Declare ou utilise une fonction necessaire au programme.
        advanceOneSecond();
    // Ferme le bloc d instructions courant.
    }

    // Definit le comportement d une methode du simulateur.
    m_snapshot.animationProgress = std::clamp(
        // Execute cette instruction dans le flux normal du programme.
        static_cast<double>(m_logicAccumulatorMs) / static_cast<double>(ngaba::kLogicStepMs),
        // Execute cette instruction dans le flux normal du programme.
        0.0,
        // Execute cette instruction dans le flux normal du programme.
        1.0);
    // Declare ou utilise une fonction necessaire au programme.
    Q_EMIT stateChanged();
// Ferme le bloc d instructions courant.
}
