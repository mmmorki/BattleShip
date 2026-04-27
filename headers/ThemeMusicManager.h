#ifndef THEMEMUSICMANGER_H
#define THEMEMUSICMANGER_H

#include <QObject>
#include <QUrl>

class QMediaPlayer;
class QAudioOutput;

class ThemeMusicManager final : public QObject
{
    Q_OBJECT

public: signals:
    void trackChangedSignal(const QString& trackName);

public:
    explicit ThemeMusicManager(QObject* parent = nullptr);

    void playNext();

    void playPrevious();

private:
    qsizetype m_currentIndex{};
    QMediaPlayer* m_audioPlayer{ nullptr };
    QAudioOutput* m_audioOutput{ nullptr };
    QList<QUrl> m_playlist{};
    QList<QString> m_playlistNames{};
};

#endif //THEMEMUSICMANGER_H