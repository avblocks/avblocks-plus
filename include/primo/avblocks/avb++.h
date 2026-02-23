#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
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

class TLibrary {
public:
    TLibrary() {
        primo::avblocks::Library::initialize();
    }
    
    ~TLibrary() {
        primo::avblocks::Library::shutdown();
    }
    
    TLibrary(const TLibrary&) = delete;
    TLibrary& operator=(const TLibrary&) = delete;
    TLibrary(TLibrary&&) = delete;
    TLibrary& operator=(TLibrary&&) = delete;

    // ---- Static accessors mirroring primo::avblocks::Library namespace ----

    static bool initialize() {
        return primo::avblocks::Library::initialize() == TRUE;
    }

    static void shutdown() {
        primo::avblocks::Library::shutdown();
    }

    static const char* description() {
        return primo::avblocks::Library::description();
    }

    static int32_t majorVersion() {
        return primo::avblocks::Library::majorVersion();
    }

    static int32_t minorVersion() {
        return primo::avblocks::Library::minorVersion();
    }

    static int32_t patchVersion() {
        return primo::avblocks::Library::patchVersion();
    }

    static void setLicenseTls(bool useTls) {
        primo::avblocks::Library::setLicenseTls(useTls ? TRUE : FALSE);
    }

    static int32_t setLicense(const char* license) {
        return primo::avblocks::Library::setLicense(license);
    }
};

class TLicenseInfo {
    primo::ref<primo::license::LicenseInfo> info_;

public:
    TLicenseInfo()
        : info_(primo::avblocks::Library::createLicenseInfo()) {}

    explicit TLicenseInfo(primo::ref<primo::license::LicenseInfo> info)
        : info_(std::move(info)) {}

    // Non-copyable, movable
    TLicenseInfo(const TLicenseInfo&) = delete;
    TLicenseInfo& operator=(const TLicenseInfo&) = delete;
    TLicenseInfo(TLicenseInfo&&) = default;
    TLicenseInfo& operator=(TLicenseInfo&&) = default;

    /// Returns the current license status as a combination of LicenseStatusFlags::Enum flags.
    primo::license::LicenseStatusFlags::Enum licenseStatus() const {
        return static_cast<primo::license::LicenseStatusFlags::Enum>(info_->licenseStatus());
    }

    /// Returns true if the specified product/feature is licensed.
    /// Pass nullptr for product/feature to use library defaults.
    bool isLicensed(const char* product = nullptr, const char* feature = nullptr) const {
        return info_->isLicensed(product, feature) == TRUE;
    }

    primo::license::LicenseInfo* get() const { return info_.get(); }
};

class TErrorInfo {
    int32_t facility_{};
    int32_t code_{};
    std::string message_;
    std::string hint_;

public:
    TErrorInfo() = default;

    explicit TErrorInfo(const primo::error::ErrorInfo* error) {
        if (error) {
            facility_ = error->facility();
            code_     = error->code();
            if (error->message()) message_ = std::string(primo::ustring(error->message()));
            if (error->hint())    hint_    = std::string(primo::ustring(error->hint()));
        }
    }

    int32_t            facility() const { return facility_; }
    int32_t            code()     const { return code_; }
    const std::string& message()  const { return message_; }
    const std::string& hint()     const { return hint_; }
};

class TAVBlocksException : public std::runtime_error {
    TErrorInfo error_;
public:
    TAVBlocksException(const char* context, const TErrorInfo& error)
        : std::runtime_error(
            error.message().empty()
                ? std::string(context)
                : std::string(context) + ": " + error.message())
        , error_(error) {}

    const TErrorInfo& error() const { return error_; }
};

class TMediaBuffer {
    primo::ref<primo::codecs::MediaBuffer> buffer_;

public:
    TMediaBuffer()
        : buffer_(primo::avblocks::Library::createMediaBuffer()) {}

    explicit TMediaBuffer(int32_t capacity)
        : buffer_(primo::avblocks::Library::createMediaBuffer(capacity)) {}

    explicit TMediaBuffer(primo::ref<primo::codecs::MediaBuffer> buffer)
        : buffer_(std::move(buffer)) {}

    // Wrap an existing (non-owned) MediaBuffer* — retains it
    explicit TMediaBuffer(primo::codecs::MediaBuffer* buffer)
        : buffer_(buffer) { if (buffer) buffer->retain(); }

    // Delete copy operations
    TMediaBuffer(const TMediaBuffer&) = delete;
    TMediaBuffer& operator=(const TMediaBuffer&) = delete;

    // Enable move operations
    TMediaBuffer(TMediaBuffer&&) = default;
    TMediaBuffer& operator=(TMediaBuffer&&) = default;

    TMediaBuffer&& attach(const uint8_t* data, size_t size, bool copy = true) && {
        buffer_->attach(const_cast<uint8_t*>(data), static_cast<int32_t>(size), copy ? TRUE : FALSE);
        return std::move(*this);
    }

    TMediaBuffer& attach(const uint8_t* data, size_t size, bool copy = true) & {
        buffer_->attach(const_cast<uint8_t*>(data), static_cast<int32_t>(size), copy ? TRUE : FALSE);
        return *this;
    }

    // -- Getters --

    uint8_t*    start()           const { return buffer_->start(); }
    int32_t     capacity()        const { return buffer_->capacity(); }
    bool        external()        const { return buffer_->external() == TRUE; }
    const uint8_t* data()         const { return buffer_->data(); }
    int32_t     dataSize()        const { return buffer_->dataSize(); }
    int32_t     dataOffset()      const { return buffer_->dataOffset(); }
    int32_t     freeSpace()       const { return buffer_->freeSpace(); }
    int32_t     freeLinearSpace() const { return buffer_->freeLinearSpace(); }

    // -- Fluent setter --

    TMediaBuffer& setData(int32_t offset, int32_t size) {
        buffer_->setData(offset, size);
        return *this;
    }

    // -- Operations --

    bool alloc(int32_t size, bool keepData) {
        return buffer_->alloc(size, keepData ? TRUE : FALSE) == TRUE;
    }

    bool alignedAlloc(int32_t alignment, int32_t size, bool keepData) {
        return buffer_->alignedAlloc(alignment, size, keepData ? TRUE : FALSE) == TRUE;
    }

    TMediaBuffer& free() {
        buffer_->free();
        return *this;
    }

    TMediaBuffer& clear() {
        buffer_->clear();
        return *this;
    }

    TMediaBuffer& normalize() {
        buffer_->normalize();
        return *this;
    }

    bool append(const void* data, int32_t dataSize) {
        return buffer_->append(data, dataSize) == TRUE;
    }

    TMediaBuffer& remove(int32_t dataSize) {
        buffer_->remove(dataSize);
        return *this;
    }

    uint8_t* detach() {
        return buffer_->detach();
    }

    TMediaBuffer clone() const {
        return TMediaBuffer(primo::ref<primo::codecs::MediaBuffer>(buffer_->clone()));
    }

    primo::codecs::MediaBuffer* get() const { return buffer_.get(); }
};

class TMediaSample {
    primo::ref<primo::codecs::MediaSample> sample_;
    
public:
    TMediaSample() 
        : sample_(primo::avblocks::Library::createMediaSample()) {}
    
    explicit TMediaSample(primo::ref<primo::codecs::MediaSample> sample)
        : sample_(std::move(sample)) {}
    
    // Delete copy operations
    TMediaSample(const TMediaSample&) = delete;
    TMediaSample& operator=(const TMediaSample&) = delete;
    
    // Enable move operations
    TMediaSample(TMediaSample&&) = default;
    TMediaSample& operator=(TMediaSample&&) = default;

    // -- Fluent buffer setters --

    TMediaSample&& buffer(TMediaBuffer&& buf) && {
        sample_->setBuffer(buf.get());
        return std::move(*this);
    }

    TMediaSample& buffer(TMediaBuffer&& buf) & {
        sample_->setBuffer(buf.get());
        return *this;
    }

    TMediaSample& buffer(std::nullptr_t) {
        sample_->setBuffer(nullptr);
        return *this;
    }

    // -- Const getters --

    TMediaBuffer buffer() const {
        return TMediaBuffer(sample_->buffer());
    }

    double  startTime()    const { return sample_->startTime(); }
    double  endTime()      const { return sample_->endTime(); }
    int32_t flags()        const { return sample_->flags(); }
    int32_t streamNumber() const { return sample_->streamNumber(); }

    primo::codecs::PictureType::Enum pictureType() const {
        return sample_->pictureType();
    }

    primo::codecs::FrameType::Enum frameType() const {
        return sample_->frameType();
    }

    int32_t videoBufferSizeInBytes(int32_t width, int32_t height,
                                   primo::codecs::ColorFormat::Enum colorFormat) const {
        return sample_->videoBufferSizeInBytes(width, height, colorFormat);
    }

    // -- Fluent setters --

    TMediaSample& startTime(double time)    { sample_->setStartTime(time); return *this; }
    TMediaSample& endTime(double time)      { sample_->setEndTime(time);   return *this; }
    TMediaSample& flags(int32_t f)          { sample_->setFlags(f);        return *this; }
    TMediaSample& streamNumber(int32_t n)   { sample_->setStreamNumber(n); return *this; }

    TMediaSample& pictureType(primo::codecs::PictureType::Enum pt) {
        sample_->setPictureType(pt);
        return *this;
    }

    TMediaSample& frameType(primo::codecs::FrameType::Enum ft) {
        sample_->setFrameType(ft);
        return *this;
    }

    // -- Operations --

    TMediaSample& reset() {
        sample_->reset();
        return *this;
    }

    TMediaSample clone() const {
        return TMediaSample(primo::ref<primo::codecs::MediaSample>(sample_->clone()));
    }

    primo::codecs::MediaSample* get() const { return sample_.get(); }
};

/**
 * CRTP base for @c TAudioStreamInfo, @c TVideoStreamInfo, and @c TDataStreamInfo.
 *
 * Holds the @c primo::ref<RawT> and exposes all properties defined on the
 * raw @c primo::codecs::StreamInfo interface, with fluent setters that return
 * @c Derived& so chaining works naturally on the concrete subclass.
 */
template<typename Derived, typename RawT>
class TStreamInfo {
protected:
    primo::ref<RawT> info_;

    explicit TStreamInfo(primo::ref<RawT> info)
        : info_(std::move(info)) {}

    /// Wraps a non-owned pointer, retaining it. Used by subclass raw-pointer constructors.
    explicit TStreamInfo(RawT* info)
        : info_(info) { if (info) info->retain(); }

    TStreamInfo(const TStreamInfo&) = delete;
    TStreamInfo& operator=(const TStreamInfo&) = delete;
    TStreamInfo(TStreamInfo&&) = default;
    TStreamInfo& operator=(TStreamInfo&&) = default;

private:
    Derived& self() { return static_cast<Derived&>(*this); }
    const Derived& self() const { return static_cast<const Derived&>(*this); }

public:
    // -- Fluent setters --

    Derived& streamType(primo::codecs::StreamType::Enum type) {
        info_->setStreamType(type);
        return self();
    }

    Derived& streamSubType(primo::codecs::StreamSubType::Enum subType) {
        info_->setStreamSubType(subType);
        return self();
    }

    Derived& duration(double d) {
        info_->setDuration(d);
        return self();
    }

    Derived& bitrate(int32_t bps) {
        info_->setBitrate(bps);
        return self();
    }

    Derived& bitrateMode(int32_t mode) {
        info_->setBitrateMode(mode);
        return self();
    }

    Derived& ID(int32_t id) {
        info_->setID(id);
        return self();
    }

    Derived& programNumber(int32_t num) {
        info_->setProgramNumber(num);
        return self();
    }

    Derived& configData(primo::codecs::MediaBuffer* buf) {
        info_->setConfigData(buf);
        return self();
    }

    // -- Getters --

    primo::codecs::MediaType::Enum     mediaType()     const { return info_->mediaType(); }
    primo::codecs::StreamType::Enum    streamType()    const { return info_->streamType(); }
    primo::codecs::StreamSubType::Enum streamSubType() const { return info_->streamSubType(); }
    double                             duration()      const { return info_->duration(); }
    int32_t                            bitrate()       const { return info_->bitrate(); }
    int32_t                            bitrateMode()   const { return info_->bitrateMode(); }
    int32_t                            ID()            const { return info_->ID(); }
    int32_t                            programNumber() const { return info_->programNumber(); }
    bool                               immutable()     const { return info_->immutable() == TRUE; }
    primo::codecs::MediaBuffer*        configData()    const { return info_->configData(); }

    // -- Operations --

    /// Resets all fields to their defaults. Returns @c false if the object is immutable.
    bool reset() { return info_->reset() == TRUE; }

    /// Returns the raw underlying pointer. For interop with the C API only.
    RawT* get() const { return info_.get(); }
};

class TAudioStreamInfo
    : public TStreamInfo<TAudioStreamInfo, primo::codecs::AudioStreamInfo> {
    using Base = TStreamInfo<TAudioStreamInfo, primo::codecs::AudioStreamInfo>;

public:
    TAudioStreamInfo()
        : Base(primo::ref<primo::codecs::AudioStreamInfo>(
              primo::avblocks::Library::createAudioStreamInfo())) {}

    explicit TAudioStreamInfo(primo::ref<primo::codecs::AudioStreamInfo> info)
        : Base(std::move(info)) {}

    TAudioStreamInfo(const TAudioStreamInfo&) = delete;
    TAudioStreamInfo& operator=(const TAudioStreamInfo&) = delete;
    TAudioStreamInfo(TAudioStreamInfo&&) = default;
    TAudioStreamInfo& operator=(TAudioStreamInfo&&) = default;

    // -- Fluent setters --

    TAudioStreamInfo& channels(int32_t channels) {
        info_->setChannels(channels);
        return *this;
    }

    TAudioStreamInfo& sampleRate(int32_t sampleRate) {
        info_->setSampleRate(sampleRate);
        return *this;
    }

    TAudioStreamInfo& bitsPerSample(int32_t bits) {
        info_->setBitsPerSample(bits);
        return *this;
    }

    TAudioStreamInfo& pcmFlags(int32_t flags) {
        info_->setPcmFlags(flags);
        return *this;
    }

    TAudioStreamInfo& channelLayout(int32_t layout) {
        info_->setChannelLayout(layout);
        return *this;
    }

    TAudioStreamInfo& bytesPerFrame(int32_t bytes) {
        info_->setBytesPerFrame(bytes);
        return *this;
    }

    // -- Getters --

    int32_t channels()      const { return info_->channels(); }
    int32_t sampleRate()    const { return info_->sampleRate(); }
    int32_t bitsPerSample() const { return info_->bitsPerSample(); }
    int32_t pcmFlags()      const { return info_->pcmFlags(); }
    int32_t channelLayout() const { return info_->channelLayout(); }
    int32_t bytesPerFrame() const { return info_->bytesPerFrame(); }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TAudioStreamInfo clone() const {
        return TAudioStreamInfo(
            primo::ref<primo::codecs::AudioStreamInfo>(info_->clone()));
    }

    /// Copies all fields to @p dst. Returns @c false if @p dst is null.
    bool copyTo(TAudioStreamInfo& dst) const {
        return info_->copyTo(dst.get()) == TRUE;
    }
};

class TVideoStreamInfo
    : public TStreamInfo<TVideoStreamInfo, primo::codecs::VideoStreamInfo> {
    using Base = TStreamInfo<TVideoStreamInfo, primo::codecs::VideoStreamInfo>;

public:
    TVideoStreamInfo()
        : Base(primo::ref<primo::codecs::VideoStreamInfo>(
              primo::avblocks::Library::createVideoStreamInfo())) {}

    explicit TVideoStreamInfo(primo::ref<primo::codecs::VideoStreamInfo> info)
        : Base(std::move(info)) {}

    /// Wraps a non-owned @c VideoStreamInfo pointer, retaining it. Used by @c TMediaPin::videoStreamInfo().
    explicit TVideoStreamInfo(primo::codecs::VideoStreamInfo* info)
        : Base(info) {}

    TVideoStreamInfo(const TVideoStreamInfo&) = delete;
    TVideoStreamInfo& operator=(const TVideoStreamInfo&) = delete;
    TVideoStreamInfo(TVideoStreamInfo&&) = default;
    TVideoStreamInfo& operator=(TVideoStreamInfo&&) = default;

    // -- Fluent setters --

    TVideoStreamInfo& colorFormat(primo::codecs::ColorFormat::Enum format) {
        info_->setColorFormat(format);
        return *this;
    }

    TVideoStreamInfo& frameWidth(int32_t width) {
        info_->setFrameWidth(width);
        return *this;
    }

    TVideoStreamInfo& frameHeight(int32_t height) {
        info_->setFrameHeight(height);
        return *this;
    }

    TVideoStreamInfo& displayRatioWidth(int32_t width) {
        info_->setDisplayRatioWidth(width);
        return *this;
    }

    TVideoStreamInfo& displayRatioHeight(int32_t height) {
        info_->setDisplayRatioHeight(height);
        return *this;
    }

    TVideoStreamInfo& frameRate(double fps) {
        info_->setFrameRate(fps);
        return *this;
    }

    TVideoStreamInfo& scanType(primo::codecs::ScanType::Enum scan) {
        info_->setScanType(scan);
        return *this;
    }

    TVideoStreamInfo& stride(int32_t stride) {
        info_->setStride(stride);
        return *this;
    }

    TVideoStreamInfo& frameBottomUp(bool bottomUp) {
        info_->setFrameBottomUp(bottomUp ? TRUE : FALSE);
        return *this;
    }

    // -- Getters --

    primo::codecs::ColorFormat::Enum colorFormat()      const { return info_->colorFormat(); }
    int32_t                          frameWidth()       const { return info_->frameWidth(); }
    int32_t                          frameHeight()      const { return info_->frameHeight(); }
    int32_t                          displayRatioWidth() const { return info_->displayRatioWidth(); }
    int32_t                          displayRatioHeight() const { return info_->displayRatioHeight(); }
    double                           frameRate()        const { return info_->frameRate(); }
    primo::codecs::ScanType::Enum    scanType()         const { return info_->scanType(); }
    int32_t                          stride()           const { return info_->stride(); }
    bool                             frameBottomUp()    const { return info_->frameBottomUp() == TRUE; }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TVideoStreamInfo clone() const {
        return TVideoStreamInfo(
            primo::ref<primo::codecs::VideoStreamInfo>(info_->clone()));
    }

    /// Copies all fields to @p dst. Returns @c false if @p dst is null.
    bool copyTo(TVideoStreamInfo& dst) const {
        return info_->copyTo(dst.get()) == TRUE;
    }
};

/**
 * Fluent wrapper for a generic data stream.
 *
 * Wraps @c primo::codecs::StreamInfo as returned by
 * @c Library::createDataStreamInfo(). The media type is always
 * @c MediaType::Data and cannot be changed. There are no subclass-specific
 * properties beyond those exposed by the @c TStreamInfo CRTP base.
 */
class TDataStreamInfo
    : public TStreamInfo<TDataStreamInfo, primo::codecs::StreamInfo> {
    using Base = TStreamInfo<TDataStreamInfo, primo::codecs::StreamInfo>;

public:
    TDataStreamInfo()
        : Base(primo::ref<primo::codecs::StreamInfo>(
              primo::avblocks::Library::createDataStreamInfo())) {}

    explicit TDataStreamInfo(primo::ref<primo::codecs::StreamInfo> info)
        : Base(std::move(info)) {}

    TDataStreamInfo(const TDataStreamInfo&) = delete;
    TDataStreamInfo& operator=(const TDataStreamInfo&) = delete;
    TDataStreamInfo(TDataStreamInfo&&) = default;
    TDataStreamInfo& operator=(TDataStreamInfo&&) = default;

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TDataStreamInfo clone() const {
        return TDataStreamInfo(
            primo::ref<primo::codecs::StreamInfo>(info_->clone()));
    }

    /// Copies all fields to @p dst. Returns @c false if @p dst is null.
    bool copyTo(TDataStreamInfo& dst) const {
        return info_->copyTo(dst.get()) == TRUE;
    }
};

class TMediaPin {
    primo::ref<primo::avblocks::MediaPin> pin_;
    
public:
    TMediaPin() 
        : pin_(primo::avblocks::Library::createMediaPin()) {}
    
    explicit TMediaPin(primo::ref<primo::avblocks::MediaPin> pin)
        : pin_(std::move(pin)) {}

    /// Wraps a non-owned @c MediaPin pointer, retaining it. Used by @c TMediaSocketT::pin().
    explicit TMediaPin(primo::avblocks::MediaPin* pin)
        : pin_(pin) { if (pin) pin->retain(); }
    
    // Delete copy operations
    TMediaPin(const TMediaPin&) = delete;
    TMediaPin& operator=(const TMediaPin&) = delete;
    
    // Enable move operations
    TMediaPin(TMediaPin&&) = default;
    TMediaPin& operator=(TMediaPin&&) = default;
    
    TMediaPin&& streamInfo(const TAudioStreamInfo& info) && {
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    TMediaPin& streamInfo(const TAudioStreamInfo& info) & {
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    TMediaPin&& streamInfo(const TVideoStreamInfo& info) && {
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    TMediaPin& streamInfo(const TVideoStreamInfo& info) & {
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    /// Returns the pin's stream info cast to @c TVideoStreamInfo, wrapping the existing pointer.
    TVideoStreamInfo videoStreamInfo() const {
        return TVideoStreamInfo(
            static_cast<primo::codecs::VideoStreamInfo*>(pin_->streamInfo()));
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
class TMediaSocketT {
    primo::ref<primo::avblocks::MediaSocket> socket_;
    
public:
    using string_type = typename string_traits<CharT>::string_type;
    
    TMediaSocketT() 
        : socket_(primo::avblocks::Library::createMediaSocket()) {}
    
    explicit TMediaSocketT(primo::ref<primo::avblocks::MediaSocket> socket)
        : socket_(std::move(socket)) {}

    /// Wraps a non-owned @c MediaSocket pointer, retaining it. Used by @c TMediaInfo::input() / @c output().
    explicit TMediaSocketT(primo::avblocks::MediaSocket* socket)
        : socket_(socket) { if (socket) socket->retain(); }

    /// Creates a socket from a @c TMediaInfo, wrapping @c Library::createMediaSocket(MediaInfo*).
    /// Member template so the body is instantiation-checked, allowing @c TMediaInfo to be
    /// defined after @c TMediaSocketT without a circular dependency.
    template<typename TMediaInfoT>
    explicit TMediaSocketT(const TMediaInfoT& info)
        : socket_(primo::avblocks::Library::createMediaSocket(info.get())) {
        socket_->setFile(nullptr);
        socket_->setStream(nullptr);
    }

    // Delete copy operations
    TMediaSocketT(const TMediaSocketT&) = delete;
    TMediaSocketT& operator=(const TMediaSocketT&) = delete;
    
    // Enable move operations
    TMediaSocketT(TMediaSocketT&&) = default;
    TMediaSocketT& operator=(TMediaSocketT&&) = default;
    
    /// @name Fluent setters (rvalue and lvalue overloads for method chaining)
    /// @{
    TMediaSocketT&& file(const string_type& path) && {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return std::move(*this);
    }
    
    TMediaSocketT& file(const string_type& path) & {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return *this;
    }
    
    TMediaSocketT&& streamType(primo::codecs::StreamType::Enum type) && {
        socket_->setStreamType(type);
        return std::move(*this);
    }
    
    TMediaSocketT& streamType(primo::codecs::StreamType::Enum type) & {
        socket_->setStreamType(type);
        return *this;
    }
    
    TMediaSocketT&& streamSubType(primo::codecs::StreamSubType::Enum subType) && {
        socket_->setStreamSubType(subType);
        return std::move(*this);
    }
    
    TMediaSocketT& streamSubType(primo::codecs::StreamSubType::Enum subType) & {
        socket_->setStreamSubType(subType);
        return *this;
    }
    
    TMediaSocketT&& addPin(TMediaPin&& pin) && {
        socket_->pins()->add(pin.get());
        return std::move(*this);
    }
    
    TMediaSocketT& addPin(TMediaPin&& pin) & {
        socket_->pins()->add(pin.get());
        return *this;
    }

    /// @}
    /// Returns the pin at @p index, wrapping the existing (non-owned) pointer.
    TMediaPin pin(int32_t index) const {
        return TMediaPin(socket_->pins()->at(index));
    }

    primo::avblocks::MediaSocket* get() const { return socket_.get(); }
};

// Type aliases for convenience
using TMediaSocket = TMediaSocketT<char>;
using TMediaSocketW = TMediaSocketT<wchar_t>;

class TMediaInfo {
    primo::ref<primo::avblocks::MediaInfo> info_;

public:
    TMediaInfo()
        : info_(primo::avblocks::Library::createMediaInfo()) {}

    // Delete copy operations
    TMediaInfo(const TMediaInfo&) = delete;
    TMediaInfo& operator=(const TMediaInfo&) = delete;

    // Enable move operations
    TMediaInfo(TMediaInfo&&) = default;
    TMediaInfo& operator=(TMediaInfo&&) = default;

    TMediaInfo& open() {
        if (!info_->open()) {
            throw TAVBlocksException("Failed to open MediaInfo", TErrorInfo(info_->error()));
        }
        return *this;
    }

    TErrorInfo error() const {
        return TErrorInfo(info_->error());
    }

    TMediaSocket input(int32_t index) const {
        return TMediaSocket(info_->inputs()->at(index));
    }

    TMediaSocket output(int32_t index) const {
        return TMediaSocket(info_->outputs()->at(index));
    }

    primo::avblocks::MediaInfo* get() const { return info_.get(); }
};

template<typename CharT = char>
class TTranscoderT {
    primo::ref<primo::avblocks::Transcoder> transcoder_;
    
public:
    TTranscoderT() 
        : transcoder_(primo::avblocks::Library::createTranscoder()) {}
    
    // Delete copy operations
    TTranscoderT(const TTranscoderT&) = delete;
    TTranscoderT& operator=(const TTranscoderT&) = delete;
    
    // Enable move operations
    TTranscoderT(TTranscoderT&&) = default;
    TTranscoderT& operator=(TTranscoderT&&) = default;
    
    TTranscoderT& allowDemoMode(bool allow = true) {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }
    
    TTranscoderT& addInput(const TMediaSocketT<CharT>& socket) {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }
    
    TTranscoderT& addOutput(const TMediaSocketT<CharT>& socket) {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }
    
    TTranscoderT& open() {
        if (!transcoder_->open()) {
            throw TAVBlocksException("Failed to open transcoder", TErrorInfo(transcoder_->error()));
        }
        return *this;
    }
    
    bool pull(int32_t& outputIndex, TMediaSample& sample) {
        return transcoder_->pull(outputIndex, sample.get()) == TRUE;
    }

    bool push(int32_t inputIndex, TMediaSample& sample) {
        return transcoder_->push(inputIndex, sample.get()) == TRUE;
    }
    
    TTranscoderT& run() {
        if (!transcoder_->run()) {
            throw TAVBlocksException("Failed to run transcoder", TErrorInfo(transcoder_->error()));
        }
        return *this;
    }
    
    void close() {
        transcoder_->close();
    }

    bool flush() {
        return transcoder_->flush() == TRUE;
    }

    TErrorInfo error() const {
        return TErrorInfo(transcoder_->error());
    }
    
    primo::avblocks::Transcoder* get() const { return transcoder_.get(); }
};

// Type aliases for convenience
using TTranscoder = TTranscoderT<char>;
using TTranscoderW = TTranscoderT<wchar_t>;

} // namespace primo::avblocks::modern