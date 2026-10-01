#include "factory.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace StronglyCoupled
{
    class MusicApp
    {
    public:
        MusicApp() = default;

        void play(const std::string &track_title)
        {
            // creation of service
            SpotifyService music_service("spotify_user", "rjdaslf276%2", 45);


            // using the service
            std::cout << "Using service " << music_service.service_name() << " for track " << track_title << "\n";

            if (!music_service.is_track_available(track_title))
            {
                std::cout << "Track not available!\n";
                return;
            }
            else
            {
                std::cout << "Track is available!\n";
                // proceed with playing the track

                std::expected<Track, std::error_code> track = music_service.get_track(track_title);

                if (track)
                {
                    std::cout << "Playing track: ";
                    for (const auto &note : *track)
                        std::cout << note << ".";
                    std::cout << "|\n";
                }
                else
                {
                    std::cout << "Error has occurred!!! " << track.error().message() << "!!!\n";
                }
            }
        }
    };
}

class MusicApp
{
    std::shared_ptr<MusicServiceCreator> music_service_creator_;

public:
    MusicApp(std::shared_ptr<MusicServiceCreator> music_service_creator)
        : music_service_creator_(music_service_creator)
    {
    }

    void play(const std::string &track_title)
    {
        // creation of service
        std::unique_ptr<MusicService> music_service = music_service_creator_->create_music_service();

        // using the service
        std::cout << "Using service " << music_service->service_name() << "!!\n";

        if (!music_service->is_track_available(track_title))
        {
            std::cout << "Track " << track_title << " not available!\n";
            return;
        }
        else
        {
            std::cout << "Track " << track_title << " is available!\n";
            // proceed with playing the track

            std::expected<Track, std::error_code> track = music_service->get_track(track_title);

            if (track)
            {
                std::cout << "Playing track: [ ";
                for (const auto &note : *track)
                    std::cout << note << ".";
                std::cout << " ]\n";
            }
            else
            {
                std::cout << "Error has occurred!!! " << track.error().message() << "\n";
            }
        }
    }
};

std::string parse_music_service_id_from_config()
{
    // for simplicity, return a hardcoded value
    // return "Tidal";
    // return "Spotify";
    return "Filesystem";
}

int main()
{
    auto music_service_creator = std::make_shared<TidalServiceCreator>("tidal_user", "KJH8324d&df");
    MusicApp app(music_service_creator);
    app.play("Would?");

    // TODO: parse the music service ID from the configuration and create the appropriate music service creator
}