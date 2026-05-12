#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPainter>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float thumbPos READ thumbPos WRITE setThumbPos)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);

    bool isChecked() const { return m_checked; }
    void setChecked(bool checked);

    float thumbPos() const { return m_thumbPos; }
    void setThumbPos(float pos);

    QSize sizeHint() const override { return QSize(38, 22); }
    QSize minimumSizeHint() const override { return QSize(38, 22); }

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    bool m_checked = false;
    float m_thumbPos = 2.0f;
    QPropertyAnimation *m_anim;
};