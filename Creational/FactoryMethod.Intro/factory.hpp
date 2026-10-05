#ifndef FACTORY_HPP_
#define FACTORY_HPP_

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <expected>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using Track = std::vector<uint8_t>;

// "Product"
class MusicService
{
public:
    virtual std::string service_name() const = 0;
    virtual bool is_track_available(const std::string &title) = 0;
    virtual std::expected<Track, std::error_code> get_track(const std::string &title) = 0;
    virtual ~MusicService() = default;
};

// "ConcreteProductA"
class TidalService : public MusicService
{
public:
    TidalService(const std::string &user_name, const std::string &secret)
    {
        std::cout << "Creating TidalService...\n";
    }

    std::string service_name() const override
    {
        return "TidalService";
    }

    bool is_track_available(const std::string &title) override
    {
        return !title.empty();
    }

    std::expected<Track, std::error_code> get_track(const std::string &title) override
    {
        return Track(title.begin(), title.end());
    }
};

// "ConcreteProductB"
class SpotifyService : public MusicService
{
public:
    SpotifyService(const std::string &user_name, const std::string &secret, int timeout = 30)
    {
        std::cout << "Creating SpotifyService...\n";
    }

    std::string service_name() const override
    {
        return "SpotifyService";
    }

    bool is_track_available(const std::string &title) override
    {
        return !title.empty();
    }

    std::expected<Track, std::error_code> get_track(const std::string &title) override
    {
        return Track(title.begin(), title.end());
    }
};

class YouTubeMusicService : public MusicService
{
public:
    YouTubeMusicService(const std::string &user_name, const std::string &secret)
    {
        std::cout << "Creating YouTubeMusicService...\n";
    }

    std::string service_name() const override
    {
        return "YouTubeMusicService";
    }

    bool is_track_available(const std::string &title) override
    {
        return !title.empty();
    }

    std::expected<Track, std::error_code> get_track(const std::string &title) override
    {
        std::cout << "Fetching track from YouTubeMusicService...\n";
        std::cout << "Now a time for advertisement...\n";
        return Track(title.begin(), title.end());
    }
};

// "ConcreteProductC"
class FilesystemMusicService : public MusicService
{
public:
    FilesystemMusicService(const std::string &directory_path = "/user/music")
    {
        std::cout << "Creating FilesystemMusicService...\n";
    }

    std::string service_name() const override
    {
        return "FilesystemMusicService";
    }

    bool is_track_available(const std::string &title) override
    {
        return !title.empty();
    }

    std::expected<Track, std::error_code> get_track(const std::string &title) override
    {
        return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }
};

// "Creator"
class MusicServiceCreator
{
public:
    virtual std::unique_ptr<MusicService> create_music_service() = 0; // factory method
    virtual ~MusicServiceCreator() = default;
};

// "ConcreteCreatorA"
class TidalServiceCreator : public MusicServiceCreator
{
    std::string user_name_;
    std::string secret_;

public:
    TidalServiceCreator(const std::string &user_name, const std::string &secret)
        : user_name_{user_name}, secret_{secret}
    {
    }

    std::unique_ptr<MusicService> create_music_service() override
    {
        return std::make_unique<TidalService>(user_name_, secret_);
    }
};

// "ConcreteCreatorB"
class SpotifyServiceCreator : public MusicServiceCreator
{
    std::string user_name_;
    std::string secret_;
    int timeout_;

public:
    SpotifyServiceCreator(const std::string &user_name, const std::string &secret, int timeout)
        : user_name_{user_name}, secret_{secret}, timeout_{timeout}
    {
    }

    std::unique_ptr<MusicService> create_music_service() override
    {
        return std::make_unique<SpotifyService>(user_name_, secret_, timeout_);
    }
};

class FsMusicServiceCreator : public MusicServiceCreator
{
    std::string path_;

public:
    FsMusicServiceCreator(const std::string &path = "/music")
        : path_{path}
    {
    }

    std::unique_ptr<MusicService> create_music_service() override
    {
        return std::make_unique<FilesystemMusicService>(path_);
    }
};

class YouTubeMusicServiceCreator : public MusicServiceCreator
{
    std::string user_name_;
    std::string secret_;

public:
    YouTubeMusicServiceCreator(const std::string &user_name, const std::string &secret)
        : user_name_{user_name}, secret_{secret}
    {
    }

    std::unique_ptr<MusicService> create_music_service() override
    {
        return std::make_unique<YouTubeMusicService>(user_name_, secret_);
    }
};

namespace ModernCpp
{
    using MusicServiceCreator = std::function<std::unique_ptr<MusicService>()>;

// "ConcreteCreatorA"
class TidalServiceCreator
{
    std::string user_name_;
    std::string secret_;

public:
    TidalServiceCreator(const std::string &user_name, const std::string &secret)
        : user_name_{user_name}, secret_{secret}
    {
    }

    std::unique_ptr<MusicService> operator()() const
    {
        return std::make_unique<TidalService>(user_name_, secret_);
    }
};

// "ConcreteCreatorB"
class SpotifyServiceCreator
{
    std::string user_name_;
    std::string secret_;
    int timeout_;

public:
    SpotifyServiceCreator(const std::string &user_name, const std::string &secret, int timeout)
        : user_name_{user_name}, secret_{secret}, timeout_{timeout}
    {
    }

    std::unique_ptr<MusicService> operator()() const
    {
        return std::make_unique<SpotifyService>(user_name_, secret_, timeout_);
    }
};

class FsMusicServiceCreator
{
    std::string path_;

public:
    FsMusicServiceCreator(const std::string &path = "/music")
        : path_{path}
    {
    }

    std::unique_ptr<MusicService> operator()() const
    {
        return std::make_unique<FilesystemMusicService>(path_);
    }
};

class YouTubeMusicServiceCreator
{
    std::string user_name_;
    std::string secret_;

public:
    YouTubeMusicServiceCreator(const std::string &user_name, const std::string &secret)
        : user_name_{user_name}, secret_{secret}
    {
    }

    std::unique_ptr<MusicService> operator()() const
    {
        return std::make_unique<YouTubeMusicService>(user_name_, secret_);
    }
};
}

#endif /*FACTORY_HPP_*/
