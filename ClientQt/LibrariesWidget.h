#pragma once
#include "Client.h"

#include <QWidget>
#include <QListView>
#include <QKeyEvent>
#include <QPushButton>
#include <QStack>
#include <QStandardItemModel>

class QTextEdit;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    LibrariesWidget(QWidget* parent = nullptr);
    ~LibrariesWidget();

private:
    QListView* listView;
    QStandardItemModel* model;
    Client* client;
    QPushButton* refreshButton;
    QPushButton* homeButton;
    QPushButton* backButton;
    QPushButton* forwardButton;
    QStack<QString>* backStack;
    QStack<QString>* forwardStack;
    void UpdateButtons();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public slots:
    void updateList(const QStringList& items);
    void handleError(const QString& errorString);
    void RequestWithSelectedItem();
    void refreshButtonClicked();
    void homeButtonClicked();
    void backButtonClicked();
    void forwardButtonClicked();
};