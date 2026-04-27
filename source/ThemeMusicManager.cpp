#include "ThemeMusicManager.h"

#include <QMediaPlayer>
#include <QAudioOutput>

ThemeMusicManager::ThemeMusicManager(QObject* parent)
    : QObject{ parent }
    , m_currentIndex{ -1 }
    , m_audioPlayer{ new QMediaPlayer{ this } }
    , m_audioOutput{ new QAudioOutput{ this } }
    , m_playlist{ QUrl("qrc:/sounds/themes/main_theme_ICEGERGERT.mp3"),
                  QUrl("qrc:/sounds/themes/main_theme_DOOM1.mp3") }
    , m_playlistNames{ "ICEGERGERT - Выходите бесы",
                       "Mick Gordon - Cultist Base" }
{
    m_audioOutput->setVolume(0.02);
    m_audioPlayer->setAudioOutput(m_audioOutput);

    auto changeMusicLambda{
        [this](const QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia)
                playNext();
        }
    };

    connect(m_audioPlayer, &QMediaPlayer::mediaStatusChanged,
        changeMusicLambda);
}

void ThemeMusicManager::playNext()
{
    m_currentIndex = (m_currentIndex + 1) % m_playlist.size();
    m_audioPlayer->setSource(m_playlist[m_currentIndex]);
    m_audioPlayer->play();
    emit trackChangedSignal(m_playlistNames[m_currentIndex]);
}

void ThemeMusicManager::playPrevious()
{
    if (m_currentIndex > 0) --m_currentIndex;
    m_audioPlayer->setSource(m_playlist[m_currentIndex]);
    m_audioPlayer->play();
    emit trackChangedSignal(m_playlistNames[m_currentIndex]);
}