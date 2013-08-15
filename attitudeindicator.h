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
    Q_PROPERTY(qreal roll        READ getRoll        WRITE setRoll        NOTIFY rollChanged)
    Q_PROPERTY(qreal pitch       READ getPitch       WRITE setPitch       NOTIFY pitchChanged)
    Q_PROPERTY(qreal yaw         READ getYaw         WRITE setYaw         NOTIFY yawChanged)

public:
    AttitudeIndicator(QWidget *parent = 0);
    ~AttitudeIndicator();
    qreal getRoll()  const {return roll;}
    qreal getPitch() const {return pitch;}
    qreal getYaw()   const {return yaw;}

public slots:
    void setRoll(qreal val) {roll  = val;}
    void setPitch(qreal val){pitch = val;}
    void setYaw(qreal val)  {yaw   = val;}
    void invalidateCache();

signals:
    void rollChanged(qreal);
    void pitchChanged(qreal);
    void yawChanged(qreal);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void initTargetChar();
    void initRollChar();

    // Painter helper
    void repaintCache();
    void setupPainterForTextOverlay(QPainter *painter);

    // Rendering steps:
    void renderHorizonBackground(QPainter *painter);
    void renderPitchIndicators(QPainter *painter);
    void renderHeadingIndicators(QPainter *painter);
    void renderOverlay(QPainter *painter);
    qreal msize;
    qreal pensize;
    qreal roll;
    qreal pitch;
    qreal yaw;
    qreal fov;
    QPainterPath target;
    QPainterPath rollPointer;
    QPixmap cache;
    bool    cache_valid;
};

#endif // ATTITUDEINDICATOR_H
