#pragma once

#include <QWidget>
#include <QPainter>

class GlassCard : public QWidget
{
    Q_OBJECT

public:
    explicit GlassCard(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
};