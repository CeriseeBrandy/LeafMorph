#include "GlassCard.h"

#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>

GlassCard::GlassCard(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void GlassCard::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(8, 8, -8, -8);

    for (int i = 6; i >= 0; --i) {
        float alpha = 8.0f + i * 3.0f;
        float offset = i * 3.0f;

        QPainterPath shadow;
        shadow.addRoundedRect(r.adjusted(-1, offset * 0.3f, 1, offset), 22, 22);

        p.fillPath(shadow, QColor(60, 140, 90, int(alpha)));
    }

    QPainterPath card;
    card.addRoundedRect(r, 22, 22);

    QLinearGradient body(r.topLeft(), r.bottomLeft());
    body.setColorAt(0.0, QColor(252, 255, 253, 210));
    body.setColorAt(0.4, QColor(238, 252, 244, 196));
    body.setColorAt(1.0, QColor(228, 248, 238, 190));

    p.fillPath(card, body);

    QRadialGradient ceil(r.center().x(), r.top() - 10, r.width() * 0.7f);
    ceil.setColorAt(0, QColor(200, 245, 220, 70));
    ceil.setColorAt(1, Qt::transparent);

    p.fillPath(card, ceil);

    QLinearGradient gloss(r.topLeft(),
                          QPointF(r.left(), r.top() + r.height() * 0.18f));

    gloss.setColorAt(0, QColor(255, 255, 255, 160));
    gloss.setColorAt(1, Qt::transparent);

    p.fillPath(card, gloss);

    p.setPen(QPen(QColor(255, 255, 255, 200), 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawPath(card);

    QPainterPath innerBorder;
    innerBorder.addRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5),
                               21.5,
                               21.5);

    p.setPen(QPen(QColor(120, 200, 150, 55), 0.5));
    p.drawPath(innerBorder);

    QRadialGradient blob1(r.right() - 30, r.top() + 30, 80);
    blob1.setColorAt(0, QColor(150, 225, 175, 38));
    blob1.setColorAt(1, Qt::transparent);

    p.fillPath(card, blob1);

    QRadialGradient blob2(r.left() + 20, r.bottom() - 20, 60);
    blob2.setColorAt(0, QColor(130, 210, 160, 30));
    blob2.setColorAt(1, Qt::transparent);

    p.fillPath(card, blob2);
}