#include "formdialog.h"
#include "ui_formdialog.h"
#include <QMenu>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrentRun>
#include <QFile>
#include <QTextCodec>
#include <QTextBlock>
#include <QTextLayout>

FormDialog::FormDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDialog)
{
    ui->setupUi(this);

    readSettings();
    createMenuButton();

    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(onThreadCompateFinished()));
}

FormDialog::~FormDialog()
{
    writeSettings();
    delete ui;
}

void FormDialog::compare()
{
    QString fileNameLeft = ui->comboBoxFileLeft->currentText();
    QString fileNameRight = ui->comboBoxFileRight->currentText();
    if (fileNameLeft.isEmpty() || fileNameRight.isEmpty())
    {
        QMessageBox::critical(this, QStringLiteral("提示"),
                              QStringLiteral("请输入要对比的文件路径！"));
        return;
    }

    QProgressDialog *progressDialog = new QProgressDialog(this);
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(0);
    progressDialog->setCancelButton(0);
    progressDialog->setWindowTitle(QStringLiteral("等待"));
    progressDialog->setLabelText(QStringLiteral("正在对比中..."));
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(&futureWatcher, SIGNAL(finished()), progressDialog, SLOT(cancel()));

    QFuture<QVariant> future = QtConcurrent::run(this, &FormDialog::compareThread, fileNameLeft, fileNameRight,
                                                 ui->pushButtonCodeLeft->text(),
                                                 ui->pushButtonCodeRight->text());
    futureWatcher.setFuture(future);
    progressDialog->exec();
}

void FormDialog::onActionCodeTriggered()
{
    QObject *object = sender();
    if (object == Q_NULLPTR)
    {
        return;
    }

    QAction *action = qobject_cast<QAction*>(object);
    QString actionText = action->text();
    int position = actionText.indexOf(" (") + 2;
    QString codecName = actionText.mid(position, actionText.length() - 1 - position);

    QWidget *pushMenu = action->parentWidget();
    if (pushMenu)
    {
        QPushButton *pushButton = qobject_cast<QPushButton*>(pushMenu->parentWidget());
        if (pushButton)
        {
            if (codecName != pushButton->text())
            {
                pushButton->setText(codecName);
                if (!ui->textEditLeft->toHtml().isEmpty() || !ui->textEditRight->toHtml().isEmpty())
                {
                    compare();
                }
            }
        }
    }
}

void FormDialog::onPushButtonFileClicked(FormDialog::DirectionType eType)
{
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开"));
    if (fileName.length() == 0)
    {
        return;
    }

    if (eType == DirectionType::Left)
    {
        if (-1 == ui->comboBoxFileLeft->findText(fileName))
        {
            ui->comboBoxFileLeft->addItem(fileName);
        }
        ui->comboBoxFileLeft->setCurrentText(fileName);
    }
    else
    {
        if (-1 == ui->comboBoxFileRight->findText(fileName))
        {
            ui->comboBoxFileRight->addItem(fileName);
        }
        ui->comboBoxFileRight->setCurrentText(fileName);
    }
}

void FormDialog::onThreadCompateFinished()
{
    QVariant result = futureWatcher.future().result();
    if (result.type() != QVariant::List)
    {
        QString msg;
        switch (result.toInt())
        {
        case 1:
            msg = QStringLiteral("左侧文件打开失败！");
            break;
        case 2:
            msg = QStringLiteral("右侧文件打开失败！");
            break;
        case 3:
            msg = QStringLiteral("左侧文件无任何字符串ID！");
            break;
        case 4:
            msg = QStringLiteral("右侧文件无任何字符串ID！");
            break;
        }
        QMessageBox::critical(this, QStringLiteral("提示"),
                              msg);
    }
    else
    {
        QList<QVariant> listReuslt = result.toList();
        ui->textEditLeft->setHtml(listReuslt.at(0).toString());
        ui->textEditRight->setHtml(listReuslt.at(1).toString());
    }
}

void FormDialog::createMenuButton()
{
    QStringList listText;
    listText.append(QStringLiteral("中文（简体） (GBK)"));
    listText.append(QStringLiteral("中文（繁体） (Big5)"));
    listText.append(QStringLiteral("韩文 (EUC-KR)"));
    listText.append(QStringLiteral("日文 (Shift_JIS)"));
    listText.append(QStringLiteral("越南文 (windows-1258)"));

    QMenu *menuLeft = new QMenu(ui->pushButtonCodeLeft);
    QMenu *menuRight = new QMenu(ui->pushButtonCodeRight);
    for (int i = 0; i < listText.size(); ++i)
    {
        menuLeft->addAction(listText.at(i), this, SLOT(onActionCodeTriggered()));
        menuRight->addAction(listText.at(i), this, SLOT(onActionCodeTriggered()));
    }
    ui->pushButtonCodeLeft->setMenu(menuLeft);
    ui->pushButtonCodeRight->setMenu(menuRight);
}

QVariant FormDialog::compareThread(const QString &fileNameLeft, const QString &fileNameRight, const QString &codecNameLeft, const QString &codecNameRight)
{
    QMap<uint, QByteArray> mapLeft, mapRight;
    QString sameLeft, sameRight;
    if (getMapStrID(fileNameLeft, mapLeft, sameLeft) == false)
    {
        return 1;
    }
    if (getMapStrID(fileNameRight, mapRight, sameRight) == false)
    {
        return 2;
    }
    if (mapLeft.count() == 0)
    {
        return 3;
    }
    if (mapRight.count() == 0)
    {
        return 4;
    }

    QTextCodec *textCodecLeft = QTextCodec::codecForName(codecNameLeft.toStdString().c_str());
    QTextCodec *textCodecRight = QTextCodec::codecForName(codecNameRight.toStdString().c_str());
    if (!textCodecLeft || !textCodecRight)
    {
        return 5;
    }

    QString compareLeft, compareRight;
    QString nothingLeft, nothingRight;
    QMap<uint, QByteArray>::const_iterator i = mapLeft.constBegin();
    while (i != mapLeft.constEnd())
    {
        if (mapRight.contains(i.key()))
        {
            if (justCompareValue(i.value(), mapRight.value(i.key())) == false)
            {
                compareLeft.append(QString("%1=%2<br/>").arg(i.key()).arg(textCodecLeft->toUnicode(i.value().constData()).toHtmlEscaped()));
                compareRight.append(QString("%1=%2<br/>").arg(i.key()).arg(textCodecRight->toUnicode(mapRight.value(i.key()).constData()).toHtmlEscaped()));
            }
        }
        else
        {
            nothingLeft.append(QString("%1=%2<br/>").arg(i.key()).arg(textCodecLeft->toUnicode(i.value().constData()).toHtmlEscaped()));
        }
        ++i;
    }
    i = mapRight.constBegin();
    while (i != mapRight.constEnd())
    {
        if (mapLeft.contains(i.key()))
        {
        }
        else
        {
            nothingRight.append(QString("%1=%2<br/>").arg(i.key()).arg(textCodecRight->toUnicode(i.value().constData()).toHtmlEscaped()));
        }
        ++i;
    }

    sameLeft.insert(0, QStringLiteral("<br/><br/><font color=red>重复的字符串ID:</font><br/>"));
    sameRight.insert(0, QStringLiteral("<br/><br/><font color=red>重复的字符串ID:</font><br/>"));
    nothingLeft.insert(0, QStringLiteral("<br/><br/><font color=red>右侧不存在的字符串ID:</font><br/>"));
    nothingRight.insert(0, QStringLiteral("<br/><br/><font color=red>左侧不存在的字符串ID:</font><br/>"));
    compareLeft.append(nothingLeft);
    compareLeft.append(sameLeft);
    compareRight.append(nothingRight);
    compareRight.append(sameRight);

    QList<QVariant> listReuslt;
    listReuslt.append(compareLeft);
    listReuslt.append(compareRight);
    return listReuslt;
}

bool FormDialog::getMapStrID(const QString &fileName, QMap<uint, QByteArray> &mapStrID, QString &sameStrID)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray byData;
    byData = file.readAll();
    file.close();

    QList<QByteArray> listByData = byData.split('\n');
    for (int i = 0; i < listByData.count(); ++i)
    {
        QByteArray lineData = listByData.at(i);
        int idxMid = lineData.indexOf('=');
        if (idxMid != -1)
        {
            QByteArray strID = lineData.left(idxMid);
            QByteArray strValue = lineData.mid(idxMid + 1);

            uint uID = strID.trimmed().toUInt();
            if (uID != 0)
            {
                if (mapStrID.contains(uID))
                {
                    sameStrID.append(QString::fromLocal8Bit(strID));
                    sameStrID.append("<br/>");
                }
                else
                {
                    mapStrID[uID] = strValue;
                }
            }
        }
    }

    return true;
}

bool FormDialog::justCompareValue(const QByteArray &valueLeft, const QByteArray &valueRight)
{
    QStringList listEscapeLeft, listEscapeRight;
    getEscapeList(valueLeft, listEscapeLeft);
    getEscapeList(valueRight, listEscapeRight);

    if (listEscapeLeft != listEscapeRight)
    {
        return false;
    }

    QStringList listPercentLeft, listPercentRight;
    getPercentList(valueLeft, listPercentLeft);
    getPercentList(valueRight, listPercentRight);

    if (listPercentLeft != listPercentRight)
    {
        return false;
    }

    return true;
}

bool FormDialog::getEscapeList(const QByteArray &value, QStringList &listEscape)
{
    int idxEnd;
    int idxBegin = value.indexOf("</");
    while (idxBegin != -1)
    {
        idxEnd = value.indexOf(">", idxBegin);
        if (idxEnd != -1)
        {
            listEscape.append(value.mid(idxBegin, idxEnd - idxBegin + 1));
            idxBegin = value.indexOf("</", idxEnd);
            continue;
        }
        break;
    }
    return true;
}

bool FormDialog::getPercentList(const QByteArray &value, QStringList &listPercent)
{
    int idxBegin = value.indexOf("%");
    while (idxBegin != -1)
    {
        if ((idxBegin + 1) < value.count())
        {
            listPercent.append(QString(value.at(idxBegin + 1)));
        }
        idxBegin = value.indexOf("%", idxBegin + 2);
    }
    return true;
}

void FormDialog::readSettings()
{
    QSettings settings;
    ui->comboBoxFileLeft->addItems(settings.value("fileListLeft").toStringList());
    ui->comboBoxFileRight->addItems(settings.value("fileListRight").toStringList());
}

void FormDialog::writeSettings()
{
    QStringList fileListLeft, fileListRight;
    int minIndex = qMax(0, ui->comboBoxFileLeft->count() - 5);
    for (int i = minIndex; i < ui->comboBoxFileLeft->count(); ++i)
    {
        fileListLeft.append(ui->comboBoxFileLeft->itemText(i));
    }
    minIndex = qMax(0, ui->comboBoxFileRight->count() - 5);
    for (int i = minIndex; i < ui->comboBoxFileRight->count(); ++i)
    {
        fileListRight.append(ui->comboBoxFileRight->itemText(i));
    }

    QSettings settings;
    settings.setValue("fileListLeft", fileListLeft);
    settings.setValue("fileListRight", fileListRight);
}

void FormDialog::on_pushButtonFileLeft_clicked()
{
    onPushButtonFileClicked(DirectionType::Left);
}

void FormDialog::on_pushButtonFileRight_clicked()
{
    onPushButtonFileClicked(DirectionType::Right);
}

void FormDialog::on_textEditLeft_cursorPositionChanged()
{
    QTextCursor currentCursor = ui->textEditLeft->textCursor();
    currentCursor.movePosition(QTextCursor::StartOfLine);
    int begin = currentCursor.position();
    currentCursor.movePosition(QTextCursor::EndOfLine);
    int end = currentCursor.position();
    qDebug("%s", ui->textEditLeft->toPlainText().mid(begin, end - begin + 1).toStdString().c_str());

    QTextLayout *layout = currentCursor.block().layout();
    int pos = currentCursor.position() - currentCursor.block().position();
    int line = layout->lineForTextPosition(pos).lineNumber() + currentCursor.block().firstLineNumber();
    qDebug("%d", line);

    QTextCursor cursorRight = ui->textEditRight->textCursor();
    QTextLayout *layoutRight = cursorRight.block().layout();
    int a = layoutRight->lineAt(line).textStart();
    int b = layoutRight->lineAt(line).textLength();
    qDebug("aaa %d %d", a,b);
    int position = ui->textEditRight->document()->findBlockByLineNumber(line).position();
    cursorRight.setPosition(position);
     qDebug("%d", position);
    ui->textEditRight->setTextCursor(cursorRight);
}
