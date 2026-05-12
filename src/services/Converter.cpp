#include "Converter.h"

#include <QFileInfo>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

static QString bundledToolPath(const QString &toolName)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString bundledPath = appDir + "/bin/" + toolName;

    if (QFileInfo::exists(bundledPath))
        return bundledPath;

    return toolName;
}
Converter::Converter(QObject *parent) : QObject(parent)
{
}

void Converter::convertFile(const QString &inputPath,
                            const QString &outputFormat,
                            bool stripMetadata,
                            bool compress,
                            const QString &outputDir)
{
    if (isDocumentFile(inputPath)) {
    convertDocument(inputPath, outputFormat, outputDir);
    return;
}
    
    QFileInfo fileInfo(inputPath);

    QString outputPath = outputDir + "/" +
                         fileInfo.completeBaseName() + "." +
                         outputFormat;

    QStringList ffmpegArgs;
    ffmpegArgs << "-y" << "-i" << inputPath;

    if (compress && outputFormat == "png")
        ffmpegArgs << "-compression_level" << "9";

    ffmpegArgs << outputPath;

    QProcess *ffmpeg = new QProcess(this);

    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, ffmpeg, outputPath, stripMetadata](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            emit failed("Conversion failed: " + ffmpeg->errorString());
            ffmpeg->deleteLater();
            return;
        }

        if (!stripMetadata) {
            emit finished(outputPath);
            ffmpeg->deleteLater();
            return;
        }

        QProcess *exiftool = new QProcess(this);
        QStringList exifArgs = { "-all=", "-overwrite_original", outputPath };

        connect(exiftool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, exiftool, outputPath](int exifCode, QProcess::ExitStatus) {
            if (exifCode == 0)
                emit finished(outputPath);
            else
                emit failed("Converted, but ExifTool failed: " + exiftool->errorString());

            exiftool->deleteLater();
        });

        exiftool->start("exiftool", exifArgs);

        if (!exiftool->waitForStarted(3000)) {
            emit failed("Converted, but ExifTool was not found.");
            exiftool->deleteLater();
        }

        ffmpeg->deleteLater();
    });

    ffmpeg->start(bundledToolPath("ffmpeg"), ffmpegArgs);

    if (!ffmpeg->waitForStarted(3000)) {
        emit failed("FFmpeg not found. Install: sudo dnf install ffmpeg");
        ffmpeg->deleteLater();
    }
}
void Converter::cleanMetadataOnly(const QString &inputPath,
                                  const QString &outputDir)
{
    QFileInfo fileInfo(inputPath);

    QString outputPath = outputDir + "/" +
                         fileInfo.completeBaseName() +
                         "_clean." +
                         fileInfo.suffix();

    QStringList copyArgs;
    copyArgs << "-y" << "-i" << inputPath << "-c" << "copy" << outputPath;

    QProcess *ffmpeg = new QProcess(this);

    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, ffmpeg, outputPath](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            emit failed("Clean copy failed: " + ffmpeg->errorString());
            ffmpeg->deleteLater();
            return;
        }

        QProcess *exiftool = new QProcess(this);
        QStringList exifArgs = { "-all=", "-overwrite_original", outputPath };

        connect(exiftool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, exiftool, outputPath](int exifCode, QProcess::ExitStatus) {
            if (exifCode == 0)
                emit finished(outputPath);
            else
                emit failed("ExifTool failed: " + exiftool->errorString());

            exiftool->deleteLater();
        });

        exiftool->start(bundledToolPath("exiftool"), exifArgs);

        if (!exiftool->waitForStarted(3000)) {
            emit failed("ExifTool not found. Install: sudo dnf install perl-Image-ExifTool");
            exiftool->deleteLater();
        }

        ffmpeg->deleteLater();
    });

    ffmpeg->start("ffmpeg", copyArgs);

    if (!ffmpeg->waitForStarted(3000)) {
        emit failed("FFmpeg not found. Install: sudo dnf install ffmpeg");
        ffmpeg->deleteLater();
    }
}
bool Converter::isDocumentFile(const QString &inputPath) const
{
    QString ext = QFileInfo(inputPath).suffix().toLower();

    return QStringList{
        "pdf",
        "doc",
        "docx",
        "odt",
        "rtf",
        "txt",
        "ppt",
        "pptx",
        "xls",
        "xlsx"
    }.contains(ext);
}
void Converter::convertDocument(const QString &inputPath,
                                const QString &outputFormat,
                                const QString &outputDir)
{
    QString format = outputFormat.toLower();

    QProcess *libreOffice = new QProcess(this);

    QStringList args;
    args << "--headless"
         << "--convert-to" << format
         << "--outdir" << outputDir
         << inputPath;

    connect(libreOffice, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, libreOffice, inputPath, outputDir, format](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            emit failed("Document conversion failed: " + libreOffice->errorString());
            libreOffice->deleteLater();
            return;
        }

        QFileInfo inputInfo(inputPath);
        QString outputPath = outputDir + "/" + inputInfo.completeBaseName() + "." + format;

        emit finished(outputPath);
        libreOffice->deleteLater();
    });

    libreOffice->start("libreoffice", args);

    if (!libreOffice->waitForStarted(3000)) {
        emit failed("LibreOffice not found. Install: sudo dnf install libreoffice");
        libreOffice->deleteLater();
    }
}