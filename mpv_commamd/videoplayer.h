#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QProcess>
#include <QWidget>
#include <QMutex>
#include<QTime>
#include <QTimer>
#include <QTimerEvent>

#include <qlocalsocket.h>
class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(WId wid, QObject *parent = 0);
    ~VideoPlayer();
    int GetVolume() const
    {
        return volume;
    }
signals:
    void finished();
public slots:
    void refresh();
    void play(const QString &_filepath,const QTime &totaltime);
    void play(const QString &_filepath);
    void stop();
    void pause(bool is_pause);
    void quit();
    void setSpeed(double speed);
    void setCurrent(float current);
    void setVolume(int volume);
    void setVolumeEx(int volume);

    float getCurrent() const;
    float getPercent() const;
    float getTotal() const;
    QTime getCurrentTime()const;
    QString getFilepath() const;

    bool connectServer();

    bool execCommand(QString cmd, QByteArray ret);
    void connected_success();

    void timeout();
private slots:
    void dataRecieve();
    void on_finished();
private:
    QMutex  mutex;
    WId wid;
    QString filepath;
    QProcess *process;
    bool is_playing;
    float time_current;
    float time_length;
    float time_percent;

    QTime time_current_;
    QTime time_length_;
    float time_percent_;
    int volume = 50;
    //QLocalSocket *m_socket;
    int     _sockfd;
    QTime           _currentPalyTime;
    QTimer          _timer;
    QTime           _totalTime;

protected:
    //void timerEvent(QTimerEvent *enent);
};

#endif // VIDEOPLAYER_H
