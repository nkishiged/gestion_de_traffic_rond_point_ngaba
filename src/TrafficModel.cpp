// Inclut une dependance necessaire a ce fichier.
#include "TrafficModel.h"

// Inclut une dependance necessaire a ce fichier.
#include <algorithm>
// Inclut une dependance necessaire a ce fichier.
#include <array>
// Inclut une dependance necessaire a ce fichier.
#include <cmath>
// Inclut une dependance necessaire a ce fichier.
#include <utility>

// Ouvre un espace de noms pour organiser le code.
namespace ngaba {

// Ouvre un espace de noms pour organiser le code.
namespace {

// Declare une structure de donnees partagee par le simulateur.
struct MovementMeta {
    // Execute cette instruction dans le flux normal du programme.
    Movement movement;
    // Execute cette instruction dans le flux normal du programme.
    Branch origin;
    // Execute cette instruction dans le flux normal du programme.
    Branch destination;
    // Execute cette instruction dans le flux normal du programme.
    int priority;
    // Declare une constante ou une valeur partagee par le systeme.
    const char* name;
    // Declare une constante ou une valeur partagee par le systeme.
    const char* label;
// Execute cette instruction dans le flux normal du programme.
};

// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<MovementMeta, kMovementCount> kMovementMeta{{
    // Execute cette instruction dans le flux normal du programme.
    {Movement::N_S, Branch::North, Branch::South, 4, "N_S", "Universite N -> S"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::N_E, Branch::North, Branch::East, 3, "N_E", "Universite N -> By-Pass E"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::N_W, Branch::North, Branch::West, 2, "N_W", "Universite N -> By-Pass O"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::S_N, Branch::South, Branch::North, 4, "S_N", "Universite S -> N"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::S_E, Branch::South, Branch::East, 2, "S_E", "Universite S -> By-Pass E"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::S_W, Branch::South, Branch::West, 3, "S_W", "Universite S -> By-Pass O"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::W_E, Branch::West, Branch::East, 4, "W_E", "By-Pass O -> E"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::W_N, Branch::West, Branch::North, 3, "W_N", "By-Pass O -> Universite N"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::W_S, Branch::West, Branch::South, 2, "W_S", "By-Pass O -> Universite S"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::E_W, Branch::East, Branch::West, 4, "E_W", "By-Pass E -> O"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::E_N, Branch::East, Branch::North, 2, "E_N", "By-Pass E -> Universite N"},
    // Execute cette instruction dans le flux normal du programme.
    {Movement::E_S, Branch::East, Branch::South, 3, "E_S", "By-Pass E -> Universite S"},
// Execute cette instruction dans le flux normal du programme.
}};

// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Movement, 3> kNorthMovements{{Movement::N_S, Movement::N_E, Movement::N_W}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Movement, 3> kSouthMovements{{Movement::S_N, Movement::S_E, Movement::S_W}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Movement, 3> kWestMovements{{Movement::W_E, Movement::W_N, Movement::W_S}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Movement, 3> kEastMovements{{Movement::E_W, Movement::E_N, Movement::E_S}};

// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<int, kBranchCount> kGreenCapacity{{6, 6, 7, 7}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kOrangeCapacity = 2;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kCongestionThreshold = 16;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kOverloadThreshold = 24;
// Declare une constante ou une valeur partagee par le systeme.
constexpr int kGreenExtension = 6;
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<int, 5> kPhaseBaseDuration{{14, 3, 1, 16, 3}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<int, 5> kGreenMinDuration{{8, 0, 0, 10, 0}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<int, 5> kGreenMaxDuration{{24, 3, 1, 24, 3}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<int, 5> kUnderloadThreshold{{6, 0, 0, 8, 0}};

// Declare une constante ou une valeur partagee par le systeme.
const std::array<QueueVisualConfig, kBranchCount> kQueueConfigs{{
    // Execute cette instruction dans le flux normal du programme.
    {QPointF(53.2, 76.0), QPointF(0.0, 5.4), -90.0, QPointF(0.0, -3.0)},
    // Execute cette instruction dans le flux normal du programme.
    {QPointF(56.0, 24.0), QPointF(0.0, -5.4), 90.0, QPointF(0.0, 3.0)},
    // Execute cette instruction dans le flux normal du programme.
    {QPointF(24.0, 48.2), QPointF(-5.4, 0.0), 0.0, QPointF(3.0, 0.0)},
    // Execute cette instruction dans le flux normal du programme.
    {QPointF(78.0, 52.0), QPointF(5.4, 0.0), 180.0, QPointF(-3.0, 0.0)},
// Execute cette instruction dans le flux normal du programme.
}};

// Declare une constante ou une valeur partagee par le systeme.
const std::array<QVector<QPointF>, kMovementCount> kRoutes{{
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{53.2, 100.0}, {53.2, 82.0}, {53.2, 66.0}, {52.0, 59.0}, {47.0, 55.0}, {36.0, 52.6}, {20.0, 52.6}, {0.0, 52.6}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{53.2, 100.0}, {53.2, 82.0}, {53.2, 66.0}, {53.8, 59.0}, {55.0, 53.0}, {54.2, 47.0}, {52.0, 32.0}, {52.0, 0.0}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{53.2, 100.0}, {53.2, 82.0}, {53.5, 66.0}, {56.5, 59.0}, {62.0, 57.0}, {71.0, 53.0}, {84.0, 47.8}, {100.0, 47.8}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{56.8, 0.0}, {56.8, 22.0}, {56.8, 40.0}, {56.8, 54.0}, {56.4, 61.0}, {56.0, 76.0}, {56.0, 100.0}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{56.0, 0.0}, {56.0, 20.0}, {56.0, 37.0}, {58.0, 44.0}, {63.0, 47.0}, {74.0, 47.8}, {100.0, 47.8}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{56.0, 0.0}, {56.0, 22.0}, {56.0, 37.0}, {54.0, 45.0}, {49.0, 49.0}, {35.0, 52.6}, {0.0, 52.6}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{0.0, 48.2}, {20.0, 48.2}, {38.0, 48.2}, {50.0, 48.0}, {70.0, 47.8}, {100.0, 47.8}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{0.0, 48.2}, {20.0, 48.2}, {38.0, 48.2}, {49.0, 53.0}, {54.0, 59.0}, {56.8, 72.0}, {56.8, 100.0}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{0.0, 48.2}, {20.0, 48.2}, {38.0, 48.2}, {47.0, 48.0}, {51.0, 45.0}, {52.0, 36.0}, {52.0, 0.0}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{100.0, 52.0}, {82.0, 52.0}, {68.0, 52.0}, {59.0, 52.6}, {42.0, 52.6}, {0.0, 52.6}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{100.0, 52.0}, {82.0, 52.0}, {68.0, 52.0}, {61.0, 55.0}, {57.5, 61.0}, {56.8, 74.0}, {56.8, 100.0}},
    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF>{{100.0, 52.0}, {82.0, 52.0}, {68.0, 52.0}, {60.0, 48.0}, {54.0, 44.0}, {52.0, 32.0}, {52.0, 0.0}},
// Execute cette instruction dans le flux normal du programme.
}};

// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Branch, kBranchCount> kBranches{{Branch::North, Branch::South, Branch::West, Branch::East}};
// Declare une constante ou une valeur partagee par le systeme.
constexpr std::array<Movement, kMovementCount> kMovements{{
    // Execute cette instruction dans le flux normal du programme.
    Movement::N_S,
    // Execute cette instruction dans le flux normal du programme.
    Movement::N_E,
    // Execute cette instruction dans le flux normal du programme.
    Movement::N_W,
    // Execute cette instruction dans le flux normal du programme.
    Movement::S_N,
    // Execute cette instruction dans le flux normal du programme.
    Movement::S_E,
    // Execute cette instruction dans le flux normal du programme.
    Movement::S_W,
    // Execute cette instruction dans le flux normal du programme.
    Movement::W_E,
    // Execute cette instruction dans le flux normal du programme.
    Movement::W_N,
    // Execute cette instruction dans le flux normal du programme.
    Movement::W_S,
    // Execute cette instruction dans le flux normal du programme.
    Movement::E_W,
    // Execute cette instruction dans le flux normal du programme.
    Movement::E_N,
    // Execute cette instruction dans le flux normal du programme.
    Movement::E_S,
// Execute cette instruction dans le flux normal du programme.
}};

// Definit le comportement d une methode du simulateur.
const std::array<Movement, 3>& branchMovements(Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (branch) {
    // Traite un cas particulier dans la selection.
    case Branch::North:
        // Retourne la valeur calculee par la fonction.
        return kNorthMovements;
    // Traite un cas particulier dans la selection.
    case Branch::South:
        // Retourne la valeur calculee par la fonction.
        return kSouthMovements;
    // Traite un cas particulier dans la selection.
    case Branch::West:
        // Retourne la valeur calculee par la fonction.
        return kWestMovements;
    // Traite un cas particulier dans la selection.
    case Branch::East:
        // Retourne la valeur calculee par la fonction.
        return kEastMovements;
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return kNorthMovements;
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
std::pair<int, int> branchDemandRange(Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (branch) {
    // Traite un cas particulier dans la selection.
    case Branch::North:
    // Traite un cas particulier dans la selection.
    case Branch::South:
        // Retourne la valeur calculee par la fonction.
        return {0, 4};
    // Traite un cas particulier dans la selection.
    case Branch::West:
    // Traite un cas particulier dans la selection.
    case Branch::East:
        // Retourne la valeur calculee par la fonction.
        return {1, 5};
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return {0, 0};
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
std::array<int, 3> branchArrivalWeights(Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (branch) {
    // Traite un cas particulier dans la selection.
    case Branch::North:
        // Retourne la valeur calculee par la fonction.
        return {{5, 3, 2}};
    // Traite un cas particulier dans la selection.
    case Branch::South:
        // Retourne la valeur calculee par la fonction.
        return {{5, 2, 3}};
    // Traite un cas particulier dans la selection.
    case Branch::West:
        // Retourne la valeur calculee par la fonction.
        return {{6, 3, 2}};
    // Traite un cas particulier dans la selection.
    case Branch::East:
        // Retourne la valeur calculee par la fonction.
        return {{6, 2, 3}};
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return {{1, 1, 1}};
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int branchCapacity(Phase phase, Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::NsOrange && (branch == Branch::North || branch == Branch::South)) {
        // Retourne la valeur calculee par la fonction.
        return kOrangeCapacity;
    // Ferme le bloc d instructions courant.
    }
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::EwOrange && (branch == Branch::West || branch == Branch::East)) {
        // Retourne la valeur calculee par la fonction.
        return kOrangeCapacity;
    // Ferme le bloc d instructions courant.
    }
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::NsGreen && (branch == Branch::North || branch == Branch::South)) {
        // Retourne la valeur calculee par la fonction.
        return kGreenCapacity[branchIndex(branch)];
    // Ferme le bloc d instructions courant.
    }
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::EwGreen && (branch == Branch::West || branch == Branch::East)) {
        // Retourne la valeur calculee par la fonction.
        return kGreenCapacity[branchIndex(branch)];
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return 0;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int remainingDemand(const TrafficArray& traffic, const MovementArray& movements, Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const int index = movementIndex(movement);
    // Retourne la valeur calculee par la fonction.
    return std::max(0, traffic[index] - movements[index]);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int scoreService(const TrafficArray& traffic, const MovementArray& movements, Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return remainingDemand(traffic, movements, movement) * 10 + movementPriority(movement);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int phaseIndex(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return static_cast<int>(phase);
// Ferme le bloc d instructions courant.
}

// Ferme le bloc d instructions courant.
}  // namespace

// Declare ou utilise une fonction necessaire au programme.
int movementIndex(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return static_cast<int>(movement);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int branchIndex(Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return static_cast<int>(branch);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
const std::array<Movement, kMovementCount>& allMovements()
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kMovements;
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
const std::array<Branch, kBranchCount>& allBranches()
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kBranches;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QString phaseLabel(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (phase) {
    // Traite un cas particulier dans la selection.
    case Phase::NsGreen:
        // Retourne la valeur calculee par la fonction.
        return "Axe Universite vert";
    // Traite un cas particulier dans la selection.
    case Phase::NsOrange:
        // Retourne la valeur calculee par la fonction.
        return "Axe Universite orange";
    // Traite un cas particulier dans la selection.
    case Phase::Safety:
        // Retourne la valeur calculee par la fonction.
        return "Phase de securite";
    // Traite un cas particulier dans la selection.
    case Phase::EwGreen:
        // Retourne la valeur calculee par la fonction.
        return "Axe By-Pass vert";
    // Traite un cas particulier dans la selection.
    case Phase::EwOrange:
        // Retourne la valeur calculee par la fonction.
        return "Axe By-Pass orange";
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return "Phase inconnue";
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QString movementName(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return QString::fromLatin1(kMovementMeta[movementIndex(movement)].name);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QString movementLabel(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return QString::fromLatin1(kMovementMeta[movementIndex(movement)].label);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
Branch originBranch(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kMovementMeta[movementIndex(movement)].origin;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
Branch destinationBranch(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kMovementMeta[movementIndex(movement)].destination;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int movementPriority(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kMovementMeta[movementIndex(movement)].priority;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
bool isNorthSouthPhase(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return phase == Phase::NsGreen || phase == Phase::NsOrange;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
bool isEastWestPhase(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return phase == Phase::EwGreen || phase == Phase::EwOrange;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
bool isGreenPhase(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return phase == Phase::NsGreen || phase == Phase::EwGreen;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
TrafficArray initialTraffic()
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return TrafficArray{{3, 2, 1, 2, 1, 2, 5, 2, 2, 4, 1, 2}};
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
BranchArray branchQueues(const TrafficArray& traffic)
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    BranchArray queues{{0, 0, 0, 0}};
    // Parcourt une suite de valeurs ou d elements.
    for (const Branch branch : allBranches()) {
        // Initialise ou met a jour une valeur du programme.
        int total = 0;
        // Parcourt une suite de valeurs ou d elements.
        for (const Movement movement : branchMovements(branch)) {
            // Initialise ou met a jour une valeur du programme.
            total += traffic[movementIndex(movement)];
        // Ferme le bloc d instructions courant.
        }
        // Initialise ou met a jour une valeur du programme.
        queues[branchIndex(branch)] = total;
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return queues;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
AxisLoads axisLoads(const TrafficArray& traffic)
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const BranchArray queues = branchQueues(traffic);

    // Execute cette instruction dans le flux normal du programme.
    AxisLoads loads;
    // Definit le comportement d une methode du simulateur.
    loads.northSouth = queues[branchIndex(Branch::North)] + queues[branchIndex(Branch::South)];
    // Definit le comportement d une methode du simulateur.
    loads.eastWest = queues[branchIndex(Branch::West)] + queues[branchIndex(Branch::East)];
    // Definit le comportement d une methode du simulateur.
    loads.universityToUniversity = traffic[movementIndex(Movement::N_S)] + traffic[movementIndex(Movement::S_N)];
    // Definit le comportement d une methode du simulateur.
    loads.universityToBypass = traffic[movementIndex(Movement::N_E)] + traffic[movementIndex(Movement::N_W)]
        // Definit le comportement d une methode du simulateur.
        + traffic[movementIndex(Movement::S_E)] + traffic[movementIndex(Movement::S_W)];
    // Definit le comportement d une methode du simulateur.
    loads.bypassToUniversity = traffic[movementIndex(Movement::W_N)] + traffic[movementIndex(Movement::W_S)]
        // Definit le comportement d une methode du simulateur.
        + traffic[movementIndex(Movement::E_N)] + traffic[movementIndex(Movement::E_S)];
    // Definit le comportement d une methode du simulateur.
    loads.bypassToBypass = traffic[movementIndex(Movement::W_E)] + traffic[movementIndex(Movement::E_W)];
    // Retourne la valeur calculee par la fonction.
    return loads;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
Phase nextPhase(Phase current, Phase lastGreenPhase)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (current) {
    // Traite un cas particulier dans la selection.
    case Phase::NsGreen:
        // Retourne la valeur calculee par la fonction.
        return Phase::NsOrange;
    // Traite un cas particulier dans la selection.
    case Phase::NsOrange:
        // Retourne la valeur calculee par la fonction.
        return Phase::Safety;
    // Traite un cas particulier dans la selection.
    case Phase::Safety:
        // Retourne la valeur calculee par la fonction.
        return lastGreenPhase == Phase::NsGreen ? Phase::EwGreen : Phase::NsGreen;
    // Traite un cas particulier dans la selection.
    case Phase::EwGreen:
        // Retourne la valeur calculee par la fonction.
        return Phase::EwOrange;
    // Traite un cas particulier dans la selection.
    case Phase::EwOrange:
        // Retourne la valeur calculee par la fonction.
        return Phase::Safety;
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return Phase::NsGreen;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
MovementArray allowedMovements(const TrafficArray& traffic, Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    MovementArray movements{};
    // Execute cette instruction dans le flux normal du programme.
    movements.fill(0);

    // Execute cette instruction dans le flux normal du programme.
    std::array<Branch, 2> activeBranches{};
    // Initialise ou met a jour une valeur du programme.
    int branchCount = 0;
    // Verifie une condition avant d executer ce bloc.
    if (isNorthSouthPhase(phase)) {
        // Initialise ou met a jour une valeur du programme.
        activeBranches[branchCount++] = Branch::North;
        // Initialise ou met a jour une valeur du programme.
        activeBranches[branchCount++] = Branch::South;
    // Execute cette instruction dans le flux normal du programme.
    } else if (isEastWestPhase(phase)) {
        // Initialise ou met a jour une valeur du programme.
        activeBranches[branchCount++] = Branch::West;
        // Initialise ou met a jour une valeur du programme.
        activeBranches[branchCount++] = Branch::East;
    // Ferme le bloc d instructions courant.
    }

    // Parcourt une suite de valeurs ou d elements.
    for (int i = 0; i < branchCount; ++i) {
        // Definit le comportement d une methode du simulateur.
        const Branch branch = activeBranches[static_cast<std::size_t>(i)];
        // Initialise ou met a jour une valeur du programme.
        int capacity = branchCapacity(phase, branch);
        // Initialise ou met a jour une valeur du programme.
        auto order = branchMovements(branch);

        // Repete le traitement tant que la condition reste vraie.
        while (capacity > 0) {
            // Definit le comportement d une methode du simulateur.
            std::sort(order.begin(), order.end(), [&](Movement left, Movement right) {
                // Retourne la valeur calculee par la fonction.
                return scoreService(traffic, movements, left) > scoreService(traffic, movements, right);
            // Execute cette instruction dans le flux normal du programme.
            });

            // Initialise ou met a jour une valeur du programme.
            bool progressed = false;
            // Parcourt une suite de valeurs ou d elements.
            for (const Movement movement : order) {
                // Verifie une condition avant d executer ce bloc.
                if (capacity <= 0) {
                    // Interrompt immediatement le bloc de controle courant.
                    break;
                // Ferme le bloc d instructions courant.
                }

                // Verifie une condition avant d executer ce bloc.
                if (remainingDemand(traffic, movements, movement) <= 0) {
                    // Passe a l iteration suivante de la boucle.
                    continue;
                // Ferme le bloc d instructions courant.
                }

                // Execute cette instruction dans le flux normal du programme.
                ++movements[movementIndex(movement)];
                // Execute cette instruction dans le flux normal du programme.
                --capacity;
                // Initialise ou met a jour une valeur du programme.
                progressed = true;
            // Ferme le bloc d instructions courant.
            }

            // Verifie une condition avant d executer ce bloc.
            if (!progressed) {
                // Interrompt immediatement le bloc de controle courant.
                break;
            // Ferme le bloc d instructions courant.
            }
        // Ferme le bloc d instructions courant.
        }
    // Ferme le bloc d instructions courant.
    }

    // Retourne la valeur calculee par la fonction.
    return movements;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
TrafficArray updateTraffic(const TrafficArray& traffic, const MovementArray& movements, std::mt19937& rng)
// Ouvre un nouveau bloc d instructions.
{
    // Initialise ou met a jour une valeur du programme.
    TrafficArray updated = traffic;

    // Parcourt une suite de valeurs ou d elements.
    for (const Movement movement : allMovements()) {
        // Declare une constante ou une valeur partagee par le systeme.
        const int index = movementIndex(movement);
        // Definit le comportement d une methode du simulateur.
        updated[index] = std::max(0, updated[index] - movements[index]);
    // Ferme le bloc d instructions courant.
    }

    // Parcourt une suite de valeurs ou d elements.
    for (const Branch branch : allBranches()) {
        // Declare une constante ou une valeur partagee par le systeme.
        const auto [minimum, maximum] = branchDemandRange(branch);
        // Definit le comportement d une methode du simulateur.
        std::uniform_int_distribution<int> demandDistribution(minimum, maximum);
        // Declare une constante ou une valeur partagee par le systeme.
        const int arrivals = demandDistribution(rng);
        // Declare une constante ou une valeur partagee par le systeme.
        const auto weights = branchArrivalWeights(branch);
        // Definit le comportement d une methode du simulateur.
        std::discrete_distribution<int> pick(weights.begin(), weights.end());
        // Declare une constante ou une valeur partagee par le systeme.
        const auto& options = branchMovements(branch);

        // Parcourt une suite de valeurs ou d elements.
        for (int i = 0; i < arrivals; ++i) {
            // Definit le comportement d une methode du simulateur.
            const Movement selected = options[static_cast<std::size_t>(pick(rng))];
            // Declare une constante ou une valeur partagee par le systeme.
            const int index = movementIndex(selected);
            // Definit le comportement d une methode du simulateur.
            updated[index] = std::min(updated[index] + 1, 999);
        // Ferme le bloc d instructions courant.
        }
    // Ferme le bloc d instructions courant.
    }

    // Retourne la valeur calculee par la fonction.
    return updated;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
int adaptiveDuration(Phase phase, const TrafficArray& traffic)
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const AxisLoads loads = axisLoads(traffic);

    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::NsGreen) {
        // Definit le comportement d une methode du simulateur.
        const int charge = std::max(loads.northSouth, loads.universityToUniversity + loads.universityToBypass);
        // Verifie une condition avant d executer ce bloc.
        if (charge <= kUnderloadThreshold[phaseIndex(phase)]) {
            // Retourne la valeur calculee par la fonction.
            return kGreenMinDuration[phaseIndex(phase)];
        // Ferme le bloc d instructions courant.
        }
        // Verifie une condition avant d executer ce bloc.
        if (charge >= kOverloadThreshold) {
            // Retourne la valeur calculee par la fonction.
            return kGreenMaxDuration[phaseIndex(phase)];
        // Ferme le bloc d instructions courant.
        }
        // Verifie une condition avant d executer ce bloc.
        if (charge >= kCongestionThreshold) {
            // Retourne la valeur calculee par la fonction.
            return std::min(kPhaseBaseDuration[phaseIndex(phase)] + kGreenExtension, kGreenMaxDuration[phaseIndex(phase)]);
        // Ferme le bloc d instructions courant.
        }
        // Retourne la valeur calculee par la fonction.
        return kPhaseBaseDuration[phaseIndex(phase)];
    // Ferme le bloc d instructions courant.
    }

    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::EwGreen) {
        // Definit le comportement d une methode du simulateur.
        const int charge = std::max(loads.eastWest, loads.bypassToUniversity + loads.bypassToBypass);
        // Verifie une condition avant d executer ce bloc.
        if (charge <= kUnderloadThreshold[phaseIndex(phase)]) {
            // Retourne la valeur calculee par la fonction.
            return kGreenMinDuration[phaseIndex(phase)];
        // Ferme le bloc d instructions courant.
        }
        // Verifie une condition avant d executer ce bloc.
        if (charge >= kOverloadThreshold) {
            // Retourne la valeur calculee par la fonction.
            return kGreenMaxDuration[phaseIndex(phase)];
        // Ferme le bloc d instructions courant.
        }
        // Verifie une condition avant d executer ce bloc.
        if (charge >= kCongestionThreshold) {
            // Retourne la valeur calculee par la fonction.
            return std::min(kPhaseBaseDuration[phaseIndex(phase)] + kGreenExtension, kGreenMaxDuration[phaseIndex(phase)]);
        // Ferme le bloc d instructions courant.
        }
        // Retourne la valeur calculee par la fonction.
        return kPhaseBaseDuration[phaseIndex(phase)];
    // Ferme le bloc d instructions courant.
    }

    // Retourne la valeur calculee par la fonction.
    return kPhaseBaseDuration[phaseIndex(phase)];
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QString adaptationHint(Phase phase, const TrafficArray& traffic)
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const AxisLoads loads = axisLoads(traffic);
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::NsGreen && loads.northSouth >= kCongestionThreshold) {
        // Retourne la valeur calculee par la fonction.
        return "Prolongation axe Universite";
    // Ferme le bloc d instructions courant.
    }
    // Verifie une condition avant d executer ce bloc.
    if (phase == Phase::EwGreen && loads.eastWest >= kCongestionThreshold) {
        // Retourne la valeur calculee par la fonction.
        return "Prolongation axe By-Pass";
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return "Flux nominal";
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
const QVector<QPointF>& routePoints(Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kRoutes[static_cast<std::size_t>(movementIndex(movement))];
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QueueVisualConfig queueConfig(Branch branch)
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return kQueueConfigs[static_cast<std::size_t>(branchIndex(branch))];
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
std::array<LightPair, kBranchCount> lightState(Phase phase)
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    std::array<LightPair, kBranchCount> lights{};
    // Execute cette instruction dans le flux normal du programme.
    lights.fill({LightColor::Red, LightColor::Green});

    // Selectionne un traitement selon la valeur courante.
    switch (phase) {
    // Traite un cas particulier dans la selection.
    case Phase::NsGreen:
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::North)].forward = LightColor::Green;
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::South)].forward = LightColor::Green;
        // Interrompt immediatement le bloc de controle courant.
        break;
    // Traite un cas particulier dans la selection.
    case Phase::NsOrange:
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::North)].forward = LightColor::Orange;
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::South)].forward = LightColor::Orange;
        // Interrompt immediatement le bloc de controle courant.
        break;
    // Traite un cas particulier dans la selection.
    case Phase::EwGreen:
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::West)].forward = LightColor::Green;
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::East)].forward = LightColor::Green;
        // Interrompt immediatement le bloc de controle courant.
        break;
    // Traite un cas particulier dans la selection.
    case Phase::EwOrange:
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::West)].forward = LightColor::Orange;
        // Initialise ou met a jour une valeur du programme.
        lights[branchIndex(Branch::East)].forward = LightColor::Orange;
        // Interrompt immediatement le bloc de controle courant.
        break;
    // Traite un cas particulier dans la selection.
    case Phase::Safety:
        // Interrompt immediatement le bloc de controle courant.
        break;
    // Ferme le bloc d instructions courant.
    }

    // Retourne la valeur calculee par la fonction.
    return lights;
// Ferme le bloc d instructions courant.
}

// Ferme le bloc d instructions courant.
}  // namespace ngaba

