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
#include <QFileInfo>
#include <QDateTime>
#include <QScrollBar>

FormDialog::FormDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDialog)
{
    ui->setupUi(this);

    readSettings();
    createMenuButton();

    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(onThreadCompateFinished()));
    connect(ui->textEditLeft->verticalScrollBar(), &QScrollBar::valueChanged,
            ui->textEditRight->verticalScrollBar(), &QScrollBar::setValue);
    connect(ui->textEditRight->verticalScrollBar(), &QScrollBar::valueChanged,
            ui->textEditLeft->verticalScrollBar(), &QScrollBar::setValue);
    connect(ui->textEditLeft->horizontalScrollBar(), &QScrollBar::valueChanged,
            ui->textEditRight->horizontalScrollBar(), &QScrollBar::setValue);
    connect(ui->textEditRight->horizontalScrollBar(), &QScrollBar::valueChanged,
            ui->textEditLeft->horizontalScrollBar(), &QScrollBar::setValue);
}

FormDialog::~FormDialog()
{
    writeSettings();
    delete ui;
}

void FormDialog::compare()
{
    ui->textEditLeft->clear();
    ui->textEditRight->clear();

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

    QFileInfo infoLeft(fileNameLeft);
    ui->labelTimeLeft->setText(infoLeft.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    QFileInfo infoRight(fileNameRight);
    ui->labelTimeRight->setText(infoRight.lastModified().toString("yyyy-MM-dd hh:mm:ss"));

    setWindowTitle(infoLeft.fileName());
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
                if (!ui->textEditLeft->toPlainText().isEmpty() || !ui->textEditRight->toPlainText().isEmpty())
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

void FormDialog::onTextEditCursorPositionChanged(FormDialog::DirectionType eType)
{
    QTextEdit *edit = nullptr;
    if (eType == DirectionType::Left)
    {
        edit = ui->textEditLeft;
    }
    else
    {
        edit = ui->textEditRight;
    }
    QTextCursor currentCursor = edit->textCursor();
    QTextLayout *layout = currentCursor.block().layout();
    int pos = currentCursor.position() - currentCursor.block().position();
    int line = layout->lineForTextPosition(pos).lineNumber() + currentCursor.block().firstLineNumber();

    QStringList listLeft = ui->textEditLeft->toHtml().split("<br />");
    if (listLeft.count() > line)
    {
        ui->labelLeft->setText(listLeft.at(line));
    }
    QStringList listRight = ui->textEditRight->toHtml().split("<br />");
    if (listRight.count() > line)
    {
        ui->labelRight->setText(listRight.at(line));
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

        QStringList listLeft = ui->textEditLeft->toHtml().split("<br />");
        QStringList listRight = ui->textEditRight->toHtml().split("<br />");

        if (listLeft.count() > listRight.count())
        {
            ui->textEditRight->append(QString("<br/>").repeated(listLeft.count() - listRight.count()));
        }
        else if (listLeft.count() < listRight.count())
        {
            ui->textEditLeft->append(QString("<br/>").repeated(listRight.count() - listLeft.count()));
        }
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

QVariant FormDialog::compareThread(const QString &fileNameLeft, const QString &fileNameRight,
                                   const QString &codecNameLeft, const QString &codecNameRight)
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
            QByteArray valueLeft = byteArrayToHtmlEscaped(i.value());
            QByteArray valueRight = byteArrayToHtmlEscaped(mapRight.value(i.key()));
            if (justCompareValue(valueLeft, valueRight) == false)
            {
                compareLeft.append(QString("%1=%2<br/>")
                                   .arg(i.key())
                                   .arg(textCodecLeft->toUnicode(valueLeft.constData())));
                compareRight.append(QString("%1=%2<br/>")
                                    .arg(i.key())
                                    .arg(textCodecRight->toUnicode(valueRight.constData())));
            }
        }
        else
        {
            nothingLeft.append(QString("%1=%2<br/>")
                               .arg(i.key())
                               .arg(textCodecLeft->toUnicode(i.value().constData()).toHtmlEscaped()));
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
            nothingRight.append(QString("%1=%2<br/>")
                                .arg(i.key())
                                .arg(textCodecRight->toUnicode(i.value().constData()).toHtmlEscaped()));
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
            if (strValue.endsWith('\r'))
            {
                strValue.remove(strValue.count() - 1, 1);
            }

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

bool FormDialog::justCompareValue(QByteArray &valueLeft, QByteArray &valueRight)
{
    ListCompareInfo listPercentLeft, listPercentRight;
    getPercentList(valueLeft, listPercentLeft);
    getPercentList(valueRight, listPercentRight);

    QPair<int, int> resultPercent = justCompareListValue(listPercentLeft, listPercentRight);
    if (resultPercent.first + resultPercent.second != -2)
    {
        highlightCompare(resultPercent.first, listPercentLeft, valueLeft, 0);
        highlightCompare(resultPercent.second, listPercentRight, valueRight, 0);
        return false;
    }

    ListCompareInfo listEscapeLeft, listEscapeRight;
    getEscapeList(valueLeft, listEscapeLeft);
    getEscapeList(valueRight, listEscapeRight);

    QPair<int, int> resultEscape = justCompareListValue(listEscapeLeft, listEscapeRight);
    if (resultEscape.first + resultEscape.second != -2)
    {
        highlightCompare(resultEscape.first, listEscapeLeft, valueLeft, 1);
        highlightCompare(resultEscape.second, listEscapeRight, valueRight, 1);
        return false;
    }

    bool emptyLeft = valueLeft.trimmed().isEmpty();
    bool emptyRight = valueRight.trimmed().isEmpty();
    if ((emptyLeft && !emptyRight) || (!emptyLeft && emptyRight))
    {
        return false;
    }

    return true;
}

bool FormDialog::getEscapeList(const QByteArray &value, ListCompareInfo &listEscape)
{
    int idxEnd;
    int idxBegin = value.indexOf("&lt;/");
    while (idxBegin != -1)
    {
        idxEnd = value.indexOf("&gt;", idxBegin);
        if (idxEnd != -1)
        {
            CompareInfo info;
            info.value = value.mid(idxBegin, idxEnd - idxBegin + 1);
            info.begin = idxBegin;
            info.end = idxEnd + 4;
            listEscape.append(info);
            idxBegin = value.indexOf("&lt;/", idxEnd);
            continue;
        }
        break;
    }
    return true;
}

bool FormDialog::getPercentList(const QByteArray &value, ListCompareInfo &listPercent)
{
    int idxBegin = value.indexOf("%");
    while (idxBegin != -1)
    {
        if ((idxBegin + 1) < value.count())
        {
            QByteArray format = byteArrayVsprintfFormat(value.mid(idxBegin));
            if (format.count() > 0)
            {
                CompareInfo info;
                info.value = format;
                info.begin = idxBegin;
                info.end = idxBegin + format.count() + 1;
                listPercent.append(info);
            }
        }
        idxBegin = value.indexOf("%", idxBegin + 2);
    }
    return true;
}

QPair<int, int> FormDialog::justCompareListValue(const ListCompareInfo &listValueLeft,
                                                 const ListCompareInfo &listValueRight)
{
    for (int i = 0; i < listValueLeft.count() && i < listValueRight.count(); ++i)
    {
        if (listValueLeft.at(i) != listValueRight.at(i))
        {
            return qMakePair(i, i);
        }
    }

    if (listValueLeft.count() > listValueRight.count())
    {
        return qMakePair(listValueRight.count(), -1);
    }
    else if (listValueLeft.count() < listValueRight.count())
    {
        return qMakePair(-1, listValueLeft.count());
    }

    return qMakePair(-1, -1);
}

void FormDialog::highlightCompare(int idx, const FormDialog::ListCompareInfo &listValue,
                                  QByteArray &value, int color)
{
    if (idx != -1)
    {
        value.insert(listValue.at(idx).end, "</font>");
        if (color == 0)
        {
            value.insert(listValue.at(idx).begin, "<font color=red>");
        }
        else
        {
            value.insert(listValue.at(idx).begin, "<font color=blue>");
        }
    }
}

QByteArray FormDialog::byteArrayToHtmlEscaped(const QByteArray &value)
{
    QByteArray rich;
    const int len = value.length();
    rich.reserve(int(len * 1.1));
    for (int i = 0; i < len; ++i) {
        if (value.at(i) == QLatin1Char('<'))
            rich += QLatin1String("&lt;");
        else if (value.at(i) == QLatin1Char('>'))
            rich += QLatin1String("&gt;");
        else if (value.at(i) == QLatin1Char('&'))
            rich += QLatin1String("&amp;");
        else if (value.at(i) == QLatin1Char('"'))
            rich += QLatin1String("&quot;");
        else
            rich += value.at(i);
    }
    rich.squeeze();
    return rich;
}

/*!
    获取有效的格式符串，需要以%开头
    例如：%0.2fabc → 0.2f
*/

QByteArray FormDialog::byteArrayVsprintfFormat(const QByteArray &value)
{
    QByteArray result;
    const char *c = value.constData();
    for (;;) {
        if (*c == '\0')
            break;
        if (*c != '%')
            break;

        // Found '%'
        ++c;

        if (*c == '\0') {
            result.append('%'); // a % at the end of the string - treat as non-escape text
            break;
        }
        if (*c == '%') {
            result.append('%'); // %%
            break;
        }

        // Parse flag characters
        bool no_more_flags = false;
        do {
            switch (*c) {
                case '#': break;
                case '0': break;
                case '-': break;
                case ' ': break;
                case '+': break;
                case '\'': break;
                default: no_more_flags = true; break;
            }

            if (!no_more_flags)
            {
                result.append(*c);
                ++c;
            }
        } while (!no_more_flags);

        if (*c == '\0') {
            return QByteArray();
        }

        // Parse field width
        if (isdigit(*c)) {
            while (*c != '\0' && isdigit(*c))
                result.append(*c++);
        }
        else if (*c == '*') {
            result.append(*c);
            ++c;
        }

        if (*c == '\0') {
            return QByteArray();
        }

        // Parse precision
        if (*c == '.') {
            result.append(*c);
            ++c;
            if (isdigit(*c)) {
                while (*c != '\0' && isdigit(*c))
                    result.append(*c++);
            }
            else if (*c == '*') {
                result.append(*c);
                ++c;
            }
        }

        if (*c == '\0') {
            return QByteArray();
        }

        // Parse the length modifier
        switch (*c) {
            case 'h':
                result.append(*c);
                ++c;
                if (*c == 'h') {
                    result.append(*c);
                    ++c;
                }
                break;

            case 'l':
                result.append(*c);
                ++c;
                if (*c == 'l') {
                    result.append(*c);
                    ++c;
                }
                break;

            case 'L':
                result.append(*c);
                ++c;
                break;

            case 'j':
                result.append(*c);
                ++c;
                break;

            case 'z':
            case 'Z':
                result.append(*c);
                ++c;
                break;

            case 't':
                result.append(*c);
                ++c;
                break;

            default: break;
        }

        if (*c == '\0') {
            return QByteArray();
        }

        // Parse the conversion specifier and do the conversion
        switch (*c) {
            case 'd':
            case 'i': {
                result.append(*c);
                ++c;
                break;
            }
            case 'o':
            case 'u':
            case 'x':
            case 'X': {
                result.append(*c);
                ++c;
                break;
            }
            case 'E':
            case 'e':
            case 'F':
            case 'f':
            case 'G':
            case 'g':
            case 'A':
            case 'a': {
                result.append(*c);
                ++c;
                break;
            }
            case 'c': {
                result.append(*c);
                ++c;
                break;
            }
            case 's': {
                result.append(*c);
                ++c;
                break;
            }
            case 'p': {
                result.append(*c);
                ++c;
                break;
            }
            case 'n':
                result.append(*c);
                ++c;
                break;

            default: // bad escape, treat as non-escape text
                return QByteArray();
        }
        break;
    }
    return result;
}

void FormDialog::readSettings()
{
    QSettings settings;
    ui->comboBoxFileLeft->addItems(settings.value("fileListLeft").toStringList());
    ui->comboBoxFileRight->addItems(settings.value("fileListRight").toStringList());
    ui->comboBoxFileLeft->setCurrentIndex(settings.value("fileIdxLeft").toInt());
    ui->comboBoxFileRight->setCurrentIndex(settings.value("fileIdxRight").toInt());
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
    settings.setValue("fileIdxLeft", qMin(ui->comboBoxFileLeft->currentIndex(), 4));
    settings.setValue("fileIdxRight", qMin(ui->comboBoxFileRight->currentIndex(), 4));
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
    onTextEditCursorPositionChanged(DirectionType::Left);
}

void FormDialog::on_textEditRight_cursorPositionChanged()
{
    onTextEditCursorPositionChanged(DirectionType::Right);
}


bool FormDialog::CompareInfo::operator ==(const FormDialog::CompareInfo &s) const
{
    return value == s.value;
}

bool FormDialog::CompareInfo::operator !=(const FormDialog::CompareInfo &s) const
{
    return value != s.value;
}
