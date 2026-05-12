#include "LeafIcon.h"

#include <QLinearGradient>
#include <QRadialGradient>
#include <QPainterPath>
#include <QPen>

LeafIcon::LeafIcon(QWidget *parent) : QWidget(parent)
{
    setFixedSize(40, 40);
}

void LeafIcon::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r(2, 2, 36, 36);

    QRadialGradient glow(r.center(), 20);
    glow.setColorAt(0, QColor(100, 200, 140, 60));
    glow.setColorAt(1, Qt::transparent);

    p.fillRect(rect(), glow);

    QPainterPath leaf;

    leaf.moveTo(20, 36);
    leaf.cubicTo(5, 28, 4, 10, 20, 4);
    leaf.cubicTo(36, 10, 35, 28, 20, 36);

    QLinearGradient leafGrad(20, 4, 20, 36);
    leafGrad.setColorAt(0.0, QColor(126, 216, 154));
    leafGrad.setColorAt(1.0, QColor(53, 168, 90));

    p.fillPath(leaf, leafGrad);

    QLinearGradient gloss(8, 6, 26, 20);
    gloss.setColorAt(0, QColor(255, 255, 255, 145));
    gloss.setColorAt(0.5, QColor(255, 255, 255, 30));
    gloss.setColorAt(1, Qt::transparent);

    p.fillPath(leaf, gloss);

    p.setPen(QPen(QColor(255, 255, 255, 130),
                  0.9,
                  Qt::SolidLine,
                  Qt::RoundCap));

    p.drawLine(QPointF(20, 8), QPointF(20, 33));

    p.setPen(QPen(QColor(255, 255, 255, 80),
                  0.7,
                  Qt::SolidLine,
                  Qt::RoundCap));

    p.drawLine(QPointF(20, 13), QPointF(27, 17));
    p.drawLine(QPointF(20, 18), QPointF(26, 22));
    p.drawLine(QPointF(20, 23), QPointF(25, 27));

    p.drawLine(QPointF(20, 13), QPointF(13, 17));
    p.drawLine(QPointF(20, 18), QPointF(14, 22));
    p.drawLine(QPointF(20, 23), QPointF(15, 27));
}