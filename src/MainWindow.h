#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QStringList>
#include <QPoint>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "widgets/ToggleSwitch.h"
#include "widgets/DropZone.h"
#include "widgets/GlassCard.h"
#include "widgets/LeafIcon.h"
#include "widgets/CustomCombo.h"
#include "services/Converter.h"
// ─────────────────────────────────────────────
// MainWindow
// ─────────────────────────────────────────────
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
private slots:
    void onConvert();
    void onFilesDropped(const QStringList &paths);
    void onCategoryChanged(int index);
    void processNextFile();
    void chooseOutputDirectory();
private:
    void buildUi();
    void applyStyleSheet();
    void runConversion(const QString &inputPath, const QString &outputFormat,
                       bool stripMeta, bool compress);
 GlassCard *m_card = nullptr;
 LeafIcon *m_leafIcon = nullptr;
 QLabel *m_titleLabel = nullptr;
 QLabel *m_subtitleLabel = nullptr;
 DropZone *m_dropZone = nullptr;
 CustomCombo *m_categoryCombo = nullptr;
 CustomCombo *m_formatCombo = nullptr;
 CustomCombo *m_actionCombo = nullptr;
 ToggleSwitch *m_metaToggle = nullptr;
 ToggleSwitch *m_compressToggle = nullptr;
 QPushButton *m_convertBtn = nullptr;
 QLabel *m_statusLabel = nullptr;
 QLabel *m_previewLabel = nullptr;
 QString m_outputDir;
 QPushButton *m_outputDirButton = nullptr;
 QLabel *m_footerLabel = nullptr;
 Converter *m_converter = nullptr;
 QProcess *m_process = nullptr;

    // Drag to move window
    QPoint m_dragStart;
    bool   m_dragging = false;

    // Pending files
    int m_currentQueueIndex = 0;
    QStringList m_pendingFiles;

    
};
