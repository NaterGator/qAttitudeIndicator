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
    Q_PROPERTY(bool  supersample READ getSuperSample WRITE setSuperSample NOTIFY superSampleChanged)
public:
    AttitudeIndicator(QWidget *parent = 0);

    qreal getRoll()  const {return roll;}
    qreal getPitch() const {return pitch;}
    qreal getYaw()   const {return yaw;}
    bool  getSuperSample() const {return ss;}
    qreal ssfactor() const {return ss ? 2.0 : 1.0;}

public slots:
    void setRoll(qreal val);
    void setPitch(qreal val);
    void setYaw(qreal val);
    void setSuperSample(bool b) { ss = b; emit superSampleChanged(b);}
    void invalidateCache();

signals:
    void rollChanged(qreal);
    void pitchChanged(qreal);
    void yawChanged(qreal);
    void superSampleChanged(bool);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

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
    bool    ss; // 2x supersample
};

#endif // ATTITUDEINDICATOR_H
