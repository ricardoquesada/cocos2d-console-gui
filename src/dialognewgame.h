#ifndef DIALOGNEWGAME_H
#define DIALOGNEWGAME_H

#include <QDialog>
#include <QListWidgetItem>
#include <QList>

#include "templateentry.h"

namespace Ui {
class DialogNewGame;
}

class DialogNewGame : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewGame(QWidget *parent = 0);
    ~DialogNewGame();

protected:
    void parseTemplates();
    void populateTemplateList(const QString& title, QList<TemplateEntry>* list, QListWidget* parent);

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_buttonBox_accepted();

private:
    QList<TemplateEntry> _entriesCpp;
    QList<TemplateEntry> _entriesLua;
    QList<TemplateEntry> _entriesJavaScript;

    Ui::DialogNewGame *ui;
};

#endif // DIALOGNEWGAME_H
