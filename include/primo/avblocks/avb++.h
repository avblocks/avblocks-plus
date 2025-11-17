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

class _AVBlocksException : public std::runtime_error {
public:
    explicit _AVBlocksException(const std::string& message) 
        : std::runtime_error(message) {}
};

class _Library {
public:
    _Library() {
        primo::avblocks::Library::initialize();
    }
    
    ~_Library() {
        primo::avblocks::Library::shutdown();
    }
    
    _Library(const _Library&) = delete;
    _Library& operator=(const _Library&) = delete;
    _Library(_Library&&) = delete;
    _Library& operator=(_Library&&) = delete;
};

class _MediaSample {
    primo::ref<primo::codecs::MediaSample> sample_;
    
public:
    _MediaSample() 
        : sample_(primo::avblocks::Library::createMediaSample()) {}
    
    explicit _MediaSample(primo::ref<primo::codecs::MediaSample> sample)
        : sample_(std::move(sample)) {}
    
    // Delete copy operations
    _MediaSample(const _MediaSample&) = delete;
    _MediaSample& operator=(const _MediaSample&) = delete;
    
    // Enable move operations
    _MediaSample(_MediaSample&&) = default;
    _MediaSample& operator=(_MediaSample&&) = default;
    
    primo::codecs::MediaSample* get() const { return sample_.get(); }
};

class _AudioStreamInfo {
    primo::ref<primo::codecs::AudioStreamInfo> info_;
    
public:
    _AudioStreamInfo() 
        : info_(primo::avblocks::Library::createAudioStreamInfo()) {}
    
    explicit _AudioStreamInfo(primo::ref<primo::codecs::AudioStreamInfo> info)
        : info_(std::move(info)) {}
    
    // Delete copy operations
    _AudioStreamInfo(const _AudioStreamInfo&) = delete;
    _AudioStreamInfo& operator=(const _AudioStreamInfo&) = delete;
    
    // Enable move operations
    _AudioStreamInfo(_AudioStreamInfo&&) = default;
    _AudioStreamInfo& operator=(_AudioStreamInfo&&) = default;
    
    _AudioStreamInfo& streamType(primo::codecs::StreamType::Enum type) {
        info_->setStreamType(type);
        return *this;
    }
    
    _AudioStreamInfo& channels(int32_t channels) {
        info_->setChannels(channels);
        return *this;
    }
    
    _AudioStreamInfo& sampleRate(int32_t sampleRate) {
        info_->setSampleRate(sampleRate);
        return *this;
    }
    
    _AudioStreamInfo& bitsPerSample(int32_t bits) {
        info_->setBitsPerSample(bits);
        return *this;
    }
    
    primo::codecs::AudioStreamInfo* get() const { return info_.get(); }
};

class _MediaPin {
    primo::ref<primo::avblocks::MediaPin> pin_;
    
public:
    _MediaPin() 
        : pin_(primo::avblocks::Library::createMediaPin()) {}
    
    explicit _MediaPin(primo::ref<primo::avblocks::MediaPin> pin)
        : pin_(std::move(pin)) {}
    
    // Delete copy operations
    _MediaPin(const _MediaPin&) = delete;
    _MediaPin& operator=(const _MediaPin&) = delete;
    
    // Enable move operations
    _MediaPin(_MediaPin&&) = default;
    _MediaPin& operator=(_MediaPin&&) = default;
    
    _MediaPin&& streamInfo(const _AudioStreamInfo& info) && {
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    _MediaPin& streamInfo(const _AudioStreamInfo& info) & {
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    // Builder methods for audio pins
    _MediaPin&& audioStreamType(primo::codecs::StreamType::Enum streamType) && {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    _MediaPin& audioStreamType(primo::codecs::StreamType::Enum streamType) & {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    _MediaPin&& channels(int32_t channels) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return std::move(*this);
    }
    
    _MediaPin& channels(int32_t channels) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return *this;
    }
    
    _MediaPin&& sampleRate(int32_t sampleRate) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return std::move(*this);
    }
    
    _MediaPin& sampleRate(int32_t sampleRate) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return *this;
    }

    _MediaPin&& bitsPerSample(int32_t bits) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setBitsPerSample(bits);
        }
        return std::move(*this);
    }
    
    _MediaPin& bitsPerSample(int32_t bits) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setBitsPerSample(bits);
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
class _MediaSocketT {
    primo::ref<primo::avblocks::MediaSocket> socket_;
    
public:
    using string_type = typename string_traits<CharT>::string_type;
    
    _MediaSocketT() 
        : socket_(primo::avblocks::Library::createMediaSocket()) {}
    
    explicit _MediaSocketT(primo::ref<primo::avblocks::MediaSocket> socket)
        : socket_(std::move(socket)) {}
    
    // Delete copy operations
    _MediaSocketT(const _MediaSocketT&) = delete;
    _MediaSocketT& operator=(const _MediaSocketT&) = delete;
    
    // Enable move operations
    _MediaSocketT(_MediaSocketT&&) = default;
    _MediaSocketT& operator=(_MediaSocketT&&) = default;
    
    // Rvalue overloads for method chaining
    _MediaSocketT&& file(const string_type& path) && {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return std::move(*this);
    }
    
    _MediaSocketT& file(const string_type& path) & {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return *this;
    }
    
    _MediaSocketT&& streamType(primo::codecs::StreamType::Enum type) && {
        socket_->setStreamType(type);
        return std::move(*this);
    }
    
    _MediaSocketT& streamType(primo::codecs::StreamType::Enum type) & {
        socket_->setStreamType(type);
        return *this;
    }
    
    _MediaSocketT&& addPin(_MediaPin&& pin) && {
        socket_->pins()->add(pin.get());
        return std::move(*this);
    }
    
    _MediaSocketT& addPin(_MediaPin&& pin) & {
        socket_->pins()->add(pin.get());
        return *this;
    }
    
    primo::avblocks::MediaSocket* get() const { return socket_.get(); }
};

// Type aliases for convenience
using _MediaSocket = _MediaSocketT<char>;
using _MediaSocketW = _MediaSocketT<wchar_t>;

template<typename CharT = char>
class _TranscoderT {
    primo::ref<primo::avblocks::Transcoder> transcoder_;
    
public:
    _TranscoderT() 
        : transcoder_(primo::avblocks::Library::createTranscoder()) {}
    
    // Delete copy operations
    _TranscoderT(const _TranscoderT&) = delete;
    _TranscoderT& operator=(const _TranscoderT&) = delete;
    
    // Enable move operations
    _TranscoderT(_TranscoderT&&) = default;
    _TranscoderT& operator=(_TranscoderT&&) = default;
    
    _TranscoderT& allowDemoMode(bool allow = true) {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }
    
    _TranscoderT& addInput(const _MediaSocketT<CharT>& socket) {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }
    
    _TranscoderT& addOutput(const _MediaSocketT<CharT>& socket) {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }
    
    _TranscoderT& open() {
        if (!transcoder_->open()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to open transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw _AVBlocksException(msg);
        }
        return *this;
    }
    
    bool pull(int32_t& outputIndex, _MediaSample& sample) {
        return transcoder_->pull(outputIndex, sample.get()) == TRUE;
    }

    bool push(int32_t inputIndex, _MediaSample& sample) {
        return transcoder_->push(inputIndex, sample.get()) == TRUE;
    }
    
    _TranscoderT& run() {
        if (!transcoder_->run()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to run transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw _AVBlocksException(msg);
        }
        return *this;
    }
    
    void close() {
        transcoder_->close();
    }

    const primo::error::ErrorInfo* error() const {
        return transcoder_->error();
    }
    
    primo::avblocks::Transcoder* get() const { return transcoder_.get(); }
};

// Type aliases for convenience
using _Transcoder = _TranscoderT<char>;
using _TranscoderW = _TranscoderT<wchar_t>;

} // namespace primo::avblocks::modern