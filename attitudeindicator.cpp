#include <QtGui>
#include <QDebug>
#include "attitudeindicator.h"


const int sizeMax = 600,
          sizeMin = 200;

enum LineType {
    Strong,
    Weak
};

typedef QPair<qreal, LineType> LineMark;

const QList<LineMark> rollMarks =
      QList<LineMark>() << LineMark(-90, Strong)
                        << LineMark(-60, Strong)
                        << LineMark(-45, Weak)
                        << LineMark(-30, Strong)
                        << LineMark(-20, Weak)
                        << LineMark(-10, Weak)
                        << LineMark(0,   Strong)
                        << LineMark(10,  Weak)
                        << LineMark(20,  Weak)
                        << LineMark(30,  Strong)
                        << LineMark(45,  Weak)
                        << LineMark(60,  Strong)
                        << LineMark(90,  Strong);

qreal constrainInRange(qreal val, qreal min, qreal max)
{
    if (qFuzzyCompare(val, min) || qFuzzyCompare(val, max))
        return val;
    qreal constrained = fmod(val-min, max-min);
    return constrained > 0 ? constrained + min : constrained + max;
}
int constrainInRange(int val, int min, int max)
{
    if (val == min || val == max)
        return val;
    int constrained = (val-min)%(max-min);
    return constrained > 0 ? constrained + min : constrained + max;
}

AttitudeIndicator::AttitudeIndicator(QWidget *parent)
    : QWidget(parent), fov(40), cache_valid(false), yaw(0), ss(true)
{
    msize = sizeMin;
    setMinimumSize(sizeMin,sizeMin);
    setMaximumSize(sizeMax,sizeMax);
    resize(msize, msize);

    connect(this, SIGNAL(pitchChanged(qreal)),      SLOT(invalidateCache()));
    connect(this, SIGNAL(rollChanged(qreal)),       SLOT(invalidateCache()));
    connect(this, SIGNAL(yawChanged(qreal)),        SLOT(invalidateCache()));
    connect(this, SIGNAL(superSampleChanged(bool)), SLOT(invalidateCache()));

    initTargetChar();
    initRollChar();
    roll = 0.0;
    pitch = 0.0;
}

void AttitudeIndicator::invalidateCache()
{
    cache_valid = false;
    update();
}

void AttitudeIndicator::initTargetChar()
{
    target = QPainterPath();
    target.moveTo(-1/4.0,0);
    target.lineTo(-1/16.0,0);
    target.lineTo(0,-1/32.0);
    target.lineTo(1/16.0,0);
    target.lineTo(1/4.0,0);
}

void AttitudeIndicator::initRollChar()
{
    rollPointer = QPainterPath();
    rollPointer.moveTo(-1/32.0,14/32.0);
    rollPointer.lineTo(0,15/32.0);
    rollPointer.lineTo(1/32.0,14/32.0);
    rollPointer.lineTo(-1/32.0,14/32.0);
}

void AttitudeIndicator::setRoll(qreal val)
{
    roll = constrainInRange(val, -180., 180.);
    emit rollChanged(roll);
}

void AttitudeIndicator::setPitch(qreal val)
{
    pitch = qBound(-90., val, 90.);
    emit pitchChanged(pitch);
}

void AttitudeIndicator::setYaw(qreal val)
{
    yaw = constrainInRange(val, 0., 360.);
    emit yawChanged(yaw);
}

void AttitudeIndicator::resizeEvent(QResizeEvent *event)
{
    invalidateCache();
}

void AttitudeIndicator::paintEvent(QPaintEvent *)
{
    repaintCache();

    QPainter blitter(this);
    blitter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    blitter.drawPixmap(rect(), cache);
}

void AttitudeIndicator::repaintCache()
{
    if (cache_valid)
        return;

    msize = qMin(width(),height());
    pensize = msize;
    msize *= ssfactor();
    cache = QPixmap(size()*ssfactor());
    cache.fill(Qt::transparent);
    QPainter painter(&cache);
    QPen whitePen(Qt::white);
    QPen blackPen(Qt::black);


    whitePen.setWidthF(2/pensize);
    blackPen.setWidthF(1/pensize);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(painter.device()->width()  / 2.,
                      painter.device()->height() / 2.);
    painter.scale(msize, msize);

    QFont ar("Arial");
    ar.setPointSizeF(qMax(8.0, qMin(32.0, 32*pensize/1000))*ssfactor());
    painter.setFont(ar);
    painter.setPen(Qt::NoPen);

    renderHorizonBackground(&painter);
    renderPitchIndicators(&painter);
    renderHeadingIndicators(&painter);
    renderOverlay(&painter);

}

void AttitudeIndicator::setupPainterForTextOverlay(QPainter *painter)
{
    painter->scale(1/msize, 1/msize);
    painter->translate(painter->device()->width()  / -2.0,
                       painter->device()->height() / -2.0);
    painter->setPen(QPen(Qt::white, 2*ssfactor()));
    painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
}

void AttitudeIndicator::renderHorizonBackground(QPainter *painter)
{
    QBrush bgSky(QColor(48,172,220));
    QBrush bgGround(QColor(247,168,21));

    const qreal cpitch = qBound(-fov, pitch, fov);  // Just ground/sky beyond these limits
    const qreal y = 0.5*cpitch/fov;
    const qreal x = sqrt(1./4. - y*y);
    const qreal gr = atan(y/x) * 180./M_PI;

    painter->save();
    painter->rotate(roll);

    painter->setBrush(bgSky);
    painter->drawChord(QRectF(-0.5,-0.5,1,1),gr*16., (180.-2.*gr)*16.);
    painter->setBrush(bgGround);
    painter->drawChord(QRectF(-0.5,-0.5,1,1),gr*16.,-(180.+2.*gr)*16.);
    painter->setPen(QPen(Qt::white, 2/pensize));
    const QLineF horizon(-x,-y,x,-y);
    if (!horizon.isNull())
        painter->drawLine(horizon);
    painter->restore();
}

void AttitudeIndicator::renderPitchIndicators(QPainter *painter)
{
    QTransform mapper(painter->transform());
    const int margin = 5*ssfactor();
    painter->save();
    painter->rotate(roll);
    setupPainterForTextOverlay(painter);

    int pitchtop = qMin(90, static_cast<int>((pitch+fov)/5.0+1)*5);
    int pitchbottom = qMax(-90, static_cast<int>((pitch-fov)/5.0-1)*5);
    for (int i = pitchbottom; i <= pitchtop; i += 5) {
        if (i == 0) continue;
        bool strong = (i % 10) == 0;
        if (!strong && pensize < 256) continue;
        const qreal y = 0.5*(i-pitch)/fov;
        const qreal x =  strong ? (5/32.0) : (3/32.0);
        const QLineF renderline = mapper.map(QLineF(QPointF(-x, y), QPointF(x, y)));
        painter->drawLine(renderline);
        if (strong) {
            const QString str(QString::number(qAbs(i)));
            QRectF br = painter->boundingRect(QRectF(), Qt::AlignCenter, str);
            br.moveCenter(QPointF(0, renderline.y1()));
            br.moveRight(renderline.x1()-margin);
            painter->drawText(br, str);
            br.moveCenter(QPointF(0, renderline.y2()));
            br.moveLeft(renderline.x2()+margin);
            painter->drawText(br, str);
        }
    }
    painter->restore();
}

void AttitudeIndicator::renderHeadingIndicators(QPainter *painter)
{
    const int dir = (pitch > 0) ? -1 : 1;
    const int scale2 = 2*ssfactor();
    QTransform mapper(painter->transform());
    const qreal cpitch = qBound(-fov, pitch, fov);
    const qreal y = 0.5*cpitch/fov;

    painter->save();
    painter->rotate(roll);
    setupPainterForTextOverlay(painter);

    painter->setPen(QPen(Qt::green, scale2));
    int horizonleft = static_cast<int>((yaw-fov*0.5)/5.0)*5;
    int horizonright = static_cast<int>((yaw+fov*0.5)/5.0+1.0)*5;
    for(int i = horizonleft; i <= horizonright; i += 5)
    {
        bool strong = (i % 10) == 0;
        const qreal yoffs = (strong ? 1/32.0 : 1/64.0)*dir;
        const qreal x = (i-yaw)/fov;
        const QLineF renderline = mapper.map(QLineF(QPointF(x, -y), QPointF(x, -y-yoffs)));
        painter->drawLine(renderline);
        if (strong) {
            const QString str(QString::number(constrainInRange(i, 0, 360)));
            QRectF br = painter->boundingRect(QRectF(), Qt::AlignCenter, str);
            br.moveCenter(renderline.p2());
            if (dir > 0)
                br.moveBottom(renderline.y2()-scale2);
            else
                br.moveTop(renderline.y2()+scale2);
            painter->drawText(br, str);
        }
    }

    painter->restore();
}

void AttitudeIndicator::renderOverlay(QPainter *painter)
{
    painter->save();
    painter->rotate(180);
    painter->setPen(QPen(Qt::black, 2/pensize));
    painter->setBrush(Qt::darkGreen);
    painter->drawPath(rollPointer);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(Qt::black, 4/pensize));
    painter->drawPath(target);
    painter->rotate(roll);
    painter->setPen(QPen(Qt::black, 2/pensize));
    painter->drawEllipse(QPointF(0,0), 0.5, 0.5);
    QPointF base(0, 1/2.0);
    foreach(const LineMark &mark, rollMarks) {
        painter->rotate(-mark.first);
        painter->drawLine(base, QPointF(0, base.y() - 1.0/(mark.second == Strong ? 20 : 40)));
        painter->rotate(mark.first);
    }
    painter->restore();
}
