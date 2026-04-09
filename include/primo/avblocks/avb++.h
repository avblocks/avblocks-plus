#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <memory>
#include <functional>
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

// ---- Metadata wrappers -------------------------------------------------------

/**
 * Fluent wrapper for @c primo::codecs::MetaAttribute.
 *
 * Owns the raw object through a @c primo::ref<MetaAttribute>.
 * Non-copyable, movable. Constructed from a factory-owned object (via
 * @c primo::ref) or from a non-owned pointer (via raw-pointer ctor, which
 * retains it).
 */
class TMetaAttribute {
    primo::ref<primo::codecs::MetaAttribute> attr_;

public:
    TMetaAttribute()
        : attr_(primo::avblocks::Library::createMetaAttribute()) {}

    explicit TMetaAttribute(primo::ref<primo::codecs::MetaAttribute> attr)
        : attr_(std::move(attr)) {}

    /// Wraps a non-owned @c MetaAttribute pointer, retaining it.
    explicit TMetaAttribute(primo::codecs::MetaAttribute* attr)
        : attr_(attr) { if (attr) attr->retain(); }

    TMetaAttribute(const TMetaAttribute&) = delete;
    TMetaAttribute& operator=(const TMetaAttribute&) = delete;
    TMetaAttribute(TMetaAttribute&&) = default;
    TMetaAttribute& operator=(TMetaAttribute&&) = default;

    // -- Getters --

    /// Returns the attribute name as an ANSI string (see @c primo::codecs::Meta constants).
    const char* name() const { return attr_ ? attr_->name() : nullptr; }

    /// Returns the attribute value as a UTF-8 @c std::string.
    std::string value() const {
        return attr_ ? std::string(primo::ustring(attr_->value())) : std::string{};
    }

    /// Returns @c true if the attribute is immutable.
    bool immutable() const { return attr_ && attr_->immutable() == TRUE; }

    // -- Fluent setters --

    /// Sets the attribute name (@p name is an ANSI string).
    TMetaAttribute& name(const char* n) { if (attr_) attr_->setName(n); return *this; }

    /// Sets the attribute value (@p val is a UTF-8 C-string).
    TMetaAttribute& value(const char* val) {
        if (attr_) attr_->setValue(primo::ustring(val));
        return *this;
    }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TMetaAttribute clone() const {
        return TMetaAttribute(
            primo::ref<primo::codecs::MetaAttribute>(attr_->clone()));
    }

    /// Returns @c true when this handle wraps a non-null underlying object.
    bool valid() const { return attr_.get() != nullptr; }

    primo::codecs::MetaAttribute* get() const { return attr_.get(); }
};

/**
 * Non-owning view of a @c MetaAttributeList.
 *
 * Returned by @c TMetadata::attributes().
 */
class TMetaAttributeList {
    primo::codecs::MetaAttributeList* list_;

public:
    explicit TMetaAttributeList(primo::codecs::MetaAttributeList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMetaAttribute) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        TMetaAttributeList& list_;
        int32_t index_;
        /// Read — implicit conversion to a non-owning attribute wrapper.
        operator TMetaAttribute() const { return TMetaAttribute(list_.list_->at(index_)); }
        /// Write — calls @c setAt; returns @c true on success.
        bool operator=(TMetaAttribute&& a) { return list_.list_->setAt(index_, a.get()) == TRUE; }
    };

    /// Returns the number of attributes.
    int32_t count() const { return list_ ? list_->count() : 0; }

    /// Returns the attribute at @p index (non-owning, retaining).
    TMetaAttribute at(int32_t index) const { return TMetaAttribute(list_->at(index)); }

    /// Non-const lvalue subscript — returns a proxy supporting read and @c setAt write.
    ItemRef operator[](int32_t index) & { return {*this, index}; }

    /// Returns the first attribute whose name matches @p name (non-owning, retaining).
    /// Check @c TMetaAttribute::valid() before use.
    TMetaAttribute itemByName(const char* name) const {
        return TMetaAttribute(list_ ? list_->itemByName(name) : nullptr);
    }

    /// Subscript by name — same as @c itemByName(@p name).
    TMetaAttribute operator[](const char* name) const & {
        return TMetaAttribute(list_ ? list_->itemByName(name) : nullptr);
    }

    /// Replaces the attribute at @p index with @p attr. Returns @c true on success.
    bool setAt(int32_t index, TMetaAttribute&& attr) {
        return list_ && list_->setAt(index, attr.get()) == TRUE;
    }

    /// Inserts @p attr before @p index, shifting later items. Returns @c true on success.
    bool insert(int32_t index, TMetaAttribute&& attr) {
        return list_ && list_->insert(index, attr.get()) == TRUE;
    }

    /// Returns @c true when the list (and its parent @c Metadata) is immutable.
    bool immutable() const { return list_ && list_->immutable() == TRUE; }

    /// Removes the attribute at @p index. Returns @c true on success.
    bool remove(int32_t index) { return list_ && list_->remove(index) == TRUE; }

    /// Removes all attributes from the list. Returns @c true on success.
    bool clear() { return list_ && list_->clear() == TRUE; }
};

/**
 * Fluent wrapper for @c primo::codecs::MetaPicture.
 *
 * Owns the raw object through a @c primo::ref<MetaPicture>.
 * Non-copyable, movable.
 */
class TMetaPicture {
    primo::ref<primo::codecs::MetaPicture> pic_;

public:
    TMetaPicture()
        : pic_(primo::avblocks::Library::createMetaPicture()) {}

    explicit TMetaPicture(primo::ref<primo::codecs::MetaPicture> pic)
        : pic_(std::move(pic)) {}

    /// Wraps a non-owned @c MetaPicture pointer, retaining it.
    explicit TMetaPicture(primo::codecs::MetaPicture* pic)
        : pic_(pic) { if (pic) pic->retain(); }

    TMetaPicture(const TMetaPicture&) = delete;
    TMetaPicture& operator=(const TMetaPicture&) = delete;
    TMetaPicture(TMetaPicture&&) = default;
    TMetaPicture& operator=(TMetaPicture&&) = default;

    // -- Getters --

    /// Returns the MIME type as an ANSI string (see @c primo::codecs::MimeType constants).
    const char*                              mimeType()    const { return pic_ ? pic_->mimeType() : nullptr; }
    primo::codecs::MetaPictureType::Enum     pictureType() const { return pic_ ? pic_->pictureType() : primo::codecs::MetaPictureType::Other; }
    /// Returns the picture description as a UTF-8 @c std::string (empty if none).
    std::string description() const {
        return (pic_ && pic_->description())
            ? std::string(primo::ustring(pic_->description()))
            : std::string{};
    }
    const uint8_t* data()     const { return pic_ ? pic_->data()     : nullptr; }
    int32_t        dataSize() const { return pic_ ? pic_->dataSize() : 0; }
    bool           immutable() const { return pic_ && pic_->immutable() == TRUE; }

    // -- Fluent setters --

    TMetaPicture& mimeType(const char* mt)                          { if (pic_) pic_->setMimeType(mt); return *this; }
    TMetaPicture& pictureType(primo::codecs::MetaPictureType::Enum pt) { if (pic_) pic_->setPictureType(pt); return *this; }
    TMetaPicture& description(const char* desc) {
        if (pic_) pic_->setDescription(primo::ustring(desc));
        return *this;
    }
    TMetaPicture& data(const void* d, int32_t size) { if (pic_) pic_->setData(d, size); return *this; }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TMetaPicture clone() const {
        return TMetaPicture(primo::ref<primo::codecs::MetaPicture>(pic_->clone()));
    }

    /// Returns @c true when this handle wraps a non-null underlying object.
    bool valid() const { return pic_.get() != nullptr; }

    primo::codecs::MetaPicture* get() const { return pic_.get(); }
};

/**
 * Non-owning view of a @c MetaPictureList.
 *
 * Returned by @c TMetadata::pictures().
 */
class TMetaPictureList {
    primo::codecs::MetaPictureList* list_;

public:
    explicit TMetaPictureList(primo::codecs::MetaPictureList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMetaPicture) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        TMetaPictureList& list_;
        int32_t index_;
        /// Read — implicit conversion to a non-owning picture wrapper.
        operator TMetaPicture() const { return TMetaPicture(list_.list_->at(index_)); }
        /// Write — calls @c setAt; returns @c true on success.
        bool operator=(TMetaPicture&& p) { return list_.list_->setAt(index_, p.get()) == TRUE; }
    };

    /// Returns the number of pictures.
    int32_t count() const { return list_ ? list_->count() : 0; }

    /// Returns the picture at @p index (non-owning, retaining).
    TMetaPicture at(int32_t index) const { return TMetaPicture(list_->at(index)); }

    /// Non-const lvalue subscript — returns a proxy supporting read and @c setAt write.
    ItemRef operator[](int32_t index) & { return {*this, index}; }

    /// Const subscript — same as @c at(@p index).
    TMetaPicture operator[](int32_t index) const & { return TMetaPicture(list_->at(index)); }

    /// Replaces the picture at @p index with @p pic. Returns @c true on success.
    bool setAt(int32_t index, TMetaPicture&& pic) {
        return list_ && list_->setAt(index, pic.get()) == TRUE;
    }

    /// Inserts @p pic before @p index, shifting later items. Returns @c true on success.
    bool insert(int32_t index, TMetaPicture&& pic) {
        return list_ && list_->insert(index, pic.get()) == TRUE;
    }

    /// Returns @c true when the list (and its parent @c Metadata) is immutable.
    bool immutable() const { return list_ && list_->immutable() == TRUE; }

    /// Removes the picture at @p index. Returns @c true on success.
    bool remove(int32_t index) { return list_ && list_->remove(index) == TRUE; }

    /// Removes all pictures from the list. Returns @c true on success.
    bool clear() { return list_ && list_->clear() == TRUE; }
};

/**
 * Fluent wrapper for @c primo::codecs::Metadata.
 *
 * Owns the raw object through a @c primo::ref<Metadata>.
 * Non-copyable, movable. Obtained from @c TMediaSocketT::metadata() or
 * created directly via the default constructor.
 */
class TMetadata {
    primo::ref<primo::codecs::Metadata> meta_;

public:
    TMetadata()
        : meta_(primo::avblocks::Library::createMetadata()) {}

    explicit TMetadata(primo::ref<primo::codecs::Metadata> meta)
        : meta_(std::move(meta)) {}

    /// Wraps a non-owned @c Metadata pointer, retaining it.
    explicit TMetadata(primo::codecs::Metadata* meta)
        : meta_(meta) { if (meta) meta->retain(); }

    TMetadata(const TMetadata&) = delete;
    TMetadata& operator=(const TMetadata&) = delete;
    TMetadata(TMetadata&&) = default;
    TMetadata& operator=(TMetadata&&) = default;

    // -- List accessors --

    /// Returns a non-owning view of the attribute list.
    TMetaAttributeList attributes() const {
        return TMetaAttributeList(meta_ ? meta_->attributes() : nullptr);
    }

    /// Returns a non-owning view of the picture list.
    TMetaPictureList pictures() const {
        return TMetaPictureList(meta_ ? meta_->pictures() : nullptr);
    }

    // -- List mutation --

    /// Appends @p attr to the attribute list (rvalue @c this overload for chaining on temporaries).
    TMetadata&& addAttribute(TMetaAttribute&& attr) && {
        meta_->attributes()->add(attr.get());
        return std::move(*this);
    }

    /// Appends @p attr to the attribute list.
    TMetadata& addAttribute(TMetaAttribute&& attr) & {
        meta_->attributes()->add(attr.get());
        return *this;
    }

    /// Appends @p pic to the picture list (rvalue @c this overload for chaining on temporaries).
    TMetadata&& addPicture(TMetaPicture&& pic) && {
        meta_->pictures()->add(pic.get());
        return std::move(*this);
    }

    /// Appends @p pic to the picture list.
    TMetadata& addPicture(TMetaPicture&& pic) & {
        meta_->pictures()->add(pic.get());
        return *this;
    }

    // -- Getters --

    bool immutable() const { return meta_ && meta_->immutable() == TRUE; }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TMetadata clone() const {
        return TMetadata(primo::ref<primo::codecs::Metadata>(meta_->clone()));
    }

    /// Returns @c true when this handle wraps a non-null underlying object.
    bool valid() const { return meta_.get() != nullptr; }

    primo::codecs::Metadata* get() const { return meta_.get(); }
};

// ---- StreamInfo CRTP base and subclasses -------------------------------------

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

    /// Returns a mutable deep copy. The cloned object is always mutable regardless
    /// of whether the source was immutable.
    Derived clone() const {
        return Derived(primo::ref<RawT>(static_cast<RawT*>(info_->clone())));
    }

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

    /// Wraps a non-owned @c AudioStreamInfo pointer, retaining it.
    explicit TAudioStreamInfo(primo::codecs::AudioStreamInfo* info)
        : Base(info) {}

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

    /// Wraps a non-owned @c StreamInfo pointer, retaining it. Used by @c TMediaPin::streamInfo().
    explicit TDataStreamInfo(primo::codecs::StreamInfo* info)
        : Base(info) {}

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
    
    /// Returns the pin's stream info cast to @c TAudioStreamInfo, wrapping the existing pointer.
    TAudioStreamInfo audioStreamInfo() const {
        return TAudioStreamInfo(
            static_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo()));
    }

    /// Returns the pin's stream info cast to @c TVideoStreamInfo, wrapping the existing pointer.
    TVideoStreamInfo videoStreamInfo() const {
        return TVideoStreamInfo(
            static_cast<primo::codecs::VideoStreamInfo*>(pin_->streamInfo()));
    }

    /// Returns the pin's stream info as a @c TDataStreamInfo (base @c StreamInfo view),
    /// wrapping the existing pointer. Use to access @c mediaType(), @c streamType(),
    /// @c ID(), @c programNumber() without knowing the concrete media type.
    TDataStreamInfo streamInfo() const {
        return TDataStreamInfo(pin_->streamInfo());
    }

    /// Sets the pin's connection ID (fluent rvalue overload).
    /// Use @c PinConnection::Disabled to exclude this pin, @c PinConnection::Auto for automatic
    /// matching, or any positive integer to match with an output pin sharing the same ID.
    TMediaPin&& connection(int32_t connectionID) && {
        pin_->setConnection(connectionID);
        return std::move(*this);
    }

    /// Sets the pin's connection ID (fluent lvalue overload).
    TMediaPin& connection(int32_t connectionID) & {
        pin_->setConnection(connectionID);
        return *this;
    }

    /// Returns the pin's current connection ID.
    int32_t connection() const {
        return pin_->connection();
    }

    /// Adds an integer parameter to the pin's @c ParameterList (fluent).
    /// @p name is a codec parameter name constant, e.g. @c Param::Encoder::Audio::AAC::StereoMode.
    TMediaPin& addParam(const char* name, int64_t value) & {
        pin_->params()->addInt(name, value);
        return *this;
    }

    TMediaPin&& addParam(const char* name, int64_t value) && {
        pin_->params()->addInt(name, value);
        return std::move(*this);
    }

    primo::avblocks::MediaPin* get() const { return pin_.get(); }
};

/**
 * Non-owning view of a @c MediaPinList. Returned by @c TMediaSocketT::pins().
 */
class TMediaPinList {
    primo::avblocks::MediaPinList* list_;

public:
    /// Wraps a non-owned @c MediaPinList pointer.
    explicit TMediaPinList(primo::avblocks::MediaPinList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMediaPin) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        TMediaPinList& list_;
        int32_t index_;
        /// Read — implicit conversion to a non-owning pin wrapper.
        operator TMediaPin() const { return TMediaPin(list_.list_->at(index_)); }
        /// Write — calls @c setAt; returns @c true on success.
        bool operator=(TMediaPin&& p) { return list_.list_->setAt(index_, p.get()) == TRUE; }
    };

    /// Returns the number of pins in the collection.
    int32_t count() const { return list_ ? list_->count() : 0; }

    /// Returns the pin at @p index, wrapping the existing (non-owned) pointer.
    TMediaPin at(int32_t index) const { return TMediaPin(list_->at(index)); }

    /// Non-const lvalue subscript — returns a proxy supporting read and @c setAt write.
    ItemRef operator[](int32_t index) & { return {*this, index}; }

    /// Const subscript — same as @c at(@p index).
    TMediaPin operator[](int32_t index) const & { return TMediaPin(list_->at(index)); }

    /// Replaces the pin at @p index with @p pin. Returns @c true on success.
    bool setAt(int32_t index, TMediaPin&& pin) {
        return list_ && list_->setAt(index, pin.get()) == TRUE;
    }

    /// Inserts @p pin before @p index, shifting later items. Returns @c true on success.
    bool insert(int32_t index, TMediaPin&& pin) {
        return list_ && list_->insert(index, pin.get()) == TRUE;
    }

    /// Returns @c true when the list (and its parent @c MediaSocket) is immutable.
    bool immutable() const { return list_ && list_->immutable() == TRUE; }

    /// Removes the pin at @p index. Returns @c true on success.
    bool remove(int32_t index) { return list_ && list_->remove(index) == TRUE; }

    /// Removes all pins from the list. Returns @c true on success.
    bool clear() { return list_ && list_->clear() == TRUE; }
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

    /// Wraps a non-owned @c MediaSocket pointer, retaining it. Used by @c TMediaInfo::inputs() / @c outputs().
    explicit TMediaSocketT(primo::avblocks::MediaSocket* socket)
        : socket_(socket) { if (socket) socket->retain(); }

    /// Creates a socket from a @c TMediaInfo, wrapping @c Library::createMediaSocket(MediaInfo*).
    /// The created socket contains the file path and pins from the media info, ready to be
    /// added as a transcoder input. Member template so the body is instantiation-checked,
    /// allowing @c TMediaInfo to be defined after @c TMediaSocketT without a circular dependency.
    template<typename TMediaInfoArg>
    explicit TMediaSocketT(const TMediaInfoArg& info)
        : socket_(primo::avblocks::Library::createMediaSocket(info.get())) {
    }

    /// Creates a socket pre-configured with the named AVBlocks preset
    /// (e.g. @c Preset::Video::Generic::MP4::Base_H264_AAC).
    explicit TMediaSocketT(const char* preset)
        : socket_(primo::avblocks::Library::createMediaSocket(preset)) {}

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

    /// Clears the file path (pass @c nullptr).
    TMediaSocketT&& file(std::nullptr_t) && {
        socket_->setFile(nullptr);
        return std::move(*this);
    }

    /// Clears the file path (pass @c nullptr).
    TMediaSocketT& file(std::nullptr_t) & {
        socket_->setFile(nullptr);
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

    /// Sets the raw @c primo::Stream source/sink for this socket.
    /// Pass @c nullptr to clear a previously set stream.
    TMediaSocketT&& stream(primo::Stream* s) && {
        socket_->setStream(s);
        return std::move(*this);
    }

    /// Sets the raw @c primo::Stream source/sink for this socket.
    /// Pass @c nullptr to clear a previously set stream.
    TMediaSocketT& stream(primo::Stream* s) & {
        socket_->setStream(s);
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

    /// Sets the time position for this socket (seek before open). Returns @c false if the socket is immutable.
    TMediaSocketT&& timePosition(double time) && {
        socket_->setTimePosition(time);
        return std::move(*this);
    }

    /// Sets the time position for this socket (seek before open). Returns @c false if the socket is immutable.
    TMediaSocketT& timePosition(double time) & {
        socket_->setTimePosition(time);
        return *this;
    }

    /// @}

    /// Returns the stream type of this socket (read from the underlying @c MediaSocket).
    primo::codecs::StreamType::Enum streamType() const {
        return socket_->streamType();
    }

    /// Returns the stream sub-type of this socket (read from the underlying @c MediaSocket).
    primo::codecs::StreamSubType::Enum streamSubType() const {
        return socket_->streamSubType();
    }

    /// Returns a non-owning view of the pin list.
    TMediaPinList pins() const { return TMediaPinList(socket_->pins()); }

    /// Returns the pin at @p index, wrapping the existing (non-owned) pointer.
    TMediaPin pins(int32_t index) const {
        return TMediaPin(socket_->pins()->at(index));
    }

    /// Returns the metadata associated with this socket as a @c TMetadata wrapper.
    /// The returned object is invalid (@c valid() == false) if the socket has no metadata.
    TMetadata metadata() const { return TMetadata(socket_->metadata()); }

    primo::avblocks::MediaSocket* get() const { return socket_.get(); }
};

// Type aliases for convenience
using TMediaSocket = TMediaSocketT<char>;
using TMediaSocketW = TMediaSocketT<wchar_t>;

/**
 * Non-owning view of a @c MediaSocketList. Returned by @c TMediaInfo::inputs() / @c outputs().
 */
class TMediaSocketList {
    primo::avblocks::MediaSocketList* list_;

public:
    /// Wraps a non-owned @c MediaSocketList pointer.
    explicit TMediaSocketList(primo::avblocks::MediaSocketList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMediaSocket) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        TMediaSocketList& list_;
        int32_t index_;
        /// Read — implicit conversion to a non-owning socket wrapper.
        operator TMediaSocket() const { return TMediaSocket(list_.list_->at(index_)); }
        /// Write — calls @c setAt; returns @c true on success.
        bool operator=(TMediaSocket&& s) { return list_.list_->setAt(index_, s.get()) == TRUE; }
    };

    /// Returns the number of sockets in the collection.
    int32_t count() const { return list_ ? list_->count() : 0; }

    /// Returns the socket at @p index, wrapping the existing (non-owned) pointer.
    TMediaSocket at(int32_t index) const { return TMediaSocket(list_->at(index)); }

    /// Non-const lvalue subscript — returns a proxy supporting read and @c setAt write.
    ItemRef operator[](int32_t index) & { return {*this, index}; }

    /// Const subscript — same as @c at(@p index).
    TMediaSocket operator[](int32_t index) const & { return TMediaSocket(list_->at(index)); }

    /// Replaces the socket at @p index with @p socket. Returns @c true on success.
    bool setAt(int32_t index, TMediaSocket&& socket) {
        return list_ && list_->setAt(index, socket.get()) == TRUE;
    }

    /// Inserts @p socket before @p index, shifting later items. Returns @c true on success.
    bool insert(int32_t index, TMediaSocket&& socket) {
        return list_ && list_->insert(index, socket.get()) == TRUE;
    }

    /// Returns @c true when the list (and its parent @c Transcoder) is immutable.
    bool immutable() const { return list_ && list_->immutable() == TRUE; }

    /// Removes the socket at @p index. Returns @c true on success.
    bool remove(int32_t index) { return list_ && list_->remove(index) == TRUE; }

    /// Removes all sockets from the list. Returns @c true on success.
    bool clear() { return list_ && list_->clear() == TRUE; }
};

template<typename CharT = char>
class TMediaInfoT {
    primo::ref<primo::avblocks::MediaInfo> info_;

public:
    TMediaInfoT()
        : info_(primo::avblocks::Library::createMediaInfo()) {}

    // Delete copy operations
    TMediaInfoT(const TMediaInfoT&) = delete;
    TMediaInfoT& operator=(const TMediaInfoT&) = delete;

    // Enable move operations
    TMediaInfoT(TMediaInfoT&&) = default;
    TMediaInfoT& operator=(TMediaInfoT&&) = default;

    /// Opens the MediaInfo, throwing @c TAVBlocksException on failure.
    TMediaInfoT& open() {
        if (!info_->open()) {
            throw TAVBlocksException("Failed to open MediaInfo", TErrorInfo(info_->error()));
        }
        return *this;
    }

    /// Opens the MediaInfo. Returns @c true on success, @c false on failure.
    bool tryOpen() {
        return info_->open() == TRUE;
    }

    /// Returns @c true when stream info and metadata have been fully parsed in push mode.
    bool isReady() { return info_->isReady() == TRUE; }

    /// Pushes a chunk of input data for the socket at @p inputIndex.
    /// Returns @c true on success.
    bool push(int32_t inputIndex, TMediaSample& sample) {
        return info_->push(inputIndex, sample.get()) == TRUE;
    }

    /// Signals end-of-stream on the input socket at @p inputIndex.
    /// Equivalent to pushing a null (empty) sample.
    bool pushEos(int32_t inputIndex) {
        return info_->push(inputIndex, nullptr) == TRUE;
    }

    TErrorInfo error() const {
        return TErrorInfo(info_->error());
    }

    /// Returns a non-owning view of the input socket list.
    TMediaSocketList inputs() const { return TMediaSocketList(info_->inputs()); }

    /// Returns the input socket at @p index, wrapping the existing (non-owned) pointer.
    TMediaSocketT<CharT> inputs(int32_t index) const {
        return TMediaSocketT<CharT>(info_->inputs()->at(index));
    }

    /// Returns a non-owning view of the output socket list.
    TMediaSocketList outputs() const { return TMediaSocketList(info_->outputs()); }

    /// Returns the output socket at @p index, wrapping the existing (non-owned) pointer.
    TMediaSocketT<CharT> outputs(int32_t index) const {
        return TMediaSocketT<CharT>(info_->outputs()->at(index));
    }

    primo::avblocks::MediaInfo* get() const { return info_.get(); }
};

// Type aliases for convenience
using TMediaInfo  = TMediaInfoT<char>;
using TMediaInfoW = TMediaInfoT<wchar_t>;

/**
 * Internal adapter that bridges @c std::function callbacks to the raw
 * @c primo::avblocks::TranscoderCallback interface.
 *
 * Owned by @c TTranscoderT via @c std::unique_ptr so its address stays
 * stable across moves.  Only allocated when at least one lambda is set.
 */
class TTranscoderCallback : public primo::avblocks::TranscoderCallback {
    std::function<void(double, double)>                              onProgress_;
    std::function<void(primo::avblocks::TranscoderStatus::Enum)>     onStatus_;
    std::function<bool(double)>                                      onContinue_;
    std::function<void(int32_t)>                                     onInputChange_;
public:
    /// @name Setters
    /// @{
    void setOnProgress(std::function<void(double, double)> f)   { onProgress_    = std::move(f); }
    void setOnStatus(std::function<void(primo::avblocks::TranscoderStatus::Enum)> f) { onStatus_ = std::move(f); }
    void setOnContinue(std::function<bool(double)> f)            { onContinue_    = std::move(f); }
    void setOnInputChange(std::function<void(int32_t)> f)        { onInputChange_ = std::move(f); }
    /// @}

    void onProgress(double cur, double total) override {
        if (onProgress_) onProgress_(cur, total);
    }
    void onStatus(primo::avblocks::TranscoderStatus::Enum s) override {
        if (onStatus_) onStatus_(s);
    }
    bool_t onContinue(double cur) override {
        return onContinue_ ? (onContinue_(cur) ? TRUE : FALSE) : TRUE;
    }
    void onInputChange(int32_t idx) override {
        if (onInputChange_) onInputChange_(idx);
    }
};

template<typename CharT = char>
class TTranscoderT {
    // callback_ must be declared BEFORE transcoder_ so that it is destroyed
    // AFTER the raw Transcoder (which holds a raw pointer to the callback).
    std::unique_ptr<TTranscoderCallback>   callback_;
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
    
    /// Returns the callback adapter, creating and installing it on first use.
    TTranscoderCallback& ensureCallback() {
        if (!callback_) {
            callback_ = std::make_unique<TTranscoderCallback>();
            transcoder_->setCallback(callback_.get());
        }
        return *callback_;
    }

    TTranscoderT& allowDemoMode(bool allow = true) {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }

    /// Sets a lambda invoked periodically during @c run() with @c (currentTime, totalTime).
    TTranscoderT& onProgress(std::function<void(double, double)> f) {
        ensureCallback().setOnProgress(std::move(f));
        return *this;
    }

    /// Sets a lambda invoked on completion status during @c run().
    TTranscoderT& onStatus(std::function<void(primo::avblocks::TranscoderStatus::Enum)> f) {
        ensureCallback().setOnStatus(std::move(f));
        return *this;
    }

    /// Sets a lambda controlling whether @c run() continues.
    /// Return @c true to continue, @c false to stop gracefully.
    TTranscoderT& onContinue(std::function<bool(double)> f) {
        ensureCallback().setOnContinue(std::move(f));
        return *this;
    }

    /// Sets a lambda invoked when an input format is detected or changed during push mode.
    TTranscoderT& onInputChange(std::function<void(int32_t)> f) {
        ensureCallback().setOnInputChange(std::move(f));
        return *this;
    }
    
    TTranscoderT& addInput(const TMediaSocketT<CharT>& socket) {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }

    /// Removes the input socket at @p index.
    TTranscoderT& removeInput(int32_t index) {
        transcoder_->inputs()->remove(index);
        return *this;
    }

    TTranscoderT& addOutput(const TMediaSocketT<CharT>& socket) {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }

    /// Removes the output socket at @p index.
    TTranscoderT& removeOutput(int32_t index) {
        transcoder_->outputs()->remove(index);
        return *this;
    }

    /// Returns a non-owning view of the input socket list.
    TMediaSocketList inputs() const { return TMediaSocketList(transcoder_->inputs()); }

    /// Returns the input socket at @p index, wrapping the existing (non-owned) pointer.
    TMediaSocketT<CharT> inputs(int32_t index) const {
        return TMediaSocketT<CharT>(transcoder_->inputs()->at(index));
    }

    /// Returns a non-owning view of the output socket list.
    TMediaSocketList outputs() const { return TMediaSocketList(transcoder_->outputs()); }

    /// Returns the output socket at @p index, wrapping the existing (non-owned) pointer.
    TMediaSocketT<CharT> outputs(int32_t index) const {
        return TMediaSocketT<CharT>(transcoder_->outputs()->at(index));
    }

    TTranscoderT& open() {
        if (!transcoder_->open()) {
            throw TAVBlocksException("Failed to open transcoder", TErrorInfo(transcoder_->error()));
        }
        return *this;
    }

    /// Opens the transcoder without throwing. Returns @c true on success.
    bool tryOpen() {
        return transcoder_->open() == TRUE;
    }
    
    bool pull(int32_t& outputIndex, TMediaSample& sample) {
        return transcoder_->pull(outputIndex, sample.get()) == TRUE;
    }

    bool push(int32_t inputIndex, TMediaSample& sample) {
        return transcoder_->push(inputIndex, sample.get()) == TRUE;
    }

    /// Signals end-of-stream on the input socket at @p inputIndex.
    /// Equivalent to pushing a null (empty) sample.
    bool pushEos(int32_t inputIndex) {
        return transcoder_->push(inputIndex, nullptr) == TRUE;
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