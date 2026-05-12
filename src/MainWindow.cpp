#include "MainWindow.h"

#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QPixmap>
// ════════════════════════════════════════════
// MainWindow
// ════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setFixedSize(390, 540);

    if (QScreen *scr = QApplication::primaryScreen()) {
        QRect sg = scr->availableGeometry();
        move(sg.center() - QPoint(width() / 2, height() / 2));
    }

    m_converter = new Converter(this);
    m_outputDir = QDir::homePath() + "/Downloads/LeafMorph";
    QDir().mkpath(m_outputDir);

connect(m_converter, &Converter::finished, this, [this](const QString &outputPath) {
    m_statusLabel->setText("Done: " + QFileInfo(outputPath).fileName());

    m_currentQueueIndex++;
    processNextFile();
});

connect(m_converter, &Converter::failed, this, [this](const QString &message) {
    m_statusLabel->setText(message);

    m_currentQueueIndex++;
    processNextFile();
});

 buildUi();
applyStyleSheet();

installEventFilter(this);

for (QWidget *w : findChildren<QWidget *>()) {
    w->installEventFilter(this);
}
}

void MainWindow::buildUi()
{
    // ── Card container ────────────────────────
    m_card = new GlassCard(this);
    m_card->setGeometry(6, 6, 378, 528);

    // ── Inner layout inside card ──────────────
    QWidget *inner = new QWidget(m_card);
    inner->setGeometry(22, 20, 334, 488);
    inner->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    inner->setStyleSheet("background: transparent;");

    QVBoxLayout *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    // ── Title bar ────────────────────────────
    QHBoxLayout *titleBar = new QHBoxLayout;
    titleBar->setContentsMargins(0, 0, 0, 0);
   
    auto makeDot = [&](const QString &color, const QString &hover) -> QPushButton * {
        QPushButton *dot = new QPushButton(inner);
        dot->setFixedSize(13, 13);
        dot->setCursor(Qt::PointingHandCursor);
        dot->setStyleSheet(QString(
            "QPushButton {"
            "  background: qradialgradient(cx:0.35,cy:0.3,radius:0.7, stop:0 %1, stop:1 %2);"
            "  border-radius: 6px;"
            "  border: 0.5px solid rgba(0,0,0,0.10);"
            "}"
            "QPushButton:hover { background: %1; }"
        ).arg(hover, color));
        return dot;
    };

    QPushButton *dotR = makeDot("#ff6b6b", "#ff9090");
    QPushButton *dotY = makeDot("#ffcc33", "#ffe080");
    QPushButton *dotG = makeDot("#44cc66", "#80e480");

    // Red → close
    connect(dotR, &QPushButton::clicked, this, &QWidget::close);

    // Yellow → minimize
    connect(dotY, &QPushButton::clicked, this, &QWidget::showMinimized);

    // Green → center on screen (restore to center if dragged away)
    connect(dotG, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            if (QScreen *scr = QApplication::primaryScreen()) {
                QRect sg = scr->availableGeometry();
                move(sg.center() - QPoint(width() / 2, height() / 2));
            }
        }
    });

    QHBoxLayout *dotRow = new QHBoxLayout;
    dotRow->setSpacing(7);
    dotRow->setContentsMargins(0, 0, 0, 0);
    dotRow->addWidget(dotR);
    dotRow->addWidget(dotY);
    dotRow->addWidget(dotG);

    titleBar->addLayout(dotRow);
    titleBar->addStretch();

    QLabel *appNameLabel = new QLabel("LeafMorph", inner);
    appNameLabel->setStyleSheet(
        "color: rgba(40,100,60,0.6);"
        "font-size: 11px;"
        "font-weight: 500;"
        "letter-spacing: 0.5px;"
        "background: transparent;"
    );
    titleBar->addWidget(appNameLabel);
    titleBar->addStretch();
    titleBar->addSpacing(44);

    root->addLayout(titleBar);
    root->addSpacing(8);

    // ── Header ───────────────────────────────
    QVBoxLayout *headerLayout = new QVBoxLayout;
    headerLayout->setAlignment(Qt::AlignHCenter);
    headerLayout->setSpacing(4);

    m_leafIcon = new LeafIcon(inner);

    QHBoxLayout *iconRow = new QHBoxLayout;
    iconRow->addStretch();
    iconRow->addWidget(m_leafIcon);
    iconRow->addStretch();
    headerLayout->addLayout(iconRow);

    m_titleLabel = new QLabel("LeafMorph", inner);
    m_titleLabel->setAlignment(Qt::AlignHCenter);
    m_titleLabel->setStyleSheet(
        "color: #1c6636;"
        "font-size: 17px;"
        "font-weight: 600;"
        "letter-spacing: -0.3px;"
        "background: transparent;"
    );
    headerLayout->addWidget(m_titleLabel);

    m_subtitleLabel = new QLabel("Clean, fast file conversion", inner);
    m_subtitleLabel->setAlignment(Qt::AlignHCenter);
    m_subtitleLabel->setStyleSheet(
        "color: rgba(40,100,60,0.50);"
        "font-size: 10px;"
        "background: transparent;"
    );
    headerLayout->addWidget(m_subtitleLabel);

    root->addLayout(headerLayout);
    root->addSpacing(14);

    // ── Drop zone ────────────────────────────
    m_dropZone = new DropZone(inner);
    connect(m_dropZone, &DropZone::filesDropped, this, &MainWindow::onFilesDropped);
    root->addWidget(m_dropZone);
    root->addSpacing(4);


// ── Dropdowns ────────────────────────────
    QHBoxLayout *combosRow = new QHBoxLayout;
    combosRow->setSpacing(10);
auto makeComboGroup = [&](const QString &labelText, CustomCombo *&combo) {
    QVBoxLayout *col = new QVBoxLayout;
    col->setSpacing(4);

    QLabel *lbl = new QLabel(labelText.toUpper(), inner);
    lbl->setStyleSheet(
        "color: rgba(40,100,60,0.52);"
        "font-size: 8px;"
        "font-weight: 500;"
        "letter-spacing: 0.5px;"
        "background: transparent;"
    );
    col->addWidget(lbl);

    combo = new CustomCombo(inner);
    col->addWidget(combo);
    combosRow->addLayout(col);
};

makeComboGroup("Action", m_actionCombo);
m_actionCombo->addItems({"Convert", "Clean metadata only"});

makeComboGroup("Output", m_formatCombo);
m_formatCombo->addItems({
    "PNG",
    "JPEG",
    "WEBP",
    "BMP",
    "TIFF",
    "ICO",
    "AVIF"
});

root->addLayout(combosRow);
root->addSpacing(14);
QVBoxLayout *outputLayout = new QVBoxLayout;
outputLayout->setSpacing(4);

QLabel *outputLabel = new QLabel("OUTPUT FOLDER", inner);
outputLabel->setStyleSheet(
    "color: rgba(40,100,60,0.52);"
    "font-size: 8px;"
    "font-weight: 500;"
    "letter-spacing: 0.5px;"
    "background: transparent;"
);

outputLayout->addWidget(outputLabel);

m_outputDirButton = new QPushButton(m_outputDir, inner);
m_outputDirButton->setStyleSheet(
    "QPushButton {"
    "  background: rgba(255,255,255,120);"
    "  border: 1px solid rgba(120,195,150,96);"
    "  border-radius: 9px;"
    "  padding: 7px 10px;"
    "  font-size: 9px;"
    "  color: #256040;"
    "  text-align: left;"
    "}"
    "QPushButton:hover {"
    "  background: rgba(255,255,255,180);"
    "}"
);

connect(m_outputDirButton, &QPushButton::clicked,
        this, &MainWindow::chooseOutputDirectory);

outputLayout->addWidget(m_outputDirButton);

root->addLayout(outputLayout);
root->addSpacing(12);
    // ── Divider ──────────────────────────────
    QFrame *divider = new QFrame(inner);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("color: rgba(150,215,175,45);");
    root->addWidget(divider);
    root->addSpacing(4);

    // ── Toggle rows ──────────────────────────
    auto makeToggleRow = [&](const QString &label, const QString &hint,
                              ToggleSwitch *&tog, bool defaultOn) {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(0, 5, 0, 5);

        QVBoxLayout *textCol = new QVBoxLayout;
        textCol->setSpacing(1);

        QLabel *lbl = new QLabel(label, inner);
        lbl->setStyleSheet(
            "color: #2a7244; font-size: 10px; font-weight: 500; background: transparent;"
        );
        textCol->addWidget(lbl);

        QLabel *hintLbl = new QLabel(hint, inner);
        hintLbl->setStyleSheet(
            "color: rgba(60,130,80,0.42); font-size: 8px; background: transparent;"
        );
        textCol->addWidget(hintLbl);

        row->addLayout(textCol);
        row->addStretch();

        tog = new ToggleSwitch(inner);
        tog->setChecked(defaultOn);
        row->addWidget(tog);

        root->addLayout(row);

        QFrame *sep = new QFrame(inner);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color: rgba(150,215,175,40);");
        root->addWidget(sep);
    };

    makeToggleRow("Remove metadata", "Strip EXIF & location data", m_metaToggle, true);
    makeToggleRow("Compress output",  "Reduce file size losslessly", m_compressToggle, false);

    root->addSpacing(6);

    // ── Status label ─────────────────────────
    m_statusLabel = new QLabel("", inner);
    m_statusLabel->setAlignment(Qt::AlignHCenter);
    m_statusLabel->setStyleSheet(
        "color: rgba(40,120,70,0.65); font-size: 9px; background: transparent;"
    );
    root->addWidget(m_statusLabel);
    root->addSpacing(2);

    // ── Convert button ───────────────────────
    m_convertBtn = new QPushButton("  Convert", inner);
    m_convertBtn->setFixedHeight(42);
    m_convertBtn->setCursor(Qt::PointingHandCursor);
    m_convertBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0.1,y1:0,x2:0.9,y2:1,"
        "              stop:0 #6dd68c, stop:0.4 #48c06a,"
        "              stop:0.8 #32a857, stop:1.0 #288f4a);"
        "  color: rgba(255,255,255,0.97);"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "  letter-spacing: 0.15px;"
        "  border: none;"
        "  border-radius: 12px;"
        "  padding-bottom: 1px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0.1,y1:0,x2:0.9,y2:1,"
        "              stop:0 #7ee09a, stop:0.4 #55cc77,"
        "              stop:0.8 #3ab560, stop:1.0 #309f54);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0.1,y1:0,x2:0.9,y2:1,"
        "              stop:0 #5ec07a, stop:1 #258044);"
        "  padding-top: 1px;"
        "}"
        "QPushButton:disabled {"
        "  background: rgba(160,200,170,120);"
        "  color: rgba(255,255,255,0.5);"
        "}"
    );
    connect(m_convertBtn, &QPushButton::clicked, this, &MainWindow::onConvert);
    root->addWidget(m_convertBtn);
    root->addSpacing(8);

    // ── Footer ───────────────────────────────
    m_footerLabel = new QLabel("No files leave your device  ·  v1.0", inner);
    m_footerLabel->setAlignment(Qt::AlignHCenter);
    m_footerLabel->setStyleSheet(
        "color: rgba(60,130,80,0.38); font-size: 8px; background: transparent;"
    );
    root->addWidget(m_footerLabel);

    // ── Leaf float animation ──────────────────
    QTimer *leafTimer = new QTimer(this);
    float phase = 0;
    connect(leafTimer, &QTimer::timeout, [this, phase]() mutable {
        phase += 0.04f;
        Q_UNUSED(phase);
        m_leafIcon->update();
    });
    leafTimer->start(50);
}

void MainWindow::applyStyleSheet()
{
    setStyleSheet("background: transparent;");
}

// ── Background scene ──────────────────────────
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(6, 6, -6, -6);

    QPainterPath windowShape;
    windowShape.addRoundedRect(r, 30, 30);

    QRadialGradient bg(r.center().x(), r.height() * 0.38, r.width() * 1.0);
    bg.setColorAt(0.0, QColor(212, 240, 223));
    bg.setColorAt(0.5, QColor(232, 247, 238));
    bg.setColorAt(1.0, QColor(240, 250, 243));
    p.fillPath(windowShape, bg);
    p.setClipPath(windowShape);
    QRadialGradient b1(r.width() * 0.15, r.height() * 0.15, 160);
    b1.setColorAt(0, QColor(140, 220, 170, 100));
    b1.setColorAt(1, Qt::transparent);
    p.fillRect(r, b1);

    QRadialGradient b2(r.width() * 0.85, r.height() * 0.82, 130);
    b2.setColorAt(0, QColor(110, 200, 150, 80));
    b2.setColorAt(1, Qt::transparent);
    p.fillRect(r, b2);

    QPainterPath leaf1;
    QPointF lc1(r.width() * 0.82, r.height() * 0.08);
    leaf1.moveTo(lc1 + QPointF(0, -45));
    leaf1.cubicTo(lc1 + QPointF(35, -25), lc1 + QPointF(42, 28), lc1 + QPointF(0, 45));
    leaf1.cubicTo(lc1 + QPointF(-42, 28), lc1 + QPointF(-35, -25), lc1 + QPointF(0, -45));
    QRadialGradient lg1(lc1, 50);
    lg1.setColorAt(0, QColor(120, 210, 155, 55));
    lg1.setColorAt(1, Qt::transparent);
    p.fillPath(leaf1, lg1);

    QPainterPath leaf2;
    QPointF lc2(r.width() * 0.14, r.height() * 0.88);
    leaf2.moveTo(lc2 + QPointF(-38, 0));
    leaf2.cubicTo(lc2 + QPointF(-20, -36), lc2 + QPointF(28, -38), lc2 + QPointF(38, 0));
    leaf2.cubicTo(lc2 + QPointF(28, 36), lc2 + QPointF(-20, 36), lc2 + QPointF(-38, 0));
    QRadialGradient lg2(lc2, 45);
    lg2.setColorAt(0, QColor(100, 195, 140, 45));
    lg2.setColorAt(1, Qt::transparent);
    p.fillPath(leaf2, lg2);

    QPointF sc1(r.width() * 0.87, r.height() * 0.13);
    float sr1 = 38;
    QRadialGradient sg1(sc1 - QPointF(sr1 * 0.28, sr1 * 0.28), sr1);
    sg1.setColorAt(0, QColor(255, 255, 255, 170));
    sg1.setColorAt(0.35, QColor(160, 230, 185, 90));
    sg1.setColorAt(0.7, QColor(100, 190, 140, 45));
    sg1.setColorAt(1, Qt::transparent);
    p.setBrush(sg1);
    p.setPen(QPen(QColor(255, 255, 255, 110), 0.8));
    p.drawEllipse(sc1, sr1, sr1);

    QPointF sc2(r.width() * 0.12, r.height() * 0.84);
    float sr2 = 23;
    QRadialGradient sg2(sc2 - QPointF(sr2 * 0.3, sr2 * 0.3), sr2);
    sg2.setColorAt(0, QColor(255, 255, 255, 145));
    sg2.setColorAt(0.4, QColor(140, 215, 170, 70));
    sg2.setColorAt(1, Qt::transparent);
    p.setBrush(sg2);
    p.setPen(QPen(QColor(255, 255, 255, 90), 0.6));
    p.drawEllipse(sc2, sr2, sr2);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        QPoint localPos = mapFromGlobal(e->globalPosition().toPoint());

        if (e->button() == Qt::LeftButton && localPos.y() <= 110) {
            if (qobject_cast<QPushButton *>(obj)) {
                return QWidget::eventFilter(obj, event);
            }

            if (windowHandle()) {
                windowHandle()->startSystemMove();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    m_dragging = false;
    QWidget::mouseReleaseEvent(e);
}
void MainWindow::chooseOutputDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Choose Output Folder",
        m_outputDir
    );

    if (dir.isEmpty())
        return;

    m_outputDir = dir;
    m_outputDirButton->setText(dir);
}
// ── Business logic ────────────────────────────
void MainWindow::onFilesDropped(const QStringList &paths)
{
    m_pendingFiles = paths;

    if (paths.isEmpty())
        return;

    QFileInfo fi(paths.first());
    QString ext = fi.suffix().toLower();
   

m_formatCombo->clear();

    // ── Images ─────────────────────
    if (QStringList{"png","jpg","jpeg","webp","bmp","tiff","gif"}
            .contains(ext))
    {
       m_formatCombo->addItems({
    "PNG",
    "JPEG",
    "WEBP",
    "BMP",
    "TIFF",
    "ICO",
    "AVIF"
});

        m_statusLabel->setText("Image detected");
    }

    // ── Videos ─────────────────────
    else if (QStringList{"mp4","mov","mkv","avi","webm"}
                 .contains(ext))
    {
        m_formatCombo->addItems({
            "MP4", "MKV", "AVI", "MOV", "WEBM"
        });

        m_statusLabel->setText("Video detected");
    }

    // ── Audio ──────────────────────
    else if (QStringList{"mp3","wav","flac","ogg","aac"}
                 .contains(ext))
    {
        m_formatCombo->addItems({
            "MP3", "WAV", "FLAC", "OGG", "AAC"
        });

        m_statusLabel->setText("Audio detected");
    }

    // ── Documents ──────────────────
    else if (QStringList{"pdf","docx","txt","odt","rtf","md"}
                 .contains(ext))
    {
        m_formatCombo->addItems({
            "PDF", "DOCX", "TXT", "ODT", "RTF"
        });

        m_statusLabel->setText("Document detected");
    }

    else {
        m_statusLabel->setText("Unsupported file type");
    }
}

void MainWindow::onCategoryChanged(int index)
{
    m_formatCombo->clear();
    switch (index) {
        case 0: m_formatCombo->addItems({"PNG","JPEG","WEBP","BMP","TIFF"}); break;
        case 1: m_formatCombo->addItems({"PDF","DOCX","TXT","ODT","RTF"});   break;
        case 2: m_formatCombo->addItems({"MP3","AAC","FLAC","OGG","WAV"});   break;
        case 3: m_formatCombo->addItems({"MP4","MKV","AVI","MOV","WEBM"});   break;
    }
}


void MainWindow::onConvert()
{
    if (m_pendingFiles.isEmpty()) {
        m_statusLabel->setText("Drop a file first.");
        return;
    }

    m_currentQueueIndex = 0;

    m_convertBtn->setEnabled(false);

    processNextFile();
}
void MainWindow::processNextFile()
{
    if (m_currentQueueIndex >= m_pendingFiles.size()) {
        m_statusLabel->setText("All files done.");
        m_convertBtn->setEnabled(true);
        return;
    }

    QString path = m_pendingFiles[m_currentQueueIndex];

    QString fmt = m_formatCombo->currentText().toLower();
    bool stripMeta = m_metaToggle->isChecked();
    bool compress = m_compressToggle->isChecked();

    m_statusLabel->setText(
        QString("Converting %1/%2...")
            .arg(m_currentQueueIndex + 1)
            .arg(m_pendingFiles.size())
    );

    if (m_actionCombo->currentText() == "Clean metadata only") {
        m_converter->cleanMetadataOnly(path, m_outputDir);
    } else {
        m_converter->convertFile(
            path,
            fmt,
            stripMeta,
            compress,
            m_outputDir
        );
    }
}
void MainWindow::runConversion(const QString &inputPath,
                               const QString &outputFormat,
                               bool stripMeta,
                               bool compress)
{
    QFileInfo fi(inputPath);
    QString outputPath = fi.absolutePath() + "/" +
                         fi.completeBaseName() + "." + outputFormat;

    QStringList ffmpegArgs;
    ffmpegArgs << "-y" << "-i" << inputPath;
    if (compress && outputFormat == "png")
        ffmpegArgs << "-compression_level" << "9";
    ffmpegArgs << outputPath;

    QProcess *ffmpeg = new QProcess(this);

    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, ffmpeg, outputPath, stripMeta](int exitCode, QProcess::ExitStatus) {

        if (exitCode != 0) {
            m_statusLabel->setText("Conversion failed: " + ffmpeg->errorString());
            m_convertBtn->setEnabled(true);
            ffmpeg->deleteLater();
            return;
        }

        if (stripMeta) {
            QProcess *exiftool = new QProcess(this);
            QStringList exifArgs = { "-all=", "-overwrite_original", outputPath };

            connect(exiftool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, [this, exiftool, outputPath](int exifCode, QProcess::ExitStatus) {

                if (exifCode == 0)
                    m_statusLabel->setText("Done — metadata stripped: " +
                                           QFileInfo(outputPath).fileName());
                else
                    m_statusLabel->setText("Converted, but ExifTool failed: " +
                                           exiftool->errorString());

                m_convertBtn->setEnabled(true);
                exiftool->deleteLater();
            });

            exiftool->start("exiftool", exifArgs);

            if (!exiftool->waitForStarted(3000)) {
                m_statusLabel->setText("Done — ExifTool not found. Install: sudo dnf install perl-Image-ExifTool");
                m_convertBtn->setEnabled(true);
                exiftool->deleteLater();
            }
        } else {
            m_statusLabel->setText("Done: " + QFileInfo(outputPath).fileName());
            m_convertBtn->setEnabled(true);
        }

        ffmpeg->deleteLater();
    });

    ffmpeg->start("ffmpeg", ffmpegArgs);

    if (!ffmpeg->waitForStarted(3000)) {
        m_statusLabel->setText("FFmpeg not found. Install: sudo dnf install ffmpeg");
        m_convertBtn->setEnabled(true);
        ffmpeg->deleteLater();
    }
}