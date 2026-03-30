// Inclut une dependance necessaire a ce fichier.
#include "IntersectionWidget.h"

// Inclut une dependance necessaire a ce fichier.
#include <QCoreApplication>
// Inclut une dependance necessaire a ce fichier.
#include <QDir>
// Inclut une dependance necessaire a ce fichier.
#include <QFileInfo>
// Inclut une dependance necessaire a ce fichier.
#include <QImage>
// Inclut une dependance necessaire a ce fichier.
#include <QLineF>
// Inclut une dependance necessaire a ce fichier.
#include <QLinearGradient>
// Inclut une dependance necessaire a ce fichier.
#include <QPaintEvent>
// Inclut une dependance necessaire a ce fichier.
#include <QPainter>
// Inclut une dependance necessaire a ce fichier.
#include <QPainterPath>
// Inclut une dependance necessaire a ce fichier.
#include <QPen>
// Inclut une dependance necessaire a ce fichier.
#include <QPolygonF>
// Inclut une dependance necessaire a ce fichier.
#include <QRadialGradient>
// Inclut une dependance necessaire a ce fichier.
#include <QStringList>

// Inclut une dependance necessaire a ce fichier.
#include <algorithm>
// Inclut une dependance necessaire a ce fichier.
#include <array>
// Inclut une dependance necessaire a ce fichier.
#include <cmath>

// Ouvre un espace de noms pour organiser le code.
namespace {

// Declare une constante ou une valeur partagee par le systeme.
constexpr qreal kPi = 3.14159265358979323846;
// Declare ou utilise une fonction necessaire au programme.
const QPointF kRoundaboutCenter(55.0, 48.5);
// Declare une constante ou une valeur partagee par le systeme.
constexpr qreal kIslandRadiusX = 3.2;
// Declare une constante ou une valeur partagee par le systeme.
constexpr qreal kIslandRadiusY = 4.6;
// Declare une constante ou une valeur partagee par le systeme.
constexpr qreal kSafetyRadiusX = 5.3;
// Declare une constante ou une valeur partagee par le systeme.
constexpr qreal kSafetyRadiusY = 7.1;

// Indique si le mouvement correspond a la premiere sortie apres entree dans l'anneau.
bool isShortTurn(ngaba::Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    switch (movement) {
    case ngaba::Movement::N_E:
    case ngaba::Movement::S_W:
    case ngaba::Movement::W_N:
    case ngaba::Movement::E_S:
        return true;
    default:
        return false;
    }
// Ferme le bloc d instructions courant.
}

// Indique si le mouvement correspond a une traversee vers la sortie opposee.
bool isCrossRingMovement(ngaba::Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    switch (movement) {
    case ngaba::Movement::N_S:
    case ngaba::Movement::S_N:
    case ngaba::Movement::W_E:
    case ngaba::Movement::E_W:
        return true;
    default:
        return false;
    }
// Ferme le bloc d instructions courant.
}

// Retourne l'intensite de contrainte sur l'anneau selon le type de mouvement.
qreal ringTargetNorm(ngaba::Movement movement)
// Ouvre un nouveau bloc d instructions.
{
    if (isShortTurn(movement)) {
        return 1.24;
    }
    if (isCrossRingMovement(movement)) {
        return 1.34;
    }
    return 1.48;
// Ferme le bloc d instructions courant.
}

// Force davantage les vehicules a longer l'anneau plutot que couper le centre.
QPointF enforceRoundaboutRing(const QPointF& point, ngaba::Movement movement, qreal progress)
// Ouvre un nouveau bloc d instructions.
{
    // N'agit surtout que dans la portion centrale de la trajectoire.
    if (progress <= 0.18 || progress >= 0.86) {
        return point;
    }

    // Cree un poids lisse qui renforce la contrainte vers le milieu du trajet.
    const qreal normalized = std::clamp((progress - 0.18) / 0.68, 0.0, 1.0);
    const qreal envelope = std::sin(normalized * kPi);
    const qreal desiredNorm = 1.02 + (ringTargetNorm(movement) - 1.02) * envelope;

    const qreal dx = point.x() - kRoundaboutCenter.x();
    const qreal dy = point.y() - kRoundaboutCenter.y();
    const qreal norm = std::sqrt((dx * dx) / (kSafetyRadiusX * kSafetyRadiusX)
        + (dy * dy) / (kSafetyRadiusY * kSafetyRadiusY));

    if (norm <= 0.0001 || norm >= desiredNorm) {
        return point;
    }

    const qreal scale = desiredNorm / norm;
    return QPointF(kRoundaboutCenter.x() + dx * scale, kRoundaboutCenter.y() + dy * scale);
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QColor lightToColor(ngaba::LightColor color)
// Ouvre un nouveau bloc d instructions.
{
    // Selectionne un traitement selon la valeur courante.
    switch (color) {
    // Traite un cas particulier dans la selection.
    case ngaba::LightColor::Green:
        // Retourne la valeur calculee par la fonction.
        return QColor("#36D971");
    // Traite un cas particulier dans la selection.
    case ngaba::LightColor::Orange:
        // Retourne la valeur calculee par la fonction.
        return QColor("#FFB547");
    // Traite un cas particulier dans la selection.
    case ngaba::LightColor::Red:
    // Definit le comportement par defaut dans cette selection.
    default:
        // Retourne la valeur calculee par la fonction.
        return QColor("#FF5A52");
    // Ferme le bloc d instructions courant.
    }
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
const QImage& satelliteImage()
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    static QImage image;
    // Declare une constante ou une valeur partagee par le systeme.
    static bool attemptedLoad = false;

    // Verifie une condition avant d executer ce bloc.
    if (!attemptedLoad) {
        // Initialise ou met a jour une valeur du programme.
        attemptedLoad = true;

        // Definit le comportement d une methode du simulateur.
        const QString appDir = QCoreApplication::applicationDirPath();
        // Definit le comportement d une methode du simulateur.
        const QString currentDir = QDir::currentPath();
        // Declare une constante ou une valeur partagee par le systeme.
        const QStringList candidates{
            // Execute cette instruction dans le flux normal du programme.
            QDir(currentDir).absoluteFilePath("assets/ngaba_satellite_style.png"),
            // Execute cette instruction dans le flux normal du programme.
            QDir(appDir).absoluteFilePath("assets/ngaba_satellite_style.png"),
            // Execute cette instruction dans le flux normal du programme.
            QDir(appDir).absoluteFilePath("../assets/ngaba_satellite_style.png"),
            // Declare ou utilise une fonction necessaire au programme.
            QDir(appDir).absoluteFilePath("../../assets/ngaba_satellite_style.png")
        // Execute cette instruction dans le flux normal du programme.
        };

        // Parcourt une suite de valeurs ou d elements.
        for (const QString& candidate : candidates) {
            // Verifie une condition avant d executer ce bloc.
            if (QFileInfo::exists(candidate) && image.load(candidate)) {
                // Interrompt immediatement le bloc de controle courant.
                break;
            // Ferme le bloc d instructions courant.
            }
        // Ferme le bloc d instructions courant.
        }
    // Ferme le bloc d instructions courant.
    }

    // Retourne la valeur calculee par la fonction.
    return image;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QPainterPath byPassCenterLine()
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    QPainterPath path;
    // Execute cette instruction dans le flux normal du programme.
    path.moveTo(-4.0, 49.6);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(16.0, 48.5, 33.0, 47.8, 45.5, 47.2);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(55.0, 46.8, 65.0, 47.5, 76.0, 49.8);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(87.0, 52.0, 96.0, 54.2, 104.0, 55.0);
    // Retourne la valeur calculee par la fonction.
    return path;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QPainterPath universityCenterLine()
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    QPainterPath path;
    // Execute cette instruction dans le flux normal du programme.
    path.moveTo(55.8, -4.0);
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(55.8, 31.0);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(55.8, 37.5, 55.6, 43.0, 55.2, 48.2);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(54.9, 55.5, 54.1, 65.0, 53.3, 78.0);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(52.9, 87.0, 52.6, 96.0, 52.2, 104.0);
    // Retourne la valeur calculee par la fonction.
    return path;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QPainterPath chapelleBranchCenterLine()
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    QPainterPath path;
    // Execute cette instruction dans le flux normal du programme.
    path.moveTo(58.3, 56.0);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(60.8, 64.0, 62.2, 73.0, 63.8, 84.0);
    // Execute cette instruction dans le flux normal du programme.
    path.cubicTo(64.6, 91.0, 65.5, 98.0, 66.5, 104.0);
    // Retourne la valeur calculee par la fonction.
    return path;
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QRectF islandRect()
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return QRectF(kRoundaboutCenter.x() - kIslandRadiusX, kRoundaboutCenter.y() - kIslandRadiusY,
        // Execute cette instruction dans le flux normal du programme.
        kIslandRadiusX * 2.0, kIslandRadiusY * 2.0);
// Ferme le bloc d instructions courant.
}

// Execute cette instruction dans le flux normal du programme.
void strokeRoad(QPainter& painter, const QPainterPath& path, qreal width,
    // Declare une constante ou une valeur partagee par le systeme.
    const QColor& shoulder, const QColor& asphaltTop, const QColor& asphaltBottom,
    // Declare une constante ou une valeur partagee par le systeme.
    const QColor& edgeHighlight)
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    painter.save();

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(0, 0, 0, 34), width + 1.9, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(path);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(shoulder, width + 0.72, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(path);

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient asphalt(0.0, 0.0, 100.0, 100.0);
    // Execute cette instruction dans le flux normal du programme.
    asphalt.setColorAt(0.0, asphaltTop);
    // Execute cette instruction dans le flux normal du programme.
    asphalt.setColorAt(0.55, asphaltBottom);
    // Execute cette instruction dans le flux normal du programme.
    asphalt.setColorAt(1.0, asphaltTop.darker(118));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QBrush(asphalt), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(path);

    // Definit le comportement d une methode du simulateur.
    painter.setPen(QPen(QColor(255, 255, 255, 12), std::max<qreal>(0.18, width * 0.10), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(path);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(edgeHighlight, 0.18, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(path);

    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Declare ou utilise une fonction necessaire au programme.
QPainterPath arrowShape(const QPointF& start, const QPointF& end, qreal shaftWidth, qreal headLength, qreal headWidth)
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    const QLineF line(start, end);
    // Verifie une condition avant d executer ce bloc.
    if (line.length() < 0.01) {
        // Retourne la valeur calculee par la fonction.
        return {};
    // Ferme le bloc d instructions courant.
    }

    // Definit le comportement d une methode du simulateur.
    const qreal angle = std::atan2(line.dy(), line.dx());
    // Definit le comportement d une methode du simulateur.
    const QPointF direction(std::cos(angle), std::sin(angle));
    // Declare ou utilise une fonction necessaire au programme.
    const QPointF normal(-direction.y(), direction.x());
    // Declare une constante ou une valeur partagee par le systeme.
    const QPointF headBase = end - direction * headLength;

    // Execute cette instruction dans le flux normal du programme.
    QPainterPath path;
    // Execute cette instruction dans le flux normal du programme.
    path.moveTo(start + normal * (shaftWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(headBase + normal * (shaftWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(headBase + normal * (headWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(end);
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(headBase - normal * (headWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(headBase - normal * (shaftWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.lineTo(start - normal * (shaftWidth / 2.0));
    // Execute cette instruction dans le flux normal du programme.
    path.closeSubpath();
    // Retourne la valeur calculee par la fonction.
    return path;
// Ferme le bloc d instructions courant.
}

// Ferme le bloc d instructions courant.
}  // namespace

// Definit le comportement d une methode du simulateur.
IntersectionWidget::IntersectionWidget(SimulationEngine* engine, QWidget* parent)
    // Execute cette instruction dans le flux normal du programme.
    : QWidget(parent)
    // Poursuit la liste d initialisation ou d arguments en cours.
    , m_engine(engine)
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    setAttribute(Qt::WA_OpaquePaintEvent);
    // Declare ou utilise une fonction necessaire au programme.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Verifie une condition avant d executer ce bloc.
    if (m_engine != nullptr) {
        // Definit le comportement d une methode du simulateur.
        connect(m_engine, &SimulationEngine::stateChanged, this, qOverload<>(&IntersectionWidget::update));
    // Ferme le bloc d instructions courant.
    }
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
QSize IntersectionWidget::minimumSizeHint() const
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return QSize(1100, 760);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::paintEvent(QPaintEvent* event)
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    Q_UNUSED(event)

    // Verifie une condition avant d executer ce bloc.
    if (m_engine == nullptr) {
        // Retourne la valeur calculee par la fonction.
        return;
    // Ferme le bloc d instructions courant.
    }

    // Declare une constante ou une valeur partagee par le systeme.
    const SimulationSnapshot& snapshot = m_engine->snapshot();

    // Declare ou utilise une fonction necessaire au programme.
    QPainter painter(this);
    // Execute cette instruction dans le flux normal du programme.
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Execute cette instruction dans le flux normal du programme.
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    // Execute cette instruction dans le flux normal du programme.
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    // Execute cette instruction dans le flux normal du programme.
    painter.fillRect(rect(), QColor("#0D1218"));

    // Execute cette instruction dans le flux normal du programme.
    painter.scale(width() / 100.0, height() / 100.0);

    // Declare ou utilise une fonction necessaire au programme.
    drawGround(painter);
    // Declare ou utilise une fonction necessaire au programme.
    drawRoads(painter);
    // Declare ou utilise une fonction necessaire au programme.
    drawLaneMarkings(painter);
    // Declare ou utilise une fonction necessaire au programme.
    drawCentralIsland(painter);
    // Declare ou utilise une fonction necessaire au programme.
    drawWaitingVehicles(painter, snapshot);
    // Declare ou utilise une fonction necessaire au programme.
    drawMovingVehicles(painter, snapshot);
    // Declare ou utilise une fonction necessaire au programme.
    drawSignals(painter, snapshot);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
QPointF IntersectionWidget::scenePoint(const QPointF& modelPoint) const
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return QPointF(modelPoint.x(), 100.0 - modelPoint.y());
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
qreal IntersectionWidget::directionAngle(const QPointF& from, const QPointF& to) const
// Ouvre un nouveau bloc d instructions.
{
    // Retourne la valeur calculee par la fonction.
    return std::atan2(to.y() - from.y(), to.x() - from.x()) * 180.0 / kPi;
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
QPointF IntersectionWidget::pointOnRoute(ngaba::Movement movement, qreal progress, qreal* angleDeg) const
// Ouvre un nouveau bloc d instructions.
{
    // Definit le comportement d une methode du simulateur.
    const QVector<QPointF>& modelPoints = ngaba::routePoints(movement);
    // Verifie une condition avant d executer ce bloc.
    if (modelPoints.isEmpty()) {
        // Verifie une condition avant d executer ce bloc.
        if (angleDeg != nullptr) {
            // Initialise ou met a jour une valeur du programme.
            *angleDeg = 0.0;
        // Ferme le bloc d instructions courant.
        }
        // Retourne la valeur calculee par la fonction.
        return QPointF();
    // Ferme le bloc d instructions courant.
    }

    // Execute cette instruction dans le flux normal du programme.
    QVector<QPointF> points;
    // Execute cette instruction dans le flux normal du programme.
    points.reserve(modelPoints.size());
    // Parcourt une suite de valeurs ou d elements.
    for (const QPointF& point : modelPoints) {
        // Execute cette instruction dans le flux normal du programme.
        points.append(scenePoint(point));
    // Ferme le bloc d instructions courant.
    }

    // Verifie une condition avant d executer ce bloc.
    if (points.size() == 1) {
        // Verifie une condition avant d executer ce bloc.
        if (angleDeg != nullptr) {
            // Initialise ou met a jour une valeur du programme.
            *angleDeg = 0.0;
        // Ferme le bloc d instructions courant.
        }
        // Retourne la valeur calculee par la fonction.
        return points.first();
    // Ferme le bloc d instructions courant.
    }

    // Definit le comportement d une methode du simulateur.
    const qreal clamped = std::clamp(progress, 0.0, 1.0);
    // Initialise ou met a jour une valeur du programme.
    qreal totalLength = 0.0;
    // Parcourt une suite de valeurs ou d elements.
    for (int i = 1; i < points.size(); ++i) {
        // Initialise ou met a jour une valeur du programme.
        totalLength += QLineF(points[i - 1], points[i]).length();
    // Ferme le bloc d instructions courant.
    }

    // Declare une constante ou une valeur partagee par le systeme.
    const qreal target = totalLength * clamped;
    // Initialise ou met a jour une valeur du programme.
    qreal walked = 0.0;
    // Parcourt une suite de valeurs ou d elements.
    for (int i = 1; i < points.size(); ++i) {
        // Declare ou utilise une fonction necessaire au programme.
        const QLineF segment(points[i - 1], points[i]);
        // Declare une constante ou une valeur partagee par le systeme.
        const qreal segmentLength = segment.length();
        // Verifie une condition avant d executer ce bloc.
        if (walked + segmentLength >= target || i == points.size() - 1) {
            // Declare une constante ou une valeur partagee par le systeme.
            const qreal local = segmentLength <= 0.0 ? 0.0 : (target - walked) / segmentLength;
            // Definit le comportement d une methode du simulateur.
            QPointF point = segment.pointAt(std::clamp(local, 0.0, 1.0));
            point = enforceRoundaboutRing(point, movement, clamped);
            // Verifie une condition avant d executer ce bloc.
            if (angleDeg != nullptr) {
                // Recalcule une tangente locale visible apres application de la contrainte annulaire.
                const qreal preview = std::min<qreal>(1.0, clamped + 0.018);
                qreal previewAngle = 0.0;
                QPointF previewPoint;
                if (preview > clamped) {
                    const qreal previewTarget = totalLength * preview;
                    qreal previewWalked = 0.0;
                    previewPoint = point;
                    for (int j = 1; j < points.size(); ++j) {
                        const QLineF previewSegment(points[j - 1], points[j]);
                        const qreal previewLength = previewSegment.length();
                        if (previewWalked + previewLength >= previewTarget || j == points.size() - 1) {
                            const qreal previewLocal = previewLength <= 0.0 ? 0.0 : (previewTarget - previewWalked) / previewLength;
                            previewPoint = previewSegment.pointAt(std::clamp(previewLocal, 0.0, 1.0));
                            previewPoint = enforceRoundaboutRing(previewPoint, movement, preview);
                            previewAngle = directionAngle(point, previewPoint);
                            break;
                        }
                        previewWalked += previewLength;
                    }
                } else {
                    previewAngle = directionAngle(segment.p1(), segment.p2());
                }
                *angleDeg = previewAngle;
            // Ferme le bloc d instructions courant.
            }
            // Retourne la valeur calculee par la fonction.
            return point;
        // Ferme le bloc d instructions courant.
        }
        // Initialise ou met a jour une valeur du programme.
        walked += segmentLength;
    // Ferme le bloc d instructions courant.
    }

    // Verifie une condition avant d executer ce bloc.
    if (angleDeg != nullptr) {
        // Initialise ou met a jour une valeur du programme.
        *angleDeg = directionAngle(points[points.size() - 2], points.last());
    // Ferme le bloc d instructions courant.
    }
    // Retourne la valeur calculee par la fonction.
    return points.last();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
QPointF IntersectionWidget::clampAwayFromIsland(const QPointF& point) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const qreal dx = point.x() - kRoundaboutCenter.x();
    // Declare une constante ou une valeur partagee par le systeme.
    const qreal dy = point.y() - kRoundaboutCenter.y();
    // Definit le comportement d une methode du simulateur.
    const qreal norm = std::sqrt((dx * dx) / (kSafetyRadiusX * kSafetyRadiusX) + (dy * dy) / (kSafetyRadiusY * kSafetyRadiusY));

    // Verifie une condition avant d executer ce bloc.
    if (norm >= 1.0 || norm <= 0.0001) {
        // Retourne la valeur calculee par la fonction.
        return point;
    // Ferme le bloc d instructions courant.
    }

    // Declare une constante ou une valeur partagee par le systeme.
    const qreal scale = 1.08 / norm;
    // Retourne la valeur calculee par la fonction.
    return QPointF(kRoundaboutCenter.x() + dx * scale, kRoundaboutCenter.y() + dy * scale);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
QColor IntersectionWidget::movementColor(ngaba::Movement movement) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    static const std::array<QColor, 6> palette{{
        // Execute cette instruction dans le flux normal du programme.
        QColor("#E4574F"),
        // Execute cette instruction dans le flux normal du programme.
        QColor("#2C7DA0"),
        // Execute cette instruction dans le flux normal du programme.
        QColor("#2A9D8F"),
        // Execute cette instruction dans le flux normal du programme.
        QColor("#F2C14E"),
        // Execute cette instruction dans le flux normal du programme.
        QColor("#748CAB"),
        // Execute cette instruction dans le flux normal du programme.
        QColor("#F28F3B"),
    // Execute cette instruction dans le flux normal du programme.
    }};
    // Retourne la valeur calculee par la fonction.
    return palette[static_cast<std::size_t>(ngaba::movementIndex(movement) % static_cast<int>(palette.size()))];
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawGround(QPainter& painter) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    const QRectF worldRect(0.0, 0.0, 100.0, 100.0);
    // Declare une constante ou une valeur partagee par le systeme.
    const QImage& background = satelliteImage();
    // Verifie une condition avant d executer ce bloc.
    if (!background.isNull()) {
        // Execute cette instruction dans le flux normal du programme.
        painter.drawImage(worldRect, background, QRectF(0.0, 0.0, background.width(), background.height()));
    // Execute cette instruction dans le flux normal du programme.
    } else {
        // Declare ou utilise une fonction necessaire au programme.
        QLinearGradient earth(0.0, 0.0, 100.0, 100.0);
        // Execute cette instruction dans le flux normal du programme.
        earth.setColorAt(0.0, QColor("#70795B"));
        // Execute cette instruction dans le flux normal du programme.
        earth.setColorAt(1.0, QColor("#556447"));
        // Execute cette instruction dans le flux normal du programme.
        painter.fillRect(worldRect, QBrush(earth));
    // Ferme le bloc d instructions courant.
    }

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient atmosphere(0.0, 0.0, 100.0, 100.0);
    // Execute cette instruction dans le flux normal du programme.
    atmosphere.setColorAt(0.0, QColor(9, 16, 25, 28));
    // Execute cette instruction dans le flux normal du programme.
    atmosphere.setColorAt(0.38, QColor(18, 24, 26, 10));
    // Execute cette instruction dans le flux normal du programme.
    atmosphere.setColorAt(1.0, QColor(153, 111, 54, 20));
    // Execute cette instruction dans le flux normal du programme.
    painter.fillRect(worldRect, QBrush(atmosphere));

    // Declare ou utilise une fonction necessaire au programme.
    QRadialGradient warmLight(18.0, 20.0, 46.0);
    // Execute cette instruction dans le flux normal du programme.
    warmLight.setColorAt(0.0, QColor(255, 217, 152, 46));
    // Execute cette instruction dans le flux normal du programme.
    warmLight.setColorAt(0.45, QColor(255, 217, 152, 14));
    // Execute cette instruction dans le flux normal du programme.
    warmLight.setColorAt(1.0, QColor(255, 217, 152, 0));
    // Execute cette instruction dans le flux normal du programme.
    painter.fillRect(worldRect, QBrush(warmLight));

    // Declare ou utilise une fonction necessaire au programme.
    QRadialGradient urbanGlow(kRoundaboutCenter, 24.0);
    // Execute cette instruction dans le flux normal du programme.
    urbanGlow.setColorAt(0.0, QColor(255, 255, 255, 16));
    // Execute cette instruction dans le flux normal du programme.
    urbanGlow.setColorAt(0.55, QColor(255, 255, 255, 3));
    // Execute cette instruction dans le flux normal du programme.
    urbanGlow.setColorAt(1.0, QColor(255, 255, 255, 0));
    // Execute cette instruction dans le flux normal du programme.
    painter.fillRect(worldRect, QBrush(urbanGlow));

    // Declare ou utilise une fonction necessaire au programme.
    QRadialGradient vignette(50.0, 50.0, 70.0);
    // Execute cette instruction dans le flux normal du programme.
    vignette.setColorAt(0.62, QColor(0, 0, 0, 0));
    // Execute cette instruction dans le flux normal du programme.
    vignette.setColorAt(1.0, QColor(5, 8, 12, 58));
    // Execute cette instruction dans le flux normal du programme.
    painter.fillRect(worldRect, QBrush(vignette));
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawRoads(QPainter& painter) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const QPainterPath byPass = byPassCenterLine();
    // Declare une constante ou une valeur partagee par le systeme.
    const QPainterPath university = universityCenterLine();
    // Declare une constante ou une valeur partagee par le systeme.
    const QPainterPath chapelle = chapelleBranchCenterLine();

    // Execute cette instruction dans le flux normal du programme.
    strokeRoad(painter, byPass, 11.8,
        // Execute cette instruction dans le flux normal du programme.
        QColor(198, 204, 210, 88),
        // Execute cette instruction dans le flux normal du programme.
        QColor(98, 104, 112, 174),
        // Execute cette instruction dans le flux normal du programme.
        QColor(78, 83, 90, 190),
        // Declare ou utilise une fonction necessaire au programme.
        QColor(247, 249, 252, 46));

    // Execute cette instruction dans le flux normal du programme.
    strokeRoad(painter, university, 10.8,
        // Execute cette instruction dans le flux normal du programme.
        QColor(196, 203, 208, 86),
        // Execute cette instruction dans le flux normal du programme.
        QColor(96, 103, 110, 176),
        // Execute cette instruction dans le flux normal du programme.
        QColor(76, 82, 89, 194),
        // Declare ou utilise une fonction necessaire au programme.
        QColor(247, 249, 252, 48));

    // Execute cette instruction dans le flux normal du programme.
    strokeRoad(painter, chapelle, 6.2,
        // Execute cette instruction dans le flux normal du programme.
        QColor(192, 198, 203, 68),
        // Execute cette instruction dans le flux normal du programme.
        QColor(92, 98, 105, 150),
        // Execute cette instruction dans le flux normal du programme.
        QColor(72, 78, 84, 162),
        // Declare ou utilise une fonction necessaire au programme.
        QColor(247, 249, 252, 34));

    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Execute cette instruction dans le flux normal du programme.
    painter.translate(kRoundaboutCenter);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(0, 0, 0, 32), 15.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QPointF(0.0, 0.0), 6.8, 8.5);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(206, 212, 217, 86), 14.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QPointF(0.0, 0.0), 6.8, 8.5);

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient ringAsphalt(-5.0, -8.0, 7.0, 9.0);
    // Execute cette instruction dans le flux normal du programme.
    ringAsphalt.setColorAt(0.0, QColor(104, 109, 116, 182));
    // Execute cette instruction dans le flux normal du programme.
    ringAsphalt.setColorAt(0.55, QColor(79, 84, 90, 196));
    // Execute cette instruction dans le flux normal du programme.
    ringAsphalt.setColorAt(1.0, QColor(60, 65, 71, 190));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QBrush(ringAsphalt), 12.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QPointF(0.0, 0.0), 6.8, 8.5);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(255, 255, 255, 12), 5.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QPointF(0.0, 0.0), 6.3, 8.0);
    // Execute cette instruction dans le flux normal du programme.
    painter.restore();

    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient splitter(0.0, 42.0, 0.0, 60.0);
    // Execute cette instruction dans le flux normal du programme.
    splitter.setColorAt(0.0, QColor(244, 246, 248, 168));
    // Execute cette instruction dans le flux normal du programme.
    splitter.setColorAt(1.0, QColor(214, 220, 226, 134));
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(splitter));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(54.3, 39.8, 1.3, 2.6), 0.32, 0.32);
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(54.3, 55.1, 1.3, 2.2), 0.30, 0.30);
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(44.8, 49.1, 2.4, 1.1), 0.26, 0.26);
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(62.4, 49.6, 2.2, 1.0), 0.26, 0.26);
    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawLaneMarkings(QPainter& painter) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    const QColor laneWhite(248, 250, 252, 160);
    // Declare ou utilise une fonction necessaire au programme.
    const QColor laneSoft(248, 250, 252, 104);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(laneWhite, 0.18, Qt::DashLine, Qt::RoundCap));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(55.6, 0.0), QPointF(55.6, 33.8));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(52.0, 60.8), QPointF(52.0, 100.0));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(byPassCenterLine().translated(0.0, 1.15));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(byPassCenterLine().translated(0.0, -1.15));

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(laneSoft, 0.14, Qt::DashLine, Qt::RoundCap));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawPath(chapelleBranchCenterLine().translated(-0.52, 0.0));

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(255, 255, 255, 190), 0.30, Qt::SolidLine, Qt::SquareCap));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(51.8, 40.4), QPointF(58.2, 40.4));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(50.5, 57.1), QPointF(57.4, 57.1));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(47.8, 46.4), QPointF(47.8, 53.1));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawLine(QPointF(62.2, 47.4), QPointF(62.2, 54.1));

    // Declare ou utilise une fonction necessaire au programme.
    drawCrosswalk(painter, QPointF(55.0, 37.2), true);
    // Declare ou utilise une fonction necessaire au programme.
    drawCrosswalk(painter, QPointF(54.0, 58.9), true);
    // Declare ou utilise une fonction necessaire au programme.
    drawCrosswalk(painter, QPointF(46.6, 49.4), false);
    // Declare ou utilise une fonction necessaire au programme.
    drawCrosswalk(painter, QPointF(64.4, 50.8), false);

    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(53.2, 9.8), QPointF(53.2, 29.2));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(56.8, 29.0), QPointF(56.8, 10.2));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(56.0, 90.0), QPointF(56.0, 71.0));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(52.0, 71.0), QPointF(52.0, 90.0));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(10.8, 51.9), QPointF(31.0, 51.9));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(31.2, 47.2), QPointF(11.0, 47.2));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(89.8, 48.0), QPointF(70.2, 48.0));
    // Declare ou utilise une fonction necessaire au programme.
    drawArrow(painter, QPointF(70.5, 52.2), QPointF(91.0, 52.2));
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawCentralIsland(QPainter& painter) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const QRectF outer = islandRect().adjusted(-0.78, -0.92, 0.78, 0.92);
    // Declare une constante ou une valeur partagee par le systeme.
    const QRectF inner = islandRect();

    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(0, 0, 0, 28));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(outer.translated(0.16, 0.24));

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient curb(outer.topLeft(), outer.bottomRight());
    // Execute cette instruction dans le flux normal du programme.
    curb.setColorAt(0.0, QColor(229, 234, 222, 210));
    // Execute cette instruction dans le flux normal du programme.
    curb.setColorAt(0.5, QColor(203, 210, 196, 220));
    // Execute cette instruction dans le flux normal du programme.
    curb.setColorAt(1.0, QColor(184, 191, 177, 220));
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(curb));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(outer);

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient grass(inner.topLeft(), inner.bottomRight());
    // Execute cette instruction dans le flux normal du programme.
    grass.setColorAt(0.0, QColor(70, 152, 95, 240));
    // Execute cette instruction dans le flux normal du programme.
    grass.setColorAt(0.55, QColor(48, 131, 80, 248));
    // Execute cette instruction dans le flux normal du programme.
    grass.setColorAt(1.0, QColor(29, 97, 61, 245));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(236, 246, 234, 70), 0.10));
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(grass));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(inner);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(148, 211, 116, 58));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QRectF(inner.center().x() - 0.95, inner.center().y() - 2.2, 1.9, 1.3));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QRectF(inner.center().x() - 1.45, inner.center().y() + 1.0, 1.2, 0.85));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QRectF(inner.center().x() + 0.35, inner.center().y() + 0.4, 1.5, 0.95));

    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(255, 255, 255, 28));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawEllipse(QRectF(inner.left() + 0.7, inner.top() + 0.75, inner.width() * 0.42, inner.height() * 0.26));
    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawSignals(QPainter& painter, const SimulationSnapshot& snapshot) const
// Ouvre un nouveau bloc d instructions.
{
    // Definit le comportement d une methode du simulateur.
    const auto lights = ngaba::lightState(snapshot.phase);

    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(53.2, 35.0), false, lights[ngaba::branchIndex(ngaba::Branch::North)].forward);
    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(56.8, 35.0), false, lights[ngaba::branchIndex(ngaba::Branch::North)].returnFlow);

    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(52.0, 60.8), false, lights[ngaba::branchIndex(ngaba::Branch::South)].returnFlow);
    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(56.0, 60.8), false, lights[ngaba::branchIndex(ngaba::Branch::South)].forward);

    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(45.0, 47.4), true, lights[ngaba::branchIndex(ngaba::Branch::West)].returnFlow);
    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(45.0, 51.8), true, lights[ngaba::branchIndex(ngaba::Branch::West)].forward);

    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(64.8, 48.0), true, lights[ngaba::branchIndex(ngaba::Branch::East)].forward);
    // Definit le comportement d une methode du simulateur.
    drawSignal(painter, QPointF(64.8, 52.2), true, lights[ngaba::branchIndex(ngaba::Branch::East)].returnFlow);
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawWaitingVehicles(QPainter& painter, const SimulationSnapshot& snapshot) const
// Ouvre un nouveau bloc d instructions.
{
    // Definit le comportement d une methode du simulateur.
    const ngaba::BranchArray queues = ngaba::branchQueues(snapshot.traffic);
    // Parcourt une suite de valeurs ou d elements.
    for (const ngaba::Branch branch : ngaba::allBranches()) {
        // Definit le comportement d une methode du simulateur.
        const int total = queues[ngaba::branchIndex(branch)];
        // Definit le comportement d une methode du simulateur.
        const ngaba::QueueVisualConfig config = ngaba::queueConfig(branch);
        // Definit le comportement d une methode du simulateur.
        const int visible = std::min(total, ngaba::kVisibleQueueMax);
        // Declare ou utilise une fonction necessaire au programme.
        const QPointF stepScene(config.step.x(), -config.step.y());
        // Declare ou utilise une fonction necessaire au programme.
        const QPointF travelVector(-stepScene.x(), -stepScene.y());
        // Declare une constante ou une valeur partagee par le systeme.
        const qreal angle = directionAngle(QPointF(0.0, 0.0), travelVector);

        // Parcourt une suite de valeurs ou d elements.
        for (int i = 0; i < visible; ++i) {
            // Declare une constante ou une valeur partagee par le systeme.
            const QPointF modelPosition = config.start + QPointF(config.step.x() * i, config.step.y() * i);
            // Declare une constante ou une valeur partagee par le systeme.
            const QPointF position = scenePoint(modelPosition);
            // Definit le comportement d une methode du simulateur.
            const QColor color = movementColor(ngaba::allMovements()[static_cast<std::size_t>((ngaba::branchIndex(branch) * 3 + i) % ngaba::kMovementCount)]);
            // Declare ou utilise une fonction necessaire au programme.
            drawVehicle(painter, position, angle, color, 3.2, 1.8);
        // Ferme le bloc d instructions courant.
        }
    // Ferme le bloc d instructions courant.
    }
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawMovingVehicles(QPainter& painter, const SimulationSnapshot& snapshot) const
// Ouvre un nouveau bloc d instructions.
{
    // Parcourt les vehicules persistants calcules par le moteur.
    for (const AnimatedVehicle& vehicle : snapshot.movingVehicles) {
        // Verifie une condition avant d executer ce bloc.
        if (vehicle.progress <= 0.0) {
            // Passe a l iteration suivante de la boucle.
            continue;
        // Ferme le bloc d instructions courant.
        }

        // Adoucit la progression pour limiter l'impression de mouvement mecanique.
        const qreal clamped = std::clamp(static_cast<qreal>(vehicle.progress), 0.0, 1.0);
        const qreal eased = clamped < 0.5
            ? 2.0 * clamped * clamped
            : 1.0 - std::pow(-2.0 * clamped + 2.0, 2.0) / 2.0;

        // Initialise ou met a jour une valeur du programme.
        qreal angle = 0.0;
        // Definit le comportement d une methode du simulateur.
        QPointF position = pointOnRoute(vehicle.movement, std::clamp(eased, 0.03, 0.97), &angle);
        // Initialise ou met a jour une valeur du programme.
        position = clampAwayFromIsland(position);
        // Declare ou utilise une fonction necessaire au programme.
        drawVehicle(painter, position, angle, movementColor(vehicle.movement), 3.2, 1.8);
    // Ferme le bloc d instructions courant.
    }
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawCrosswalk(QPainter& painter, const QPointF& center, bool horizontal) const
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);

    // Parcourt une suite de valeurs ou d elements.
    for (int i = -4; i <= 4; ++i) {
        // Execute cette instruction dans le flux normal du programme.
        QRectF stripe;
        // Verifie une condition avant d executer ce bloc.
        if (horizontal) {
            // Initialise ou met a jour une valeur du programme.
            stripe = QRectF(center.x() - 3.25 + i * 0.75, center.y() - 0.94, 0.36, 1.88);
        // Execute cette instruction dans le flux normal du programme.
        } else {
            // Initialise ou met a jour une valeur du programme.
            stripe = QRectF(center.x() - 0.94, center.y() - 3.25 + i * 0.75, 1.88, 0.36);
        // Ferme le bloc d instructions courant.
        }

        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(0, 0, 0, 22));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawRoundedRect(stripe.translated(0.05, 0.08), 0.04, 0.04);
        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(251, 253, 255, 205));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawRoundedRect(stripe, 0.04, 0.04);
    // Ferme le bloc d instructions courant.
    }

    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawSignal(QPainter& painter, const QPointF& center, bool vertical, ngaba::LightColor color) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const QSizeF bodySize = vertical ? QSizeF(2.2, 5.5) : QSizeF(5.5, 2.2);
    // Declare une constante ou une valeur partagee par le systeme.
    const QRectF body(center.x() - bodySize.width() / 2.0, center.y() - bodySize.height() / 2.0,
        // Execute cette instruction dans le flux normal du programme.
        bodySize.width(), bodySize.height());

    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);

    // Verifie une condition avant d executer ce bloc.
    if (vertical) {
        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(42, 48, 54, 145));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawRoundedRect(QRectF(center.x() - 0.16, center.y() - 4.0, 0.32, 1.55), 0.08, 0.08);
    // Execute cette instruction dans le flux normal du programme.
    } else {
        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(42, 48, 54, 145));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawRoundedRect(QRectF(center.x() + 2.05, center.y() - 0.16, 1.20, 0.32), 0.08, 0.08);
    // Ferme le bloc d instructions courant.
    }

    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(0, 0, 0, 40));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(body.translated(0.12, 0.18), 0.50, 0.50);

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient housing(body.topLeft(), body.bottomLeft());
    // Execute cette instruction dans le flux normal du programme.
    housing.setColorAt(0.0, QColor(56, 61, 68, 240));
    // Execute cette instruction dans le flux normal du programme.
    housing.setColorAt(1.0, QColor(26, 30, 36, 240));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(218, 224, 232, 110), 0.08));
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(housing));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(body, 0.50, 0.50);

    // Declare une constante ou une valeur partagee par le systeme.
    const std::array<ngaba::LightColor, 3> order{{ngaba::LightColor::Red, ngaba::LightColor::Orange, ngaba::LightColor::Green}};
    // Parcourt une suite de valeurs ou d elements.
    for (int i = 0; i < 3; ++i) {
        // Initialise ou met a jour une valeur du programme.
        QPointF lampCenter = center;
        // Verifie une condition avant d executer ce bloc.
        if (vertical) {
            // Initialise ou met a jour une valeur du programme.
            lampCenter.ry() += 1.45 - static_cast<qreal>(i) * 1.45;
        // Execute cette instruction dans le flux normal du programme.
        } else {
            // Initialise ou met a jour une valeur du programme.
            lampCenter.rx() += static_cast<qreal>(i) * 1.45 - 1.45;
        // Ferme le bloc d instructions courant.
        }

        // Definit le comportement d une methode du simulateur.
        const ngaba::LightColor lamp = order[static_cast<std::size_t>(i)];
        // Declare une constante ou une valeur partagee par le systeme.
        const bool active = lamp == color;
        // Declare une constante ou une valeur partagee par le systeme.
        const QColor activeColor = lightToColor(lamp);

        // Verifie une condition avant d executer ce bloc.
        if (active) {
            // Declare ou utilise une fonction necessaire au programme.
            QRadialGradient glow(lampCenter, 0.95);
            // Execute cette instruction dans le flux normal du programme.
            glow.setColorAt(0.0, QColor(activeColor.red(), activeColor.green(), activeColor.blue(), 170));
            // Execute cette instruction dans le flux normal du programme.
            glow.setColorAt(0.55, QColor(activeColor.red(), activeColor.green(), activeColor.blue(), 50));
            // Execute cette instruction dans le flux normal du programme.
            glow.setColorAt(1.0, QColor(activeColor.red(), activeColor.green(), activeColor.blue(), 0));
            // Execute cette instruction dans le flux normal du programme.
            painter.setPen(Qt::NoPen);
            // Execute cette instruction dans le flux normal du programme.
            painter.setBrush(QBrush(glow));
            // Execute cette instruction dans le flux normal du programme.
            painter.drawEllipse(lampCenter, 0.95, 0.95);
        // Ferme le bloc d instructions courant.
        }

        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(18, 22, 27, 225));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawEllipse(lampCenter, 0.49, 0.49);
        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(active ? activeColor : QColor(86, 91, 96, 235));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawEllipse(lampCenter, 0.34, 0.34);
        // Execute cette instruction dans le flux normal du programme.
        painter.setBrush(QColor(255, 255, 255, active ? 85 : 28));
        // Execute cette instruction dans le flux normal du programme.
        painter.drawEllipse(QPointF(lampCenter.x() - 0.08, lampCenter.y() - 0.09), 0.09, 0.09);
    // Ferme le bloc d instructions courant.
    }

    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawVehicle(QPainter& painter, const QPointF& center, qreal angleDeg, const QColor& bodyColor, qreal length, qreal width) const
// Ouvre un nouveau bloc d instructions.
{
    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.translate(center);
    // Execute cette instruction dans le flux normal du programme.
    painter.rotate(angleDeg);

    // Declare ou utilise une fonction necessaire au programme.
    const QRectF bodyRect(-length / 2.0, -width / 2.0, length, width);

    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(0, 0, 0, 34));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(bodyRect.translated(0.18, 0.20), 0.46, 0.46);

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient bodyGradient(bodyRect.topLeft(), bodyRect.bottomRight());
    // Execute cette instruction dans le flux normal du programme.
    bodyGradient.setColorAt(0.0, bodyColor.lighter(126));
    // Execute cette instruction dans le flux normal du programme.
    bodyGradient.setColorAt(0.45, bodyColor);
    // Execute cette instruction dans le flux normal du programme.
    bodyGradient.setColorAt(1.0, bodyColor.darker(126));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(QPen(QColor(14, 18, 24, 80), 0.08));
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(bodyGradient));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(bodyRect, 0.48, 0.48);

    // Declare ou utilise une fonction necessaire au programme.
    const QRectF glassRect(-length * 0.19, -width * 0.30, length * 0.38, width * 0.60);
    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient glass(glassRect.topLeft(), glassRect.bottomLeft());
    // Execute cette instruction dans le flux normal du programme.
    glass.setColorAt(0.0, QColor(228, 242, 252, 225));
    // Execute cette instruction dans le flux normal du programme.
    glass.setColorAt(1.0, QColor(138, 176, 198, 215));
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QBrush(glass));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(glassRect, 0.16, 0.16);

    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(255, 255, 255, 65));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(-length * 0.30, -width * 0.33, length * 0.54, width * 0.12), 0.06, 0.06);

    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(255, 244, 200, 170));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(length / 2.0 - 0.18, -width * 0.22, 0.11, width * 0.44), 0.04, 0.04);
    // Execute cette instruction dans le flux normal du programme.
    painter.setBrush(QColor(255, 96, 96, 160));
    // Execute cette instruction dans le flux normal du programme.
    painter.drawRoundedRect(QRectF(-length / 2.0 + 0.07, -width * 0.22, 0.11, width * 0.44), 0.04, 0.04);

    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}

// Definit le comportement d une methode du simulateur.
void IntersectionWidget::drawArrow(QPainter& painter, const QPointF& start, const QPointF& end) const
// Ouvre un nouveau bloc d instructions.
{
    // Declare une constante ou une valeur partagee par le systeme.
    const QPainterPath arrow = arrowShape(start, end, 0.22, 1.05, 0.96);
    // Verifie une condition avant d executer ce bloc.
    if (arrow.isEmpty()) {
        // Retourne la valeur calculee par la fonction.
        return;
    // Ferme le bloc d instructions courant.
    }

    // Execute cette instruction dans le flux normal du programme.
    painter.save();
    // Execute cette instruction dans le flux normal du programme.
    painter.setPen(Qt::NoPen);
    // Execute cette instruction dans le flux normal du programme.
    painter.fillPath(arrow.translated(0.08, 0.10), QColor(0, 0, 0, 18));

    // Declare ou utilise une fonction necessaire au programme.
    QLinearGradient fill(start, end);
    // Execute cette instruction dans le flux normal du programme.
    fill.setColorAt(0.0, QColor(255, 255, 255, 74));
    // Execute cette instruction dans le flux normal du programme.
    fill.setColorAt(1.0, QColor(255, 255, 255, 138));
    // Execute cette instruction dans le flux normal du programme.
    painter.fillPath(arrow, QBrush(fill));
    // Execute cette instruction dans le flux normal du programme.
    painter.restore();
// Ferme le bloc d instructions courant.
}
