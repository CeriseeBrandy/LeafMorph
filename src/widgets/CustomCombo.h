#pragma once

#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QStringList>
#include <QList>
#include <QEvent>

class CustomCombo : public QWidget
{
    Q_OBJECT

public:
    explicit CustomCombo(QWidget *parent = nullptr);

    void addItems(const QStringList &items);
    void clear();
    QString currentText() const;
    int currentIndex() const;
    void setCurrentIndex(int index);

signals:
    void currentIndexChanged(int index);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void togglePopup();
    void selectItem(int index);

private:
    void buildPopup();
    void closePopup();
    void updateButton();
    QString buttonStyle() const;
    QString itemStyle() const;

    QPushButton *m_button = nullptr;
    QFrame *m_popup = nullptr;
    QVBoxLayout *m_popupLayout = nullptr;
    QList<QPushButton *> m_items;

    QStringList m_texts;
    int m_currentIndex = -1;
    bool m_open = false;
};