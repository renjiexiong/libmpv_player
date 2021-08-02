#include "videoplayer.h"
#include "userinfomanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <QDebug>

#define UNIXSTR_PATH "/tmp/mpvsocket"
#define LISTENQ 5
#define BUFFER_SIZE 256

VideoPlayer::VideoPlayer(WId _wid, QObject *parent) : QObject(parent),wid(_wid), process(nullptr), is_playing(false)
{
   // time_current = 0.0f;
   // time_length = 0.0f;
    time_percent = 0.0f;
    volume = 50;
    _currentPalyTime = QTime(0,0);
    _timer.setInterval(99);
    _totalTime = QTime(0,0);
    connect(&_timer,&QTimer::timeout,this,&VideoPlayer::timeout,Qt::QueuedConnection);
}

VideoPlayer::~VideoPlayer()
{
    if(process != nullptr )
    {
        _timer.stop();
        process->write("quit\n");
        process->close();
        process->kill();
        delete process;
        process = nullptr;
    }
}

void VideoPlayer::refresh()
{
    mutex.lock();
    if(process != nullptr && is_playing)
    {
        process->write("get_time_length\n");
        process->write("get_time_pos\n");
        process->write("get_percent_pos\n");
    }
    mutex.unlock();
}

void VideoPlayer::play(const QString &_filepath, const QTime &totaltime)
{
    _totalTime = totaltime;
    play(_filepath);
}

void VideoPlayer::play(const QString &_filepath)
{
    //QString ct = m_currentPlayTime.toString("hh:mm:ss.zzz");
    {
        mutex.lock();
        if(process != nullptr )
        {
            _timer.stop();
            _currentPalyTime = QTime(0,0);
            process->write("quit\n");

//            QByteArray ba;
//            QString cmd = QString("quit")+ "\n";
//            execCommand(cmd,ba);
            disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
            //process->close();
            process->kill();
            delete process;
            process = nullptr;
            is_playing = false;
        }
        mutex.unlock();
    }

    time_current = 0.0f;
    time_length = 0.0f;
    time_percent = 0.0f;
    _currentPalyTime = QTime(0,0);

    filepath = _filepath;
    QFileInfo playfile(filepath);
    if (!playfile.isFile())
    {
        return;
    }
#if     defined(Q_OS_WIN)
    QStringList args;
    args << "-wid" << QString::number(wid);
    args << "-slave";
    args << "-quiet";
    args  << "-loop";
    args  << "0";
    args << "-nokeepaspect";
    args << "-osdlevel";
    args << "0";
    args << "-framedrop";
    args << "-colorkey";
    args << "0x0";
    args << "-mc";
    args << "0";
    args << "-subcp";
    args << "cp936,UTF-8,UTF-16";
    args << "-subfont";
    args << "C:\\Windows\\Fonts\\simhei.ttf";
    args << "-autosync";
    args << "0";
    args << "-mc";
    args << "2.0";

    args << filepath;
    //qDebug() << args;
#elif       defined(Q_OS_MACOS)

    QStringList args;
    args << QString("--wid=%1").arg(wid);
    args<<"--audio-display=no";
    args<<"--no-video";
    args << "--input-ipc-server=/tmp/mpvsocket";
    args << "--no-config";
    args << "--no-msg-color";
    args << "--no-fs";
    args << "--no-input-default-bindings";
    args << "--input-vo-keyboard=no";
    args << "--no-input-cursor";
    args << "--cursor-autohide=no";
    //args<<"--loop-file=inf";
    args<<"--loop-playlist=inf";
    //args <<"--volume=100";
    args <<QString("--volume=%1").arg(QString::number(volume*2));
    args<<filepath;

#endif

    mutex.lock();
    process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this,&VideoPlayer::on_finished);
    //process->setWorkingDirectory("D:/workspace/window_audio_converter/build-AudioEditor-Desktop_Qt_5_7_1_MinGW_32bit-Debug/debug");
    //process->setWorkingDirectory("./mplayer");
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
    QString str = UserInfoManager::Instance()->GetPlayerPath();
    process->start(UserInfoManager::Instance()->GetPlayerPath(), args);
    qDebug()<<"exe:"<<UserInfoManager::Instance()->GetPlayerPath()<< "args:"<<args <<endl;
    _timer.start();
    connectServer();
    is_playing = true;
    mutex.unlock();
}

void VideoPlayer::stop()
{
    mutex.lock();
    if(is_playing)
    {
        QByteArray ba;
        _timer.stop();
        QString cmd = QString("set pause yes")+ "\n";
        execCommand(cmd,ba);
      // process->write(QString("{\"command\":[\"set_property\",\"pause\",true]}").toUtf8() + "\n");
        disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        is_playing = false;
    }
    mutex.unlock();
}

void VideoPlayer::quit()
{

    mutex.lock();
    if(process != nullptr)
    {
        process->close();
        disconnect(process);
        delete process;
        process = nullptr;
        is_playing = false;
        filepath = "";
        _timer.stop();
        _currentPalyTime = QTime(0,0);
    }
    mutex.unlock();
}

void VideoPlayer::pause(bool is_pause)
{
    if(!is_pause)
    {
        _timer.stop();
    }
    else
    {
        _timer.start();
    }
    mutex.lock();
    if(process == nullptr)
    {
        mutex.unlock();
        return;
    }
    is_playing = !is_playing;
    if(is_playing)
    {
        setVolumeEx(volume);
    }

    QByteArray ba;
    QString cmd = QString("set pause ") + (is_pause?"no":"yes") + "\n";
    execCommand(cmd,ba);
   // process->write(QString("set pause %1").arg(is_playing ? "yes" : "no").toLocal8Bit() +"\n");
    //process->write("set pause yes \n");

    if (!is_pause)
    {
        disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
    }
    else
    {
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
    }
    mutex.unlock();
    if (is_pause)
    {
        dataRecieve();
    }
}

void VideoPlayer::setSpeed(double speed)
{
    mutex.lock();
    if(speed > 0 && process != nullptr)
    {

        process->write(QString("speed_set " + QString::number(speed) + " 2\n").toLocal8Bit());
    }
    mutex.unlock();
}

void VideoPlayer::setCurrent(float current)
{
    {
        mutex.lock();
        if(process == nullptr)
        {
            mutex.unlock();
            return;
        }
        time_current = current;
        QTime temptime(0,0);
        _currentPalyTime = temptime.addMSecs(current);
        disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        mutex.unlock();
    }
    dataRecieve();
    {
        mutex.lock();
        QByteArray ba;
        _timer.start();
       QString cmd = QString("set pause no")+ "\n";
        execCommand(cmd,ba);
        cmd = QString("seek " + _currentPalyTime.toString("hh:mm:ss.zzz")  +" absolute" +"\n");
        execCommand(cmd,ba);

        //QString cmd = QString("set pause ") + (is_pause?"no":"yes") + "\n";

       //process->write(QString("seek " + QString::number(current) +" absolute"+ "\n"));
        is_playing = true;
        mutex.unlock();
    }

}

void VideoPlayer::setVolume(int _volume)
{
    mutex.lock();
    if(_volume >= 0 && process != nullptr)
    {
        volume = _volume;
        if(is_playing)
        {
#if defined(Q_OS_WIN)
            process->write(QString("volume " + QString::number(volume) + " \r").toUtf8());
#elif   defined(Q_OS_MACOS)
            QByteArray ba;
            QString cmd = QString("set volume %1").arg(QString::number(volume*2)) +"\n";
            execCommand(cmd,ba);
#endif
        }
    }
    mutex.unlock();
}

void VideoPlayer::setVolumeEx(int _volume)
{
    if(_volume >= 0 && process != nullptr)
    {
        volume = _volume;
        if(is_playing)
        {
           // process->write(QString("volume " + QString::number(volume) + " 2\n").toUtf8());
           //process->write(QString("set volume " + QString::number(volume)).toLocal8Bit() +"\n");
            QByteArray ba;
            QString cmd = QString("set volume %1").arg(QString::number(volume*2)) +"\n";
            execCommand(cmd,ba);
        }
    }
}

void VideoPlayer::dataRecieve()
{
    mutex.lock();
    if(process != nullptr)
    {
        while (process->canReadLine())
        {
            QByteArray b = process->readLine();
            b.replace(QByteArray("\n"), QByteArray(""));
            QString s(b);
            //qDebug() << __FUNCTION__ << s;
//#if     defined(Q_OS_WIN)
            if (b.startsWith("ANS_TIME_POSITION"))
            {
                time_current = s.mid(18).toFloat();
            }
            else if (b.startsWith("ANS_LENGTH"))
            {
                time_length = s.mid(11).toFloat();
            }
            else if(b.startsWith("ANS_PERCENT_POSITION"))
            {
                time_percent = s.mid(21).toFloat();
            }
            else if(b.startsWith("Starting playback..."))
            {
                setVolumeEx(volume);
            }
//#elif       defined (Q_OS_MACOS)
//            QStringList strlist = s.split(" ");
//            if(s.startsWith("(Paused)"))
//            {
//                time_current_ = QTime::fromString(strlist.at(2));
//                time_length_ = QTime::fromString(strlist.at(4));
//                time_percent_ = strlist.at(5).toFloat();

//            }
//            else if(s.startsWith("AV:"))
//            {
//                time_current_ = QTime::fromString(strlist.at(1));
//                time_length_ = QTime::fromString(strlist.at(3));
//                time_percent_ = strlist.at(5).toFloat();
//            }
//#endif
        }
    }
    mutex.unlock();
}

void VideoPlayer::on_finished()
{
    if(process->exitCode() == 0)
    {
       play(filepath);
    }
}

//void VideoPlayer::timerEvent(QTimerEvent *event)
//{
//    int timerid = event->timerId();
//    if(timerid == m_icurrenttime_timer_id)
//    {
//        killTimer(m_icurrenttime_timer_id);
//        m_icurrenttime_timer_id = -1;
//        m_icurrenttime_timer_id = startTimer(1);
//        m_currentPlayTime.addMSecs(1);
//    }

//}

float VideoPlayer::getCurrent() const
{
    return time_current;

}

float VideoPlayer::getPercent() const
{
    return time_percent;
}

float VideoPlayer::getTotal() const
{
    return time_length;
}

QTime VideoPlayer::getCurrentTime() const
{
    return _currentPalyTime;
}

QString VideoPlayer::getFilepath() const
{
    return filepath;
}

bool VideoPlayer::connectServer()
{
    struct sockaddr_un servaddr;
    _sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    struct timeval      tv;
    /* 设置套接字读写超时 */
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(_sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof (tv));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);
    ///链接unix domian socket
    int result = -1;
    do{
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        result = ::connect(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }while(result <0);
    return true;
}

bool VideoPlayer::execCommand(QString cmd, QByteArray ret)
{
    qDebug()<<"cmd:"<<cmd;
    QByteArray ba = cmd.toLocal8Bit();
    int len = write(_sockfd, ba.data(), (unsigned long)ba.length());
    qDebug()<<"cmd len:"<<len;

}

void VideoPlayer::connected_success()
{
    qDebug()<<"connect success";
}

void VideoPlayer::timeout()
{
   if(_totalTime != QTime(0,0) && _currentPalyTime >=_totalTime)
   {
       _timer.stop();
       play(filepath);
       //_currentPalyTime = QTime(0,0);
   }
   else
   {
       _currentPalyTime = _currentPalyTime.addMSecs(99);
   }

}

