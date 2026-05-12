#pragma once

#include <QObject>
#include <QString>

class Converter : public QObject
{
    Q_OBJECT

public:
    explicit Converter(QObject *parent = nullptr);

    void convertFile(const QString &inputPath,
                     const QString &outputFormat,
                     bool stripMetadata,
                     bool compress,
                     const QString &outputDir);

    void cleanMetadataOnly(const QString &inputPath,
                           const QString &outputDir);
private:
    bool isDocumentFile(const QString &inputPath) const;

    void convertDocument(const QString &inputPath,
                         const QString &outputFormat,
                         const QString &outputDir);

signals:
    void finished(const QString &outputPath);
    void failed(const QString &message);
};