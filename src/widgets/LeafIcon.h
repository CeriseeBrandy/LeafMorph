#pragma once

#include <QWidget>
#include <QPainter>

class LeafIcon : public QWidget
{
    Q_OBJECT

public:
    explicit LeafIcon(QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(40, 40); }

protected:
    void paintEvent(QPaintEvent *) override;
};