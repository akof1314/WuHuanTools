#ifndef FORMDIALOG_H
#define FORMDIALOG_H

#include <QDialog>
#include <QFutureWatcher>

namespace Ui {
class FormDialog;
}

class FormDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FormDialog(QWidget *parent = 0);
    ~FormDialog();

    enum class DirectionType
    {
        Left,
        Right
    };

    void compare();

private slots:
    void onActionCodeTriggered();
    void onPushButtonFileClicked(DirectionType eType);
    void onThreadCompateFinished();

    void on_pushButtonFileLeft_clicked();
    void on_pushButtonFileRight_clicked();

    void on_textEditLeft_cursorPositionChanged();

private:
    void createMenuButton();
    QVariant compareThread(const QString &fileNameLeft, const QString &fileNameRight, const QString &codecNameLeft, const QString &codecNameRight);
    bool getMapStrID(const QString &fileName, QMap<uint, QByteArray> &mapStrID, QString &sameStrID);
    bool justCompareValue(const QByteArray &valueLeft, const QByteArray &valueRight);
    bool getEscapeList(const QByteArray &value, QStringList &listEscape);
    bool getPercentList(const QByteArray &value, QStringList &listPercent);

    void readSettings();
    void writeSettings();

    Ui::FormDialog *ui;
    QFutureWatcher<QVariant> futureWatcher;
};

#endif // FORMDIALOG_H
