#include "mpv_windows_player.h"
#include <QDebug>
#include <QTime>
// 唤醒函数
static void wakeup(void *ctx)
{
    // 此回调可从任何mpv线程调用（但也可以从调用mpv API的线程递归地返回）
    // 只是需要通知要唤醒的Qt GUI线程（以便它可以使用mpv_wait_event（）），并尽快返回
    mpv_windows_player *mvpPlayer = (mpv_windows_player *)ctx;
    emit mvpPlayer->mpv_events();
}
mpv_windows_player::mpv_windows_player(QObject *parent) : QObject(parent)
{

}

mpv_windows_player::~mpv_windows_player()
{
    if (mpv)
    {
        mpv_terminate_destroy(mpv);
        //mpv = nullptr;
    }

}

bool mpv_windows_player::play(WId wid, QString filename)
{
    if(!mpv)
    {
        create_mvpPlayer(wid);
    }
    if (mpv)
    {

        const QByteArray c_filename = filename.toUtf8();
        const char *args[] = {"loadfile", c_filename.data(), NULL};
        mpv_command_async(mpv, 0, args);
        pause(false);
        is_playing = true;
        return true;
    }
    else
    {
        is_playing = false;
    }
    return false;
}

bool mpv_windows_player::pause(bool flag)
{
    if(flag)
    {
        is_playing = true;
        setProperty("pause", "yes");

    }
    else
    {
        is_playing = false;
        setProperty("pause", "no");

    }

}

bool mpv_windows_player::stop()
{
    setProperty( "stop","true");
}

bool mpv_windows_player::quit()
{

}

void mpv_windows_player::create_mvpPlayer(WId wid)
{
    // 创建mpv实例
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("can't create mpv instance");
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    mpv_set_option_string(mpv, "input-vo-keyboard", "yes");
    mpv_set_option_string(mpv,  "loop-file", "inf");

    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
//    mpv_observe_property(mpv, 0, "track-list", MPV_FORMAT_NODE);
//    mpv_observe_property(mpv, 0, "chapter-list", MPV_FORMAT_NODE);



    mpv_request_log_messages(mpv, "info");

    connect(this, &mpv_windows_player::mpv_events, this, &mpv_windows_player::on_mpv_events,
            Qt::QueuedConnection);
    mpv_set_wakeup_callback(mpv, wakeup, this);

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("mpv failed to initialize");
}

void mpv_windows_player::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id)
    {
        case MPV_EVENT_PROPERTY_CHANGE:
        {
            mpv_event_property *prop = (mpv_event_property *)event->data;

            if (strcmp(prop->name, "time-pos") == 0)
            {
                if (prop->format == MPV_FORMAT_DOUBLE)
                {
                    double time = *(double *)prop->data;
                    emit changeCurrent(time* 1000.0);
                }
                else if (prop->format == MPV_FORMAT_NONE)
                {
                    //该属性不可用，可能意味着播放已停止
                    qDebug()<<"该属性不可用，可能意味着播放已停止"<<endl;
                }
            }
            else if (strcmp(prop->name, "duration") == 0)
            {
                //文件总长度
                if (prop->format == MPV_FORMAT_DOUBLE)
                {
                    double time = *(double *)prop->data;
                    emit changeDuration(time* 1000.0);
                }
            }
            break;
        }
        case MPV_EVENT_VIDEO_RECONFIG:
        {
            // 检索新的视频大小
            int64_t w, h;
            if (mpv_get_property(mpv, "dwidth", MPV_FORMAT_INT64, &w) >= 0 &&
                mpv_get_property(mpv, "dheight", MPV_FORMAT_INT64, &h) >= 0 &&
                w > 0 && h > 0)
            {
                // 请注意，MPV_EVENT_VIDEO_RECONFIG事件不一定表示要调整大小，如果尺寸确实发生了变化，
                // 您应该检查一下视频。如果视频不合适，mpv本身会将视频缩放，放大到container大小
//                QStringList ss;
//                ss << "Reconfig: " << w << " " << h;
//                //statusBar()->showMessage(QString::fromStdString(ss.str()));
//                qDebug()<<"ss"<< ss.join(" ")<<endl;
            }
            break;
        }
        case MPV_EVENT_LOG_MESSAGE:
        {
//            struct mpv_event_log_message *msg = (struct mpv_event_log_message *)event->data;
//            QStringList ss;
//            ss << "[" << msg->prefix << "] " << msg->level << ": " << msg->text;
//            //append_log(QString::fromStdString(ss.str()));
//            qDebug()<<"ss"<<ss.join(" ")<<endl;
            break;
        }
        case MPV_EVENT_SHUTDOWN:
        {
            if(mpv)
            {
                mpv_terminate_destroy(mpv);
                //mpv = nullptr;
            }

            break;
        }
        case MPV_EVENT_END_FILE:
        {
            qDebug()<<"MPV_EVENT_END_FILE";

        }
        default: ;
    }
}

void mpv_windows_player::command_variant(const QVariant &args)
{
    if(!mpv)
    {
        return ;
    }
    node_builder node(args);
    mpv_node res;
    if (mpv_command_node(mpv, node.node(), &res) < 0) {
        return;
    }
    node_autofree f(&res);
}

void mpv_windows_player::setProperty(const QString &name, const QString &value)
{
    if(mpv)
    {
        mpv_set_option_string(mpv, name.toLatin1().data(), value.toLatin1().data());
    }

}

void mpv_windows_player::on_mpv_events()
{
    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        handle_mpv_event(event);
    }
}

bool mpv_windows_player::getIs_playing() const
{
    return is_playing;
}

void mpv_windows_player::setIs_playing(const bool &value)
{
    is_playing = value;
}

float mpv_windows_player::speed() const
{
    return m_speed;
}

void mpv_windows_player::setSpeed(float speed)
{
    m_speed = speed;
    setProperty("speed",QString::number(speed));
}

double mpv_windows_player::current() const
{
    return m_current;
}

void mpv_windows_player::setCurrent(double current)
{
    m_current = current;
    QTime time(0,0);
    time = time.addMSecs(current);
    QStringList list;
    list<<"seek";
    list<<time.toString("hh:mm:ss.zzz");
    list<<QString("absolute");
    //qDebug()<<list.join(" ")<<endl;
    command_variant(list);
}

int mpv_windows_player::volume() const
{
    return m_volume;
}

void mpv_windows_player::setVolume(int volume)
{
    m_volume = volume;
    setProperty("volume",QString::number(volume));
}
