#include "factory.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>

namespace StronglyCoupled
{
    class MusicApp
    {
    public:
        MusicApp() = default;

        void play(const std::string& track_title)
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
                    for (const auto& note : *track)
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
} // namespace StronglyCoupled

class MusicApp
{
    std::shared_ptr<MusicServiceCreator> music_service_creator_;

public:
    MusicApp(std::shared_ptr<MusicServiceCreator> music_service_creator)
        : music_service_creator_(music_service_creator)
    {
    }

    void play(const std::string& track_title)
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
                for (const auto& note : *track)
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

namespace ModernCpp
{
    class MusicApp
    {
        MusicServiceCreator music_service_creator_;

    public:
        MusicApp(MusicServiceCreator music_service_creator)
            : music_service_creator_(music_service_creator)
        {
        }

        void play(const std::string& track_title)
        {
            // creation of service
            std::unique_ptr<MusicService> music_service = music_service_creator_();

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
                    for (const auto& note : *track)
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

} // namespace ModernCpp

std::string parse_music_service_id_from_config()
{
    // for simplicity, return a hardcoded value
    // return "Tidal";
    // return "Spotify";
    return "YouTubeMusic";
}

namespace ModernCpp
{
    class MusicServiceFactory
    {
        std::unordered_map<std::string, MusicServiceCreator> creators_;

    public:
        void register_creator(const std::string& id, MusicServiceCreator creator)
        {
            creators_[id] = creator;
        }

        MusicServiceCreator get_creator(const std::string& id)
        {
            if (creators_.find(id) != creators_.end())
            {
                return creators_[id];
            }
            return nullptr;
        }
        std::unique_ptr<MusicService> create(const std::string& id)
        {
            auto creator = get_creator(id);
            if (creator)
            {
                // Assuming each creator has a clone method to create a new instance
                return creator();
            }
            return nullptr;
        }
    };
} // namespace ModernCpp

struct CustomContainer
{
    std::vector<std::string> words;

    CustomContainer(std::initializer_list<std::string> init_list)
        : words(init_list)
    {
    }

    auto begin() { return words.begin(); }
    auto end() { return words.end(); }
};

void most_freqently_used_factory_method()
{
    CustomContainer words = {"Tidal", "Spotify", "YouTubeMusic", "Filesystem"};

    // for(const auto& word : words)
    // {
    //     std::cout << word << "\n";
    // }

    for(std::input_iterator auto it = words.begin(); it != words.end(); ++it)
    {
        std::cout << *it << "\n";
    }
}

int main()
{   
    ModernCpp::MusicServiceFactory music_factory;
    music_factory.register_creator("Tidal", ModernCpp::TidalServiceCreator("tidal_user", "KJH8324d&df"));
    music_factory.register_creator("Filesystem", ModernCpp::FsMusicServiceCreator("~/music"));
    music_factory.register_creator("Spotify", ModernCpp::SpotifyServiceCreator("spotify_user", "ABC123xyz", 45));
    music_factory.register_creator("YouTubeMusic", ModernCpp::YouTubeMusicServiceCreator("youtube_user", "XYZ987abc"));

    auto music_service_creator = music_factory.get_creator(parse_music_service_id_from_config());

    ModernCpp::MusicApp app(music_service_creator);
    app.play("Would?");
}