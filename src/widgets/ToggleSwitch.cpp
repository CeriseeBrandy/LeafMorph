#include "ToggleSwitch.h"

#include <QLinearGradient>
#include <QRadialGradient>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QEasingCurve>

ToggleSwitch::ToggleSwitch(QWidget *parent) : QWidget(parent)
{
    setFixedSize(38, 22);
    setCursor(Qt::PointingHandCursor);

    m_anim = new QPropertyAnimation(this, "thumbPos", this);
    m_anim->setDuration(220);
    m_anim->setEasingCurve(QEasingCurve::OutBack);
}

void ToggleSwitch::setChecked(bool checked)
{
    if (m_checked == checked) return;

    m_checked = checked;

    m_anim->stop();
    m_anim->setStartValue(m_thumbPos);
    m_anim->setEndValue(checked ? 18.0f : 2.0f);
    m_anim->start();

    emit toggled(checked);
}

void ToggleSwitch::setThumbPos(float pos)
{
    m_thumbPos = pos;
    update();
}

void ToggleSwitch::mousePressEvent(QMouseEvent *)
{
    setChecked(!m_checked);
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF trackRect(0, 1, 38, 20);

    QPainterPath trackPath;
    trackPath.addRoundedRect(trackRect, 10, 10);

    if (m_checked) {
        QLinearGradient grad(trackRect.topLeft(), trackRect.topRight());

        float t = (m_thumbPos - 2.0f) / 16.0f;
        t = qBound(0.0f, t, 1.0f);

        QColor c1 = QColor(98, 204, 130).lighter(int(100 + t * 5));
        QColor c2 = QColor(61, 176, 96).lighter(int(100 + t * 3));

        grad.setColorAt(0, c1);
        grad.setColorAt(1, c2);

        p.fillPath(trackPath, grad);

        QLinearGradient gloss(trackRect.topLeft(), QPointF(trackRect.left(), trackRect.top() + 8));
        gloss.setColorAt(0, QColor(255, 255, 255, 50));
        gloss.setColorAt(1, Qt::transparent);
        p.fillPath(trackPath, gloss);
    } else {
        QColor trackColor(155, 210, 175, 110);
        p.fillPath(trackPath, trackColor);

        p.setPen(QPen(QColor(120, 190, 145, 80), 1));
        p.drawPath(trackPath);
    }

    QRectF thumb(m_thumbPos, 4.0f, 14.0f, 14.0f);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 30));
    p.drawEllipse(thumb.adjusted(1, 1, 1, 1));

    QRadialGradient thumbGradient(thumb.center() + QPointF(-2, -2), 8);
    thumbGradient.setColorAt(0, QColor(255, 255, 255));
    thumbGradient.setColorAt(0.6, QColor(245, 245, 245));
    thumbGradient.setColorAt(1, QColor(225, 225, 225));

    p.setBrush(thumbGradient);
    p.drawEllipse(thumb);
}