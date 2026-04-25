#ifndef THEMEMUSICMANGER_H
#define THEMEMUSICMANGER_H

#include <QObject>

class QMediaPlayer;
class QAudioOutput;

class ThemeMusicManager final : public QObject
{
public:
    explicit ThemeMusicManager(QObject* parent = nullptr);

    void playNext();

private:
    qsizetype m_currentIndex{};
    QMediaPlayer* m_audioPlayer{ nullptr };
    QAudioOutput* m_audioOutput{ nullptr };
    QList<QUrl> m_playlist{};
};

#endif //THEMEMUSICMANGER_H