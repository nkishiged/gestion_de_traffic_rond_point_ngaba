// Evite les inclusions multiples de ce fichier.
#pragma once

// Inclut une dependance necessaire a ce fichier.
#include <QPointF>
// Inclut une dependance necessaire a ce fichier.
#include <QString>
// Inclut une dependance necessaire a ce fichier.
#include <QVector>

// Inclut une dependance necessaire a ce fichier.
#include <array>
// Inclut une dependance necessaire a ce fichier.
#include <random>

// Ouvre un espace de noms pour organiser le code.
namespace ngaba {

// Definit une enumeration forte pour representer des etats.
enum class Phase {
    // Initialise ou met a jour une valeur du programme.
    NsGreen = 0,
    // Execute cette instruction dans le flux normal du programme.
    NsOrange,
    // Execute cette instruction dans le flux normal du programme.
    Safety,
    // Execute cette instruction dans le flux normal du programme.
    EwGreen,
    // Execute cette instruction dans le flux normal du programme.
    EwOrange,
// Execute cette instruction dans le flux normal du programme.
};

// Definit une enumeration forte pour representer des etats.
enum class Branch {
    // Initialise ou met a jour une valeur du programme.
    North = 0,
    // Execute cette instruction dans le flux normal du programme.
    South,
    // Execute cette instruction dans le flux normal du programme.
    West,
    // Execute cette instruction dans le flux normal du programme.
    East,
// Execute cette instruction dans le flux normal du programme.
};

// Definit une enumeration forte pour representer des etats.
enum class LightColor {
    // Initialise ou met a jour une valeur du programme.
    Red = 0,
    // Execute cette instruction dans le flux normal du programme.
    Orange,
    // Execute cette instruction dans le flux normal du programme.
    Green,
// Execute cette instruction dans le flux normal du programme.
};

// Definit une enumeration forte pour representer des etats.
enum class Movement {
    // Initialise ou met a jour une valeur du programme.
    N_S = 0,
    // Execute cette instruction dans le flux normal du programme.
    N_E,
    // Execute cette instruction dans le flux normal du programme.
    N_W,
    // Execute cette instruction dans le flux normal du programme.
    S_N,
    // Execute cette instruction dans le flux normal du programme.
    S_E,
    // Execute cette instruction dans le flux normal du programme.
    S_W,
    // Execute cette instruction dans le flux normal du programme.
    W_E,
    // Execute cette instruction dans le flux normal du programme.
    W_N,
    // Execute cette instruction dans le flux normal du programme.
    W_S,
    // Execute cette instruction dans le flux normal du programme.
    E_W,
    // Execute cette instruction dans le flux normal du programme.
    E_N,
    // Execute cette instruction dans le flux normal du programme.
    E_S,
// Execute cette instruction dans le flux normal du programme.
};

// Declare une constante ou une valeur partagee par le systeme.
constexpr int kMovementCount = 12;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kBranchCount = 4;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kVisibleQueueMax = 6;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kLogicStepMs = 700;

// Initialise ou met a jour une valeur du programme.
using TrafficArray = std::array<int, kMovementCount>;
// Initialise ou met a jour une valeur du programme.
using MovementArray = std::array<int, kMovementCount>;
// Initialise ou met a jour une valeur du programme.
using BranchArray = std::array<int, kBranchCount>;

// Declare une structure de donnees partagee par le simulateur.
struct AxisLoads {
    // Initialise ou met a jour une valeur du programme.
    int northSouth = 0;
    // Initialise ou met a jour une valeur du programme.
    int eastWest = 0;
    // Initialise ou met a jour une valeur du programme.
    int universityToUniversity = 0;
    // Initialise ou met a jour une valeur du programme.
    int universityToBypass = 0;
    // Initialise ou met a jour une valeur du programme.
    int bypassToUniversity = 0;
    // Initialise ou met a jour une valeur du programme.
    int bypassToBypass = 0;
// Execute cette instruction dans le flux normal du programme.
};

// Declare une structure de donnees partagee par le simulateur.
struct LightPair {
    // Initialise ou met a jour une valeur du programme.
    LightColor forward = LightColor::Red;
    // Initialise ou met a jour une valeur du programme.
    LightColor returnFlow = LightColor::Green;
// Execute cette instruction dans le flux normal du programme.
};

// Declare une structure de donnees partagee par le simulateur.
struct QueueVisualConfig {
    // Execute cette instruction dans le flux normal du programme.
    QPointF start;
    // Execute cette instruction dans le flux normal du programme.
    QPointF step;
    // Initialise ou met a jour une valeur du programme.
    double angleDeg = 0.0;
    // Execute cette instruction dans le flux normal du programme.
    QPointF badgeOffset;
// Execute cette instruction dans le flux normal du programme.
};

// Declare ou utilise une fonction necessaire au programme.
int movementIndex(Movement movement);
// Declare ou utilise une fonction necessaire au programme.
int branchIndex(Branch branch);

// Definit le comportement d une methode du simulateur.
const std::array<Movement, kMovementCount>& allMovements();
// Definit le comportement d une methode du simulateur.
const std::array<Branch, kBranchCount>& allBranches();

// Declare ou utilise une fonction necessaire au programme.
QString phaseLabel(Phase phase);
// Declare ou utilise une fonction necessaire au programme.
QString movementName(Movement movement);
// Declare ou utilise une fonction necessaire au programme.
QString movementLabel(Movement movement);

// Declare ou utilise une fonction necessaire au programme.
Branch originBranch(Movement movement);
// Declare ou utilise une fonction necessaire au programme.
Branch destinationBranch(Movement movement);
// Declare ou utilise une fonction necessaire au programme.
int movementPriority(Movement movement);

// Declare ou utilise une fonction necessaire au programme.
bool isNorthSouthPhase(Phase phase);
// Declare ou utilise une fonction necessaire au programme.
bool isEastWestPhase(Phase phase);
// Declare ou utilise une fonction necessaire au programme.
bool isGreenPhase(Phase phase);

// Declare ou utilise une fonction necessaire au programme.
TrafficArray initialTraffic();
// Declare ou utilise une fonction necessaire au programme.
BranchArray branchQueues(const TrafficArray& traffic);
// Declare ou utilise une fonction necessaire au programme.
AxisLoads axisLoads(const TrafficArray& traffic);

// Declare ou utilise une fonction necessaire au programme.
Phase nextPhase(Phase current, Phase lastGreenPhase);
// Declare ou utilise une fonction necessaire au programme.
MovementArray allowedMovements(const TrafficArray& traffic, Phase phase);
// Declare ou utilise une fonction necessaire au programme.
TrafficArray updateTraffic(const TrafficArray& traffic, const MovementArray& movements, std::mt19937& rng);
// Declare ou utilise une fonction necessaire au programme.
int adaptiveDuration(Phase phase, const TrafficArray& traffic);
// Declare ou utilise une fonction necessaire au programme.
QString adaptationHint(Phase phase, const TrafficArray& traffic);

// Declare ou utilise une fonction necessaire au programme.
const QVector<QPointF>& routePoints(Movement movement);
// Declare ou utilise une fonction necessaire au programme.
QueueVisualConfig queueConfig(Branch branch);
// Definit le comportement d une methode du simulateur.
std::array<LightPair, kBranchCount> lightState(Phase phase);

// Ferme le bloc d instructions courant.
}  // namespace ngaba
