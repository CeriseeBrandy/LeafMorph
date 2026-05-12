#include "DropZone.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPainterPath>
#include <QLinearGradient>
#include <QPen>
#include <QPolygonF>
#include <QFontMetrics>
#include <QPixmap>

DropZone::DropZone(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(82);
}

void DropZone::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(1, 1, -1, -1);

    QLinearGradient bg(r.topLeft(), r.bottomLeft());

    if (m_hovered) {
        bg.setColorAt(0, QColor(200, 245, 220, 180));
        bg.setColorAt(1, QColor(180, 238, 208, 155));
    } else {
        bg.setColorAt(0, QColor(255, 255, 255, 145));
        bg.setColorAt(1, QColor(240, 252, 246, 118));
    }

    QPainterPath path;
    path.addRoundedRect(r, 14, 14);

    p.fillPath(path, bg);

    QLinearGradient gloss(r.topLeft(), QPointF(r.left(), r.top() + r.height() * 0.45));
    gloss.setColorAt(0, QColor(255, 255, 255, 70));
    gloss.setColorAt(1, Qt::transparent);

    p.fillPath(path, gloss);

    QPen borderPen = m_hovered
        ? QPen(QColor(70, 170, 110, 170), 1.5, Qt::DashLine)
        : QPen(QColor(100, 185, 130, 105), 1.5, Qt::DashLine);

    borderPen.setDashPattern({5, 4});

    p.setPen(borderPen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    QFont mainFont = p.font();
    mainFont.setPointSize(10);
    mainFont.setWeight(QFont::Medium);
    p.setFont(mainFont);

    if (!m_droppedFiles.isEmpty()) {
        QString name = QFileInfo(m_droppedFiles.first()).fileName();

        if (m_droppedFiles.size() > 1)
            name += QString(" +%1 more").arg(m_droppedFiles.size() - 1);

        QFontMetrics fm(mainFont);

        if (!m_preview.isNull()) {
            QRectF previewRect(r.left() + 18, r.top() + 18, 46, 46);

            QPainterPath previewPath;
            previewPath.addRoundedRect(previewRect, 10, 10);

            p.save();
            p.setClipPath(previewPath);
            p.drawPixmap(
                previewRect.toRect(),
                m_preview.scaled(
                    previewRect.size().toSize(),
                    Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation
                )
            );
            p.restore();

            p.setPen(QPen(QColor(255, 255, 255, 140), 1));
            p.drawPath(previewPath);

            name = fm.elidedText(name, Qt::ElideMiddle, int(r.width() - 100));

            p.setPen(QColor(50, 140, 80));
            p.drawText(
                QRectF(r.left() + 78, r.top() + 26, r.width() - 98, 20),
                Qt::AlignVCenter | Qt::AlignLeft,
                name
            );

            mainFont.setPointSize(8);
            mainFont.setWeight(QFont::Normal);
            p.setFont(mainFont);
            p.setPen(QColor(60, 130, 80, 120));

            p.drawText(
                QRectF(r.left() + 78, r.top() + 46, r.width() - 98, 16),
                Qt::AlignVCenter | Qt::AlignLeft,
                "Click to change"
            );
        } else {
            name = fm.elidedText(name, Qt::ElideMiddle, int(r.width() - 40));

            p.setPen(QColor(50, 140, 80));
            p.drawText(
                QRectF(r.left(), r.top() + 38, r.width(), 18),
                Qt::AlignHCenter,
                name
            );

            mainFont.setPointSize(8);
            mainFont.setWeight(QFont::Normal);
            p.setFont(mainFont);
            p.setPen(QColor(60, 130, 80, 120));

            p.drawText(
                QRectF(r.left(), r.top() + 56, r.width(), 16),
                Qt::AlignHCenter,
                "Click to change"
            );
        }

        return;
    }

    p.setPen(QPen(m_hovered ? QColor(60, 160, 100)
                            : QColor(88, 184, 118), 1.8));

    QPointF arrowTip(r.center().x(), r.top() + 18);
    QPointF arrowBase(r.center().x(), r.top() + 30);

    p.drawLine(arrowTip, arrowBase);

    QPolygonF head;
    head << QPointF(r.center().x() - 6, r.top() + 24)
         << QPointF(r.center().x(), r.top() + 17)
         << QPointF(r.center().x() + 6, r.top() + 24);

    p.drawPolyline(head);

    p.drawLine(QPointF(r.center().x() - 9, r.top() + 30),
               QPointF(r.center().x() + 9, r.top() + 30));

    p.setPen(QColor(50, 120, 72));

    p.drawText(
        QRectF(r.left(), r.top() + 38, r.width(), 18),
        Qt::AlignHCenter,
        "Drop files here"
    );

    mainFont.setPointSize(8);
    mainFont.setWeight(QFont::Normal);
    p.setFont(mainFont);
    p.setPen(QColor(60, 130, 80, 120));

    p.drawText(
        QRectF(r.left(), r.top() + 56, r.width(), 16),
        Qt::AlignHCenter,
        "or click to browse"
    );
}

void DropZone::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
        m_hovered = true;
        update();
    }
}

void DropZone::dragLeaveEvent(QDragLeaveEvent *)
{
    m_hovered = false;
    update();
}

void DropZone::dropEvent(QDropEvent *e)
{
    m_hovered = false;

    QStringList paths;

    for (const QUrl &url : e->mimeData()->urls()) {
        if (url.isLocalFile())
            paths << url.toLocalFile();
    }

    if (!paths.isEmpty()) {
        m_droppedFiles = paths;
        m_preview = QPixmap(paths.first());
        emit filesDropped(paths);
    }

    update();
}

void DropZone::mousePressEvent(QMouseEvent *)
{
    QStringList paths = QFileDialog::getOpenFileNames(
        this,
        "Select files",
        QDir::homePath(),
        "All Files (*.*)"
    );

    if (!paths.isEmpty()) {
        m_droppedFiles = paths;
        m_preview = QPixmap(paths.first());
        emit filesDropped(paths);
        update();
    }
}