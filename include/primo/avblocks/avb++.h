#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <memory>
#include <optional>
#include <stdexcept>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

namespace primo::avblocks::modern {

class AVBlocksException : public std::runtime_error {
public:
    explicit AVBlocksException(const std::string& message) 
        : std::runtime_error(message) {}
};

class Library {
public:
    Library() {
        primo::avblocks::Library::initialize();
    }
    
    ~Library() {
        primo::avblocks::Library::shutdown();
    }
    
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
    Library(Library&&) = delete;
    Library& operator=(Library&&) = delete;
};

class AudioStreamInfo {
    primo::ref<primo::codecs::AudioStreamInfo> info_;
    
public:
    AudioStreamInfo() 
        : info_(primo::avblocks::Library::createAudioStreamInfo()) {}
    
    explicit AudioStreamInfo(primo::ref<primo::codecs::AudioStreamInfo> info)
        : info_(std::move(info)) {}
    
    // Delete copy operations
    AudioStreamInfo(const AudioStreamInfo&) = delete;
    AudioStreamInfo& operator=(const AudioStreamInfo&) = delete;
    
    // Enable move operations
    AudioStreamInfo(AudioStreamInfo&&) = default;
    AudioStreamInfo& operator=(AudioStreamInfo&&) = default;
    
    AudioStreamInfo& streamType(primo::codecs::StreamType::Enum type) {
        info_->setStreamType(type);
        return *this;
    }
    
    AudioStreamInfo& channels(int32_t channels) {
        info_->setChannels(channels);
        return *this;
    }
    
    AudioStreamInfo& sampleRate(int32_t sampleRate) {
        info_->setSampleRate(sampleRate);
        return *this;
    }
    
    AudioStreamInfo& bitsPerSample(int32_t bits) {
        info_->setBitsPerSample(bits);
        return *this;
    }
    
    primo::codecs::AudioStreamInfo* get() const { return info_.get(); }
};

class MediaPin {
    primo::ref<primo::avblocks::MediaPin> pin_;
    
public:
    MediaPin() 
        : pin_(primo::avblocks::Library::createMediaPin()) {}
    
    explicit MediaPin(primo::ref<primo::avblocks::MediaPin> pin)
        : pin_(std::move(pin)) {}
    
    // Delete copy operations
    MediaPin(const MediaPin&) = delete;
    MediaPin& operator=(const MediaPin&) = delete;
    
    // Enable move operations
    MediaPin(MediaPin&&) = default;
    MediaPin& operator=(MediaPin&&) = default;
    
    MediaPin&& streamInfo(const AudioStreamInfo& info) && {
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    MediaPin& streamInfo(const AudioStreamInfo& info) & {
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    // Builder methods for audio pins
    MediaPin&& audioStreamType(primo::codecs::StreamType::Enum streamType) && {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    MediaPin& audioStreamType(primo::codecs::StreamType::Enum streamType) & {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    MediaPin&& channels(int32_t channels) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return std::move(*this);
    }
    
    MediaPin& channels(int32_t channels) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return *this;
    }
    
    MediaPin&& sampleRate(int32_t sampleRate) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return std::move(*this);
    }
    
    MediaPin& sampleRate(int32_t sampleRate) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return *this;
    }
    
    primo::avblocks::MediaPin* get() const { return pin_.get(); }
};

// String traits for handling both char and wchar_t
template<typename CharT>
struct string_traits;

template<>
struct string_traits<char> {
    using string_type = std::string;
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
};

template<>
struct string_traits<wchar_t> {
    using string_type = std::wstring;
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
};

template<typename CharT = char>
class MediaSocketT {
    primo::ref<primo::avblocks::MediaSocket> socket_;
    
public:
    using string_type = typename string_traits<CharT>::string_type;
    
    MediaSocketT() 
        : socket_(primo::avblocks::Library::createMediaSocket()) {}
    
    explicit MediaSocketT(primo::ref<primo::avblocks::MediaSocket> socket)
        : socket_(std::move(socket)) {}
    
    // Delete copy operations
    MediaSocketT(const MediaSocketT&) = delete;
    MediaSocketT& operator=(const MediaSocketT&) = delete;
    
    // Enable move operations
    MediaSocketT(MediaSocketT&&) = default;
    MediaSocketT& operator=(MediaSocketT&&) = default;
    
    // Rvalue overloads for method chaining
    MediaSocketT&& file(const string_type& path) && {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return std::move(*this);
    }
    
    MediaSocketT& file(const string_type& path) & {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return *this;
    }
    
    MediaSocketT&& streamType(primo::codecs::StreamType::Enum type) && {
        socket_->setStreamType(type);
        return std::move(*this);
    }
    
    MediaSocketT& streamType(primo::codecs::StreamType::Enum type) & {
        socket_->setStreamType(type);
        return *this;
    }
    
    MediaSocketT&& addPin(MediaPin&& pin) && {
        socket_->pins()->add(pin.get());
        return std::move(*this);
    }
    
    MediaSocketT& addPin(MediaPin&& pin) & {
        socket_->pins()->add(pin.get());
        return *this;
    }
    
    primo::avblocks::MediaSocket* get() const { return socket_.get(); }
};

// Type aliases for convenience
using MediaSocket = MediaSocketT<char>;
using MediaSocketW = MediaSocketT<wchar_t>;

template<typename CharT = char>
class TranscoderT {
    primo::ref<primo::avblocks::Transcoder> transcoder_;
    
public:
    TranscoderT() 
        : transcoder_(primo::avblocks::Library::createTranscoder()) {}
    
    // Delete copy operations
    TranscoderT(const TranscoderT&) = delete;
    TranscoderT& operator=(const TranscoderT&) = delete;
    
    // Enable move operations
    TranscoderT(TranscoderT&&) = default;
    TranscoderT& operator=(TranscoderT&&) = default;
    
    TranscoderT& allowDemoMode(bool allow = true) {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }
    
    TranscoderT& addInput(const MediaSocketT<CharT>& socket) {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }
    
    TranscoderT& addOutput(const MediaSocketT<CharT>& socket) {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }
    
    TranscoderT& open() {
        if (!transcoder_->open()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to open transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw AVBlocksException(msg);
        }
        return *this;
    }
    
    TranscoderT& run() {
        if (!transcoder_->run()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to run transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw AVBlocksException(msg);
        }
        return *this;
    }
    
    void close() {
        transcoder_->close();
    }
    
    primo::avblocks::Transcoder* get() const { return transcoder_.get(); }
};

// Type aliases for convenience
using Transcoder = TranscoderT<char>;
using TranscoderW = TranscoderT<wchar_t>;

} // namespace primo::avblocks::modern