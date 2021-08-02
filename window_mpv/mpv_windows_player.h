#ifndef MPV_WINDOWS_PLAYER_H
#define MPV_WINDOWS_PLAYER_H

#include <QObject>
#include <client.h>
#include <cstring>
#include <QVariant>
#include <QWidget>
struct node_builder {
    node_builder(const QVariant &v) {
        set(&node_, v);
    }
    ~node_builder() {
        free_node(&node_);
    }
    mpv_node *node() {
        return &node_;
    }
private:
    Q_DISABLE_COPY(node_builder)
    mpv_node node_;
    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num) {
        dst->format = is_map ? MPV_FORMAT_NODE_MAP : MPV_FORMAT_NODE_ARRAY;
        mpv_node_list *list = new mpv_node_list();
        dst->u.list = list;
        if (!list) {
            goto err;
        }
        list->values = new mpv_node[num]();
        if (!list->values) {
            goto err;
        }
        if (is_map) {
            list->keys = new char *[num]();
            if (!list->keys) {
                goto err;
            }
        }
        return list;
    err:
        free_node(dst);
        return NULL;
    }
    char *dup_qstring(const QString &s) {
        QByteArray b = s.toUtf8();
        char *r = new char[b.size() + 1];
        if (r) {
            std::memcpy(r, b.data(), b.size() + 1);
        }
        return r;
    }
    bool test_type(const QVariant &v, QMetaType::Type t) {
        // The Qt docs say: "Although this function is declared as returning
        // "QVariant::Type(obsolete), the return value should be interpreted
        // as QMetaType::Type."
        // So a cast really seems to be needed to avoid warnings (urgh).
        return static_cast<int>(v.type()) == static_cast<int>(t);
    }
    void set(mpv_node *dst, const QVariant &src) {
        if (test_type(src, QMetaType::QString)) {
            dst->format = MPV_FORMAT_STRING;
            dst->u.string = dup_qstring(src.toString());
            if (!dst->u.string) {
                goto fail;
            }
        } else if (test_type(src, QMetaType::Bool)) {
            dst->format = MPV_FORMAT_FLAG;
            dst->u.flag = src.toBool() ? 1 : 0;
        } else if (test_type(src, QMetaType::Int) ||
                   test_type(src, QMetaType::LongLong) ||
                   test_type(src, QMetaType::UInt) ||
                   test_type(src, QMetaType::ULongLong)) {
            dst->format = MPV_FORMAT_INT64;
            dst->u.int64 = src.toLongLong();
        } else if (test_type(src, QMetaType::Double)) {
            dst->format = MPV_FORMAT_DOUBLE;
            dst->u.double_ = src.toDouble();
        } else if (src.canConvert<QVariantList>()) {
            QVariantList qlist = src.toList();
            mpv_node_list *list = create_list(dst, false, qlist.size());
            if (!list) {
                goto fail;
            }
            list->num = qlist.size();
            for (int n = 0; n < qlist.size(); n++) {
                set(&list->values[n], qlist[n]);
            }
        } else if (src.canConvert<QVariantMap>()) {
            QVariantMap qmap = src.toMap();
            mpv_node_list *list = create_list(dst, true, qmap.size());
            if (!list) {
                goto fail;
            }
            list->num = qmap.size();
            for (int n = 0; n < qmap.size(); n++) {
                list->keys[n] = dup_qstring(qmap.keys()[n]);
                if (!list->keys[n]) {
                    free_node(dst);
                    goto fail;
                }
                set(&list->values[n], qmap.values()[n]);
            }
        } else {
            goto fail;
        }
        return;
    fail:
        dst->format = MPV_FORMAT_NONE;
    }
    void free_node(mpv_node *dst) {
        switch (dst->format) {
            case MPV_FORMAT_STRING:
                delete[] dst->u.string;
                break;
            case MPV_FORMAT_NODE_ARRAY:
            case MPV_FORMAT_NODE_MAP: {
                mpv_node_list *list = dst->u.list;
                if (list) {
                    for (int n = 0; n < list->num; n++) {
                        if (list->keys) {
                            delete[] list->keys[n];
                        }
                        if (list->values) {
                            free_node(&list->values[n]);
                        }
                    }
                    delete[] list->keys;
                    delete[] list->values;
                }
                delete list;
                break;
            }
            default:
                ;
        }
        dst->format = MPV_FORMAT_NONE;
    }
};

struct node_autofree {
    mpv_node *ptr;
    node_autofree(mpv_node *a_ptr) : ptr(a_ptr) {}
    ~node_autofree() {
        mpv_free_node_contents(ptr);
    }
};

class mpv_windows_player : public QObject
{
    Q_OBJECT
public:
    explicit mpv_windows_player(QObject *parent = nullptr);
    ~mpv_windows_player();


    bool play(WId wid,QString filename);
    bool pause(bool flag);
    bool stop();
    bool quit();



    int volume() const;
    void setVolume(int volume);

    double current() const;
    void setCurrent(double current);

    float speed() const;
    void setSpeed(float speed);


    bool getIs_playing() const;
    void setIs_playing(const bool &value);

private slots:
    void create_mvpPlayer(WId wid); // 创建mvpPlayer
    void handle_mpv_event(mpv_event *event); // 处理mpv事件
    void command_variant( const QVariant &args);  //命令
    void setProperty(const QString &name, const QString &value);//属性
    void on_mpv_events(); // 这个槽函数由 wakeup()调用（通过mpv_events信号）

signals:
    void mpv_events();
    void changeCurrent(double);
    void changeDuration(double);
private:
    mpv_handle *mpv = nullptr;


    QString m_file;
    int m_volume = 50;
    double m_current = 0.0;
    float m_speed = 1.0;
    double m_duration = 0.0;
    bool is_playing = false;


};

#endif // MPV_WINDOWS_PLAYER_H
