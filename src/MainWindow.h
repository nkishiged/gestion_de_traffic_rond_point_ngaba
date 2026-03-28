// Evite les inclusions multiples de ce fichier.
#pragma once

// Inclut une dependance necessaire a ce fichier.
#include <QMainWindow>

// Declare une classe utilisee par l application.
class IntersectionWidget;
// Declare une classe utilisee par l application.
class SimulationEngine;

// Declare une classe utilisee par l application.
class MainWindow : public QMainWindow
// Ouvre un nouveau bloc d instructions.
{
    // Active les mecanismes meta objet de Qt pour cette classe.
    Q_OBJECT

// Definit le niveau d acces des membres qui suivent.
public:
    // Declare un constructeur explicite pour eviter les conversions implicites.
    explicit MainWindow(QWidget* parent = nullptr);

// Definit le niveau d acces des membres qui suivent.
private:
    // Initialise ou met a jour une valeur du programme.
    SimulationEngine* m_engine = nullptr;
    // Initialise ou met a jour une valeur du programme.
    IntersectionWidget* m_intersectionWidget = nullptr;
// Execute cette instruction dans le flux normal du programme.
};
