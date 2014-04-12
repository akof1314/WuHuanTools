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

    enum DirectionType
    {
        Left,
        Right
    };

    void compare();

private slots:
    void onActionCodeTriggered();
    void onPushButtonFileClicked(DirectionType eType);
    void onTextEditCursorPositionChanged(DirectionType eType);
    void onThreadCompateFinished();

    void on_pushButtonFileLeft_clicked();
    void on_pushButtonFileRight_clicked();
    void on_textEditLeft_cursorPositionChanged();
    void on_textEditRight_cursorPositionChanged();

private:

    struct CompareInfo
    {
        QString value;
        int begin;
        int end;
        bool operator==(const CompareInfo &s) const;
        bool operator!=(const CompareInfo &s) const;
    };
    typedef QList<CompareInfo> ListCompareInfo;

    void createMenuButton();
    QVariant compareThread(const QString &fileNameLeft, const QString &fileNameRight, const QString &codecNameLeft, const QString &codecNameRight);
    bool getMapStrID(const QString &fileName, QMap<uint, QByteArray> &mapStrID, QString &sameStrID);
    bool justCompareValue(QByteArray &valueLeft, QByteArray &valueRight);
    bool getEscapeList(const QByteArray &value, ListCompareInfo &listEscape);
    bool getPercentList(const QByteArray &value, ListCompareInfo &listPercent);
    QPair<int, int> justCompareListValue(const ListCompareInfo &listValueLeft, const ListCompareInfo &listValueRight);
    void highlightCompare(int idx, const ListCompareInfo &listValue, QByteArray &value, int color);
    QByteArray byteArrayToHtmlEscaped(const QByteArray &value);
    QByteArray byteArrayVsprintfFormat(const QByteArray &value);

    void readSettings();
    void writeSettings();

    Ui::FormDialog *ui;
    QFutureWatcher<QVariant> futureWatcher;
};

#endif // FORMDIALOG_H
