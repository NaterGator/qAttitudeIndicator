#ifndef ATTITUDEINDICATOR_H
#define ATTITUDEINDICATOR_H

#include <QWidget>


typedef enum _en_global_definitions_
{
    sizeMax = 600,
    sizeMin = 200,
    numbRollLine = 13,
    numbPitchLine =8
} EN_GLOABL_DEFINITIONS;

typedef enum _en_types_attitude_
{
    smallRollLine = 0,
    normalRollLine,
    smallPitchLine,
    normalPitchLine
} EN_TYPES_ATTITUDE;

class AttitudeIndicator : public QWidget
{
    Q_OBJECT

public:
    AttitudeIndicator(QWidget *parent = 0);
    ~AttitudeIndicator();
    void setRoll(qreal val) {roll  = val;}
    void setPitch(qreal val){pitch = val;}
    qreal getRoll() {return roll;}
    qreal getPitch(){return pitch;}
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void initTargetChar();
    void initRollChar();

    // Painter helper
    void setupPainterForTextOverlay(QPainter *painter);

    // Rendering steps:
    void renderHorizonBackground(QPainter *painter);
    void renderPitchIndicators(QPainter *painter);
    void renderOverlay(QPainter *painter);
    qreal size;
    qreal pensize;
    qreal roll;
    qreal pitch;
    qreal fov;
    QPainterPath target;
    QPainterPath rollPointer;
};

#endif // ATTITUDEINDICATOR_H
