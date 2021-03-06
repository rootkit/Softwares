﻿#include <stdlib.h>

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QString>
#include <QtDebug>
#include <QTextCodec>
#include <QTimer>

#include "utils.h"
#include "vs_char_set.h"

QProcess *CUtils::m_pProgress = NULL;

CUtils::CUtils()
{

}

CUtils::~CUtils()
{

}

QString CUtils::floatToString(float data, int precision, char format)
{
    QString strValue;
    strValue = strValue.setNum(data, format, precision);
    return strValue;
}

void CUtils::changeADFile(const QString &strOldFileName, const QString &strNewFileName)
{
    QString strOldFilePath = getFileFullPath(strOldFileName);
    QString strNewFilePath = getFileFullPath(strNewFileName);
    if (QFile::exists(strOldFilePath)) {
        if (QFile::remove(strOldFilePath)) {
            qDebug() << "删除旧的广告文件成功: " << strOldFilePath;
        } else {
            qCritical() << "删除旧的广告文件失败: " << strOldFilePath;
        }
    }
    if (QFile::exists(strNewFilePath)) {
        /* 这里是把新文件改名为老文件名 */
        if (QFile::rename(strNewFilePath, strOldFilePath)) {
            qDebug() << "替换广告文件成功，新的文件为: " << strOldFilePath;
        } else {
            qCritical() << "替换广告文件失败!";
        }
    } else {
        qWarning() << "替换广告文件失败，因为文件不存在: " << strNewFilePath;
    }
}

void CUtils::insertData(float array[], int arrayLength, float newData)
{
    qDebug() << "插入前的数组数据为：";
    for (int i = 0; i < arrayLength; ++i)
        qDebug() << array[i];

    for (int i = 0; i < (arrayLength - 1); ++i) {
        array[i] = array[i + 1];
    }
    array[arrayLength - 1] = newData;

    qDebug() << "插入后的数组数据为：";
    for (int i = 0; i < arrayLength; ++i)
        qDebug() << array[i];
}

bool CUtils::testDataStable(float array[], int arrayLength, float deviation)
{
    for (int i = 0; i <arrayLength - 1; ++i) {
        if (qAbs((array[i] - array[i + 1])) > deviation)
            return false;
    }
    return true;
}

bool CUtils::execApp(const QString &appPath)
{
	if (appPath.isEmpty() || appPath.isNull()) {
		qCritical() << "execApp执行失败 要执行的程序路径 为空";
		return false;
	}

	if (!QFile::exists(appPath)) {
		qCritical() << "execApp执行失败 文件不存在 要执行的程序路径" << appPath;
		return false;
	}

	// 这里不能使用QProcess processApp; 如果使用QProcess processApp;
	// 这个接口执行完成的时候 拉起的程序直接退出了 只能用指针
	QProcess *processApp = new QProcess;
	// 值得注意的是 当程序路径中含有空格的时候 仅设置程序路径program
	// 是无法被识别的 解决方法就是使用带参模式执行
	// 如果还是不行 可将程序路径传给参数列表
	// processApp->start(strAppPath);
	// 目前windows上测试下面两种都是可以的 带空参 或者将程序路径传给参数列表
	// processApp->start(strAppPath, QStringList());
	processApp->start(appPath, QStringList(appPath));

	while (false == processApp->waitForStarted(5000)) {
		qCritical() << "execApp执行失败 在5秒内等待其start未成功";
		processApp->close();
		delete processApp;
		processApp = 0;
		return false;
    }
    m_pProgress = processApp;

	// 这里不能close 也不能delete 否则刚被拉起的程序会退出
	// processApp->close();
	// delete processApp;
	// processApp = 0;
	qDebug() << "execApp执行成功";
    return true;
}

/* 这里暂时只针对升级程序的退出 */
void CUtils::exitApp(const QString &appPath)
{
    if (appPath.isEmpty() || appPath.isNull()) {
        qCritical() << "exitApp执行失败,要退出的程序路径为空";
        return;
    }

    if (!QFile::exists(appPath)) {
        qCritical() << "exitApp执行失败,文件不存在:" << appPath;
        return;
    }

    if (m_pProgress) {
        m_pProgress->kill();
        m_pProgress->deleteLater();
        m_pProgress = NULL;
    }
}

bool CUtils::execSystemCommand(const QString &cmd)
{
    if (cmd.isEmpty() || cmd.isNull()) {
        return false;
    }

    // 这个路径一定要.bat结尾 否则无法执行
    QString batFilePath = QString("%1%2%3%4").arg(CUtils::getFileFullPath("Command"))
            .arg(QString("_"))
            .arg(QDateTime::currentDateTime().toTime_t())
            .arg(".bat");

    QFile fileBat(batFilePath);
    if (!fileBat.open(QIODevice::ReadWrite)) {
        return false;
    }

    // 当有中文时 systemCommand.length() != fileBat.write(systemCommand.toLocal8Bit())
    // 会出错
    if (cmd.toLocal8Bit().length() != fileBat.write(cmd.toLocal8Bit())) {
    // if (systemCommand.length() != fileBat.write(systemCommand.toLocal8Bit())) {
        fileBat.close();
        return false;
    }
    fileBat.close();

    // 直接用system命令的方式 会在运行的时候一闪而过一个cmd窗口
    // system("arp -d && ping -n 1 www.baidu.com && arp -a > c:/hwupdateRouterMac.tmp");
    // 用QT来执行本地.bat脚本 就不会出现一闪而过的一个cmd窗口
    // 如果batFilePath 路径中没有空格 直接用execute也可以
    // 但如果有空格 就只能用execApp
    // QProcess:execute会拉起完成
    // 目前是在execApp中也加了等待拉起完成
    // 如果不等待完成 后面的删除.bat文件可能导致.bat没有被执行
    // QProcess::execute(batFilePath);
	if (!QFile::exists(batFilePath)) {
		qCritical() << "execSystemCommand 要执行的bat文件不存在 要执行的程序路径" << batFilePath;
		return false;
	}

	QProcess *processApp = new QProcess;
	processApp->start(batFilePath, QStringList(batFilePath));

	while (false == processApp->waitForStarted(5000)) {
		qCritical() << "execApp执行失败 在5秒内等待其start未成功";
		processApp->close();
		delete processApp;
		processApp = 0;
		return false;
	}

	// 如果不加这个waitForFinished 如果执行的是个.bat文件
	// 而在执行完成之后 .bat文件马上被删除 这个时候这个.bat文件可能没有执行成功
	// 因为这个.bat文件被删除得太快
	// 所以加这个waitForFinished 保证.bat文件在删除前被执行
	// 注意这个finish是指这个拉起程序的QProcess完成
	// 而不是被拉起的程序完成(结束)
	// 就比如如果是拉起主程序 那么是空上拉起主程序的QProcess完成
	// 主程序仍然成功被拉起并继续执行
	while (false == processApp->waitForFinished(5000)) {
		qCritical() << "execApp执行失败 在5秒内等待其finish未成功";
		processApp->close();
		delete processApp;
		processApp = 0;
		return false;
	}

	qDebug() << "execSystemCommand执行成功";
	QFile::remove(batFilePath);
	return true;
}

QString CUtils::getFileFullPath(const QString &strFilename)
{
    QString strFileFullPath = QApplication::applicationDirPath() + "/" + strFilename;
    return strFileFullPath;
}

QString CUtils::calculateMD5ForString(const QString &strData)
{
    QString strReturn = "";
    if (!strData.isEmpty()) {
        /* 在进行hash的时候，需要将传入的字符串修改为utf-8格式，不能是local8bit，与服务器统一 */
        strReturn = QString(QCryptographicHash::hash(strData.toUtf8(),
                                                     QCryptographicHash::Md5).toHex());
    }
    return strReturn;
}

QString CUtils::calculateMD5ForFile(const QString &filePath)
{
    QString strReturn = "";
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QCryptographicHash hash(QCryptographicHash::Md5);
            /* 此方法需要验证是否支持大文件 */
            hash.addData(&file);
            strReturn = QString(hash.result().toHex());
            file.close();
        } else {
            qWarning() << "文件打开失败，无法进行MD5取值！";
        }
    }
    return strReturn;
}

void CUtils::disconnectTimerTimeout(QTimer *timer)
{
    if (timer && timer->isActive())
        timer->stop();
    timer->disconnect(SIGNAL(timeout()));
}

void CUtils::stopSpecificTimer(QTimer *timer)
{
    if (timer && timer->isActive())
        timer->stop();
}

#if 0
bool CUtils::encryptDataByDES(const QByteArray &inData, const QString &inStrKey, QByteArray &outData)
{
   if (inData.isEmpty() || inStrKey.isEmpty()) {
       qWarning() << "DES加密的原数据或密钥为空,无法进行加密!";
       return false;
   }

   if (8 != inStrKey.length()) {
       qWarning() << "密钥长度不够8个字符,无法进行正常加密!";
       return false;
   }

   if (0 != (inData.length() % 8)) {
       /* 加密的数据长度必须是8字节的整数倍,这里不做容错处理 */
       qWarning() << "加密的数据不合法,长度不等于8N,加密失败!";
       return false;
   }

   outData.resize(inData.length());
   outData.fill(0);
   /* 加密过程一次只能对8个字节进行 */
   for (int i = 0; i < inData.length() / 8; ++i) {
       encryptDES((unsigned char*)(inData.data() + i * 8),
                  (unsigned char*)(inStrKey.toLocal8Bit().data()),
                  (unsigned char*)outData.data() + i * 8);
   }

   return true;
}

bool CUtils::decryptDataByDES(const QByteArray &inData, const QString &inStrKey, QByteArray &outData)
{
    if (inData.isEmpty() || inStrKey.isEmpty()) {
        qWarning() << "DES解密的原数据或密钥为空,无法进行解密!";
        return false;
    }

    if (8 != inStrKey.length()) {
        qWarning() << "密钥长度不够8个字符,无法进行正常解密!";
        return false;
    }

    if (0 != (inData.length() % 8)) {
        /* 解密的数据长度必须是8字节的整数倍,这里不做容错处理 */
        qWarning() << "解密的数据不合法,长度不等于8N,解密失败!";
        return false;
    }

    outData.resize(inData.length());
    outData.fill(0);
    /* 解密过程一次只能对8个字节进行 */
    for (int i = 0; i < inData.length() / 8; ++i) {
        decryptDES((unsigned char*)(inData.data() + i * 8),
                   (unsigned char*)(inStrKey.toLocal8Bit().data()),
                   (unsigned char*)outData.data() + i * 8);
    }
    return true;
}
#endif


QString CUtils::getCurrentGatewayMac()
{
#ifdef Q_OS_UNIX
    QString strTempFilePath = getFileFullPath("gatewayMac.tmp");
    QString strSystemCMD = "route -n | grep UG | awk '{print $2}' > " + strTempFilePath;
    if (-1 == system(strSystemCMD.toLocal8Bit().data())) {
        qCritical() << "system系统命令执行出错!";
        return QString("");
    }
    QFile fileIPTmp(strTempFilePath);
    QString strGatewayIP;
    if (fileIPTmp.open(QIODevice::ReadOnly)) {
        strGatewayIP = fileIPTmp.readLine(1024);
        if (!strGatewayIP.isEmpty()) {
            QRegExp reg("\\s+");
            strGatewayIP = strGatewayIP.section(reg, 0, 0);
            qDebug() << "当前网关IP地址:" << strGatewayIP;
            fileIPTmp.close();
        } else {
            qCritical("获取路由器MAC失败, 读取到的网关IP为空!");
            return QString("");
        }
    } else {
        qCritical("获取当前网关IP操作失败!");
        return QString("");
    }

    // arp line
    // 192.168.1.1      0x1         0x2         08:10:78:40:1f:93     *        eth0
    QFile fileArp("/proc/net/arp");
    QString strGatewayMac, strLineRead;
    if (fileArp.open(QIODevice::ReadOnly)) {
        strLineRead = fileArp.readLine(1024);
        QRegExp reg("\\s+");
        while (!fileArp.atEnd()) {
            strLineRead = fileArp.readLine(1024);
            qDebug() << strLineRead;
            if (!strLineRead.isEmpty() && !strLineRead.isNull()) {
                qDebug() << strLineRead.section(reg, 0, 0);
                if (strGatewayIP == strLineRead.section(reg, 0, 0)) {
                    strGatewayMac = strLineRead.section(reg, 3, 3);
                    break;
                }
            }
        }
    } else {
        qCritical() << "获取当前网关MAC操作失败!";
        return QString("");
    }
    fileArp.close();

    if (strGatewayMac.isEmpty()) {
        qCritical("获取路由器MAC为空!");
        return QString("");
    }

    strGatewayMac.replace(strGatewayMac.length() - 1, 1, QChar(strGatewayMac.at(strGatewayMac.length()-1).toLatin1() - 1));
    qDebug() << "获取路由器MAC成功:" << strGatewayMac;
    return strGatewayMac;
#else
    // 获取网关IP
    QString routerMacTmpFile = getFileFullPath("hwupdateRouterMac.tmp");
    // 中文需要进行处理 不然写入文件会是乱码
    QString cmd = QString("%1%2%3%4%5%6%7%8%9%10%11")
        .arg("ipconfig | findstr /i ")
        .arg("\"默认网关\"")
        // 中文需要进行处理 不然写入文件会是乱码
        // .arg("\"默认网关\"")
        .arg(" > ")
        .arg("\"")
        .arg(routerMacTmpFile)
        .arg("\"")
        .arg(" & ")
        // 暂定用 &
        // .arg(" && ")
        .arg("ipconfig | findstr /i \"Gateway\" >> ")
        .arg("\"")
        .arg(routerMacTmpFile)
        .arg("\"");
    execSystemCommand(cmd);

    QFile fileIpTmp(routerMacTmpFile);
    QString gatewayIp, lineIpRead;
    QStringList fieldIp;
    if (fileIpTmp.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!fileIpTmp.atEnd()) {
            lineIpRead = QTextCodec::codecForLocale()->toUnicode(fileIpTmp.readLine(1024));
            if (!lineIpRead.isEmpty() && !lineIpRead.isNull()) {
                gatewayIp = lineIpRead.right(lineIpRead.length() - lineIpRead.indexOf(": ") - QString(": ").length());
                gatewayIp.remove(gatewayIp.length()-1, 1);
                if (!gatewayIp.isEmpty() && !gatewayIp.isNull()) {
                    // IP以.分隔 并有4个段
                    fieldIp = gatewayIp.split(".");
                    if (4 == fieldIp.size()) {
                        break;
                    } else {
                        // 置gatewayIp为null
                        gatewayIp.clear();
                    }
                }
            }
        }
    }

    fileIpTmp.close();
    fileIpTmp.remove();

    qDebug()<<"当前网关IP地址:"<<gatewayIp;
    if (gatewayIp.isEmpty() || gatewayIp.isNull()) {
        qCritical("获取路由器MAC失败 获取网关IP失败");
        return QString("");
    }

    // 通过网关IP 获取MAC
    cmd = QString("%1%2%3%4%5%6%7%8%9%10%11")
        .arg("arp -d")
        .arg(" & ")
        // ping 命令防止ARP表为空 即使不执行arp -d ARP表也可能为空
        .arg("ping -n 1 ")
        .arg("www.baidu.com")
        .arg(" & ")
        .arg("arp -a | findstr /i \"")
        .arg(gatewayIp)
        .arg("\" > ")
        .arg("\"")
        .arg(routerMacTmpFile)
        .arg("\"");
    execSystemCommand(cmd);

    QFile fileMacTmp(routerMacTmpFile);
    QString routerMac, lineRead;
    QStringList fieldMac;
    QRegExp reg("\\s+");
    if (fileMacTmp.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!fileMacTmp.atEnd()) {
            lineRead = QTextCodec::codecForLocale()->toUnicode(fileMacTmp.readLine(1024));
            if (!lineRead.isEmpty() && !lineRead.isNull()) {
                routerMac = lineRead.section(reg, 2, 2);
                if (!routerMac.isEmpty() && !routerMac.isNull()) {
                    // MAC以-分隔 并有6个段
                    fieldMac = routerMac.split("-");
                    if (6 == fieldMac.size()) {
                        break;
                    } else {
                        // 置routerMac为null
                        routerMac.clear();
                    }
                }
            }
        }
    }

    fileMacTmp.close();
    fileMacTmp.remove();

    if (routerMac.isEmpty() || routerMac.isNull()) {
        qCritical("获取路由器MAC失败");
        return QString("");
    }

    // mac长度17 一般这个判断可以不要
    routerMac = routerMac.left(17);
    routerMac = routerMac.replace("-", ":");
    routerMac.replace(routerMac.length()-1, 1,
                      QChar(routerMac.at(routerMac.length()-1).toLatin1()-1));
    qDebug("获取路由器MAC成功");
    return routerMac;
#endif
}

QString CUtils::getHostMac()
{
#ifdef Q_OS_UNIX
    QFile file("/sys/class/net/eth0/address");
    if (file.open(QIODevice::ReadOnly)) {
        QString strHostMac = file.readLine(1024);
        qDebug() << "当前主机MAC: " << strHostMac;
        file.close();
        return strHostMac;
    } else {
        qCritical("获取主机MAC地址失败!");
        return QString("");
    }
#else
    return QString("");
#endif
}
