﻿#include <QDebug>
#include <QDir>

#include "cure_log.h"
#include "cure_utils.h"
#include "msvs_charset.h"

#define OUTPUT_DEBUG_LEVEL QtDebugMsg

static QMutex g_mutexLog;

static const QString g_logFileDir                = "log";
static const QString g_logFileName               = "CureOffice.log";
static const QString g_logDateFormat             = "yyyy-MM-dd hh:mm:ss:zzz";
static const QString g_logFileNameSeperator      = "/";
static const QString g_logFunctionNameSeperator  = "::";
static const QString g_logFunctionParamSeperator = "(";

static const int g_nFileNameLength              = -35;
static const int g_nFunctionNameLength          = -35;
static const int g_nLineNumberLength            = -3;
static const int g_nDebugLevelLength            = -9;
static const unsigned int g_unMaxLogFileSize    = 500 * 1024 * 1024; // 500M

CMyLog::CMyLog()
{
}

CMyLog::~CMyLog()
{

}

void CMyLog::initLog()
{
//      qInstallMessageHandler(CMyLog::messageHandler);

    QString strFilePath = CUtils::getFileFullPath(g_logFileDir + "/" + g_logFileName);
    QString strFileDir = CUtils::getFileFullPath(g_logFileDir);
    QDir dir(strFileDir);
    if (!dir.exists()) {
        dir.mkpath(strFileDir);
    }

    if (QFile::exists(strFilePath)) {
        QFile file(strFilePath);
        if (file.size() >= g_unMaxLogFileSize) {
            QFile::remove(strFilePath + ".bak");
            QFile::rename(strFilePath, strFilePath + ".bak");
        }
    } else {
        qDebug() << "Log file is not exist!";
    }
    qDebug() << "";
    qDebug() << "*************************************************";
    qDebug() << QDateTime::currentDateTime().toString();
    qDebug() << "*************************************************";
}

void CMyLog::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (type < OUTPUT_DEBUG_LEVEL)
        return;

    g_mutexLog.lock();

    QString strDebugLevel;
    switch (type) {
    case QtDebugMsg:
        strDebugLevel = "DEBUG:";
        break;
    case QtWarningMsg:
        strDebugLevel = "Warning:";
        break;
    case QtCriticalMsg:
        strDebugLevel = "Critical:";
        break;
    case QtFatalMsg:
        strDebugLevel = "Fatal:";
        break;
    default:
        break;
    }

    QString strFileNamePath = QString(context.file);

    /* 有可能是系统报的消息 */
    if (strFileNamePath.length() < 1)
        strFileNamePath = "System Message";
    QString strFunctionNamePath = QString(context.function);
    if (strFunctionNamePath.length() < 1)
        strFunctionNamePath = "System Message";
    QString strLogTime = QString(QDateTime::currentDateTime().toString(g_logDateFormat));

    QString strMesssage;

    strMesssage = QString("[%1][%2] %3").arg(strLogTime).arg(strDebugLevel, g_nDebugLevelLength).arg(msg);

    QString strFilePath = CUtils::getFileFullPath(g_logFileDir + "/" + g_logFileName);
    /* 将日志内容写入文件 */
    QFile logFile(strFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream textStream(&logFile);
        textStream << strMesssage << "\r\n";
        logFile.flush();
        logFile.close();
    }

    g_mutexLog.unlock();
}

void CMyLog::clearLogFile()
{
}

QString CMyLog::getFileName(const QString &strFileNamePath)
{
    QString strFileName = "";
    if (!strFileNamePath.isEmpty())
        strFileName = strFileNamePath.section(g_logFileNameSeperator, -1 , -1);
    return strFileName;
}

QString CMyLog::getFunctionName(const QString &strFunctionNamePath)
{
    QString strFunctionName = "";
    if (!strFunctionNamePath.isEmpty()) {
        strFunctionName = strFunctionNamePath.section(g_logFunctionNameSeperator, -1, -1);
        strFunctionName = strFunctionName.section(g_logFunctionParamSeperator, 0, 0);
    }
    return strFunctionName;
}

QString CMyLog::getAppPathName()
{
    QString strFullAppPath = "";
    strFullAppPath = CUtils::getFileFullPath("");
    if (!strFullAppPath.isEmpty()) {
        strFullAppPath = strFullAppPath.section("/", -2, -2);
    }
    return strFullAppPath;
}
