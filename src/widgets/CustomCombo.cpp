#include "CustomCombo.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QtGlobal>

CustomCombo::CustomCombo(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_button = new QPushButton(this);
    m_button->setCursor(Qt::PointingHandCursor);
    m_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_button, &QPushButton::clicked, this, &CustomCombo::togglePopup);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_button);

    updateButton();
}

QString CustomCombo::buttonStyle() const
{
    return
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "              stop:0 rgba(255,255,255,168),"
        "              stop:1 rgba(240,252,246,138));"
        "  border: 1px solid rgba(120,195,150,96);"
        "  border-radius: 9px;"
        "  padding: 0px 28px 0px 9px;"
        "  font-size: 10px;"
        "  font-weight: 500;"
        "  color: #256040;"
        "  text-align: left;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,255,255,210);"
        "  border-color: rgba(80,170,115,150);"
        "}";
}

QString CustomCombo::itemStyle() const
{
    return
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 5px 10px;"
        "  font-size: 10px;"
        "  font-weight: 500;"
        "  color: #256040;"
        "  text-align: left;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(100,200,140,80);"
        "}";
}

void CustomCombo::updateButton()
{
    QString label = (m_currentIndex >= 0 && m_currentIndex < m_texts.size())
        ? m_texts[m_currentIndex] + "  ▾"
        : "  ▾";

    m_button->setStyleSheet(buttonStyle());
    m_button->setText(label);
}

void CustomCombo::addItems(const QStringList &items)
{
    m_texts << items;

    if (m_currentIndex < 0 && !m_texts.isEmpty()) {
        m_currentIndex = 0;
        updateButton();
    }
}

void CustomCombo::clear()
{
    m_texts.clear();
    m_currentIndex = -1;
    closePopup();
    updateButton();
}

QString CustomCombo::currentText() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_texts.size())
        return m_texts[m_currentIndex];

    return {};
}

int CustomCombo::currentIndex() const
{
    return m_currentIndex;
}

void CustomCombo::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_texts.size())
        return;

    m_currentIndex = index;
    updateButton();

    emit currentIndexChanged(index);
}

void CustomCombo::buildPopup()
{
    QWidget *topLevel = window();

    m_popup = new QFrame(topLevel);
    m_popup->setObjectName("comboPopup");
    m_popup->setStyleSheet(
        "QFrame#comboPopup {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "              stop:0 rgba(245,254,249,245),"
        "              stop:1 rgba(230,248,238,240));"
        "  border: 1px solid rgba(120,195,150,140);"
        "  border-radius: 10px;"
        "}"
    );

    m_popupLayout = new QVBoxLayout(m_popup);
    m_popupLayout->setContentsMargins(4, 4, 4, 4);
    m_popupLayout->setSpacing(1);

    m_items.clear();

    for (int i = 0; i < m_texts.size(); ++i) {
        QPushButton *button = new QPushButton(m_texts[i], m_popup);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(24);
        button->setStyleSheet(itemStyle());

        if (i == m_currentIndex) {
            button->setStyleSheet(
                "QPushButton {"
                "  background: rgba(100,200,140,60);"
                "  border: none;"
                "  border-radius: 6px;"
                "  padding: 5px 10px;"
                "  font-size: 10px;"
                "  font-weight: 600;"
                "  color: #1c6636;"
                "  text-align: left;"
                "}"
                "QPushButton:hover {"
                "  background: rgba(100,200,140,90);"
                "}"
            );
        }

        connect(button, &QPushButton::clicked, this, [this, i]() {
            selectItem(i);
        });

        m_popupLayout->addWidget(button);
        m_items.append(button);
    }

    int popupHeight = m_texts.size() * 26 + 8;
    int popupWidth = qMax(width(), 110);

    QPoint globalPos = mapToGlobal(QPoint(0, height()));
    QPoint localPos = topLevel->mapFromGlobal(globalPos);

    m_popup->setGeometry(localPos.x(), localPos.y(), popupWidth, popupHeight);
    m_popup->raise();
    m_popup->show();

    topLevel->installEventFilter(this);
}

void CustomCombo::closePopup()
{
    if (!m_popup)
        return;

    window()->removeEventFilter(this);

    m_popup->hide();
    m_popup->deleteLater();

    m_popup = nullptr;
    m_popupLayout = nullptr;
    m_items.clear();
    m_open = false;
}

void CustomCombo::togglePopup()
{
    if (m_open) {
        closePopup();
        return;
    }

    m_open = true;
    buildPopup();
}

void CustomCombo::selectItem(int index)
{
    closePopup();

    if (index == m_currentIndex)
        return;

    m_currentIndex = index;
    updateButton();

    emit currentIndexChanged(index);
}

bool CustomCombo::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (m_open && m_popup && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint popupPos = m_popup->mapFromGlobal(mouseEvent->globalPosition().toPoint());

        if (!m_popup->rect().contains(popupPos))
            closePopup();
    }

    return QWidget::eventFilter(obj, event);
}