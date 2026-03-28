// Evite les inclusions multiples de ce fichier.
#pragma once

// Inclut une dependance necessaire a ce fichier.
#include <QColor>
// Inclut une dependance necessaire a ce fichier.
#include <QPointF>
// Inclut une dependance necessaire a ce fichier.
#include <QWidget>

// Inclut une dependance necessaire a ce fichier.
#include "SimulationEngine.h"

// Declare une classe utilisee par l application.
class QPaintEvent;
// Declare une classe utilisee par l application.
class QPainter;

// Declare une classe utilisee par l application.
class IntersectionWidget : public QWidget
// Ouvre un nouveau bloc d instructions.
{
    // Active les mecanismes meta objet de Qt pour cette classe.
    Q_OBJECT

// Definit le niveau d acces des membres qui suivent.
public:
    // Declare un constructeur explicite pour eviter les conversions implicites.
    explicit IntersectionWidget(SimulationEngine* engine, QWidget* parent = nullptr);

    // Declare ou utilise une fonction necessaire au programme.
    QSize minimumSizeHint() const override;

// Definit le niveau d acces des membres qui suivent.
protected:
    // Declare ou utilise une fonction necessaire au programme.
    void paintEvent(QPaintEvent* event) override;

// Definit le niveau d acces des membres qui suivent.
private:
    // Declare ou utilise une fonction necessaire au programme.
    QPointF scenePoint(const QPointF& modelPoint) const;
    // Declare ou utilise une fonction necessaire au programme.
    qreal directionAngle(const QPointF& from, const QPointF& to) const;
    // Declare ou utilise une fonction necessaire au programme.
    QPointF pointOnRoute(ngaba::Movement movement, qreal progress, qreal* angleDeg = nullptr) const;
    // Declare ou utilise une fonction necessaire au programme.
    QPointF clampAwayFromIsland(const QPointF& point) const;
    // Declare ou utilise une fonction necessaire au programme.
    QColor movementColor(ngaba::Movement movement) const;

    // Declare ou utilise une fonction necessaire au programme.
    void drawGround(QPainter& painter) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawRoads(QPainter& painter) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawLaneMarkings(QPainter& painter) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawCentralIsland(QPainter& painter) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawSignals(QPainter& painter, const SimulationSnapshot& snapshot) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawWaitingVehicles(QPainter& painter, const SimulationSnapshot& snapshot) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawMovingVehicles(QPainter& painter, const SimulationSnapshot& snapshot) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawCrosswalk(QPainter& painter, const QPointF& center, bool horizontal) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawSignal(QPainter& painter, const QPointF& center, bool vertical, ngaba::LightColor color) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawVehicle(QPainter& painter, const QPointF& center, qreal angleDeg, const QColor& bodyColor, qreal length, qreal width) const;
    // Declare ou utilise une fonction necessaire au programme.
    void drawArrow(QPainter& painter, const QPointF& start, const QPointF& end) const;

    // Initialise ou met a jour une valeur du programme.
    SimulationEngine* m_engine = nullptr;
// Execute cette instruction dans le flux normal du programme.
};
