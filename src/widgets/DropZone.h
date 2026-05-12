#pragma once

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QStringList>
#include <QPainter>
#include <QPixmap>

class DropZone : public QWidget
{
    Q_OBJECT

public:
    explicit DropZone(QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(320, 80); }

signals:
    void filesDropped(const QStringList &paths);

protected:
    void paintEvent(QPaintEvent *) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dropEvent(QDropEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    bool m_hovered = false;
    QStringList m_droppedFiles;
    QPixmap m_preview;
};