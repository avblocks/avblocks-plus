#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <concepts>
#include <type_traits>
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

/**
 * RAII owner of the AVBlocks library and a static facade over @c primo::avblocks::Library.
 *
 * Constructing a @c TLibrary initializes the library and destroying it shuts the library down. Create one
 * instance before using any other class in this header and keep it alive for as long as AVBlocks is used.
 * The static members mirror the functions of the @c primo::avblocks::Library namespace.
 *
 * Not copyable and not movable.
 * @code
 * TLibrary lib;
 * std::cout << TLibrary::description() << std::endl;
 * @endcode
 */
class TLibrary {
public:
    /// Initializes the AVBlocks library.
    TLibrary() {
        primo::avblocks::Library::initialize();
    }
    
    /// Shuts the AVBlocks library down and frees the resources it uses.
    ~TLibrary() {
        primo::avblocks::Library::shutdown();
    }
    
    TLibrary(const TLibrary&) = delete;
    TLibrary& operator=(const TLibrary&) = delete;
    TLibrary(TLibrary&&) = delete;
    TLibrary& operator=(TLibrary&&) = delete;

    // ---- Static accessors mirroring primo::avblocks::Library namespace ----

    /**
     * Initializes the library. Must be called once before the library is used unless a @c TLibrary instance
     * is created.
     * @return @c true if the library is initialized successfully.
     */
    static bool initialize() {
        return primo::avblocks::Library::initialize() == TRUE;
    }

    /// Closes the library and frees the resources it uses. Call it when the library is not needed anymore.
    static void shutdown() {
        primo::avblocks::Library::shutdown();
    }

    /**
     * Returns the build description: full version, architecture, configuration (Release/Debug),
     * license (OEM/Demo) and platform.
     */
    static const char* description() {
        return primo::avblocks::Library::description();
    }

    /// Returns the library major version.
    static int32_t majorVersion() {
        return primo::avblocks::Library::majorVersion();
    }

    /// Returns the library minor version.
    static int32_t minorVersion() {
        return primo::avblocks::Library::minorVersion();
    }

    /// Returns the library patch version. It changes for bug fixes and small improvements that do not affect the API.
    static int32_t patchVersion() {
        return primo::avblocks::Library::patchVersion();
    }

    /**
     * Sets whether TLS (Transport Layer Security) is used to validate the license. TLS is not used by default.
     * Call it before @c setLicense() to have an effect.
     * @param useTls @c true to validate the license over TLS.
     */
    static void setLicenseTls(bool useTls) {
        primo::avblocks::Library::setLicenseTls(useTls ? TRUE : FALSE);
    }

    /**
     * Sets a license string. The license is appended to the library license state.
     * @param license The license string. If @c nullptr the library license state is cleared.
     * @return The license status after the license is set, as a combination of
     *         @c primo::license::LicenseStatusFlags::Enum flags.
     */
    static int32_t setLicense(const char* license) {
        return primo::avblocks::Library::setLicense(license);
    }
};

/**
 * Fluent wrapper for @c primo::license::LicenseInfo, which describes the license state of the library.
 *
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TLicenseInfo {
    primo::ref<primo::license::LicenseInfo> info_;

public:
    /**
     * Creates a license info object for the library (@c Library::createLicenseInfo).
     */
    TLicenseInfo()
        : info_(primo::avblocks::Library::createLicenseInfo()) {}

    /**
     * Takes ownership of an existing @c LicenseInfo reference.
     */
    explicit TLicenseInfo(primo::ref<primo::license::LicenseInfo> info)
        : info_(std::move(info)) {}

    // Non-copyable, movable
    TLicenseInfo(const TLicenseInfo&) = delete;
    TLicenseInfo& operator=(const TLicenseInfo&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TLicenseInfo(TLicenseInfo&&) = default;
    TLicenseInfo& operator=(TLicenseInfo&&) = default;
    /// @}

    /// Returns the current license status as a combination of LicenseStatusFlags::Enum flags.
    primo::license::LicenseStatusFlags::Enum licenseStatus() const {
        return static_cast<primo::license::LicenseStatusFlags::Enum>(info_->licenseStatus());
    }

    /// Returns true if the specified product/feature is licensed.
    /// Pass nullptr for product/feature to use library defaults.
    bool isLicensed(const char* product = nullptr, const char* feature = nullptr) const {
        return info_->isLicensed(product, feature) == TRUE;
    }

    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::license::LicenseInfo* get() const { return info_.get(); }
};

/**
 * Copy of the error information reported by AVBlocks: facility, code, message and hint.
 *
 * The strings are copied out of the source @c primo::error::ErrorInfo, so a @c TErrorInfo stays valid after
 * the source object is gone. A default-constructed @c TErrorInfo is empty.
 */
class TErrorInfo {
    int32_t facility_{};
    int32_t code_{};
    std::string message_;
    std::string hint_;

public:
    /// Creates an empty error info: zero facility and code, empty message and hint.
    TErrorInfo() = default;

    /**
     * Copies the error information from @p error.
     * @param error The error to copy. If @c nullptr the result is an empty error info.
     */
    explicit TErrorInfo(const primo::error::ErrorInfo* error) {
        if (error) {
            facility_ = error->facility();
            code_     = error->code();
            if (error->message()) message_ = std::string(primo::ustring(error->message()));
            if (error->hint())    hint_    = std::string(primo::ustring(error->hint()));
        }
    }

    /// Returns the error facility, i.e. the component that reports the error.
    int32_t            facility() const { return facility_; }
    /// Returns the error code. Its meaning depends on the facility.
    int32_t            code()     const { return code_; }
    /// Returns the error message, a textual representation of the error code. Empty if not set.
    const std::string& message()  const { return message_; }
    /// Returns a diagnostic hint, suitable for error logs and debugging. Empty if not set.
    const std::string& hint()     const { return hint_; }
};

/**
 * Exception thrown when an AVBlocks operation fails.
 *
 * @c what() returns the context of the failure followed by the error message, when there is one.
 * Use @c error() to get the complete error information.
 */
class TAVBlocksException : public std::runtime_error {
    TErrorInfo error_;
public:
    /**
     * Creates the exception.
     * @param context Short description of the failed operation, e.g. "Failed to open transcoder".
     * @param error   Error information reported by AVBlocks.
     */
    TAVBlocksException(const char* context, const TErrorInfo& error)
        : std::runtime_error(
            error.message().empty()
                ? std::string(context)
                : std::string(context) + ": " + error.message())
        , error_(error) {}

    /// Returns the error information reported by AVBlocks.
    const TErrorInfo& error() const { return error_; }
};

/**
 * Fluent wrapper for @c primo::codecs::MediaBuffer, a buffer for media data.
 *
 * A buffer has a capacity, which is the size of its memory block, and a data range, the part of the block that
 * holds valid data. The valid data is contiguous and can start at any offset in the block.
 *
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TMediaBuffer {
    primo::ref<primo::codecs::MediaBuffer> buffer_;

public:
    /// Creates an empty buffer with zero capacity.
    TMediaBuffer()
        : buffer_(primo::avblocks::Library::createMediaBuffer()) {}

    /**
     * Creates a buffer with internal storage.
     * @param capacity Buffer capacity in bytes.
     */
    explicit TMediaBuffer(int32_t capacity)
        : buffer_(primo::avblocks::Library::createMediaBuffer(capacity)) {}

    /**
     * Takes ownership of an existing @c MediaBuffer reference.
     */
    explicit TMediaBuffer(primo::ref<primo::codecs::MediaBuffer> buffer)
        : buffer_(std::move(buffer)) {}

    /**
     * Wraps a non-owned @c MediaBuffer pointer, retaining it.
     */
    explicit TMediaBuffer(primo::codecs::MediaBuffer* buffer)
        : buffer_(buffer) { if (buffer) buffer->retain(); }

    // Delete copy operations
    TMediaBuffer(const TMediaBuffer&) = delete;
    TMediaBuffer& operator=(const TMediaBuffer&) = delete;

    // Enable move operations
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMediaBuffer(TMediaBuffer&&) = default;
    TMediaBuffer& operator=(TMediaBuffer&&) = default;
    /// @}

    /// @{
    /**
     * Attaches an external memory block as the buffer storage. Any internal storage is freed.
     * The block is not copied and must stay valid for as long as it is attached.
     * @param data Pointer to the memory block.
     * @param size Size of the block in bytes.
     * @param copy When @c true (default) the whole block is marked as valid data, i.e. @c dataSize() equals
     *             @p size. When @c false the data size is 0.
     * @return This object, for chaining.
     */
    TMediaBuffer&& attach(const uint8_t* data, size_t size, bool copy = true) && {
        buffer_->attach(const_cast<uint8_t*>(data), static_cast<int32_t>(size), copy ? TRUE : FALSE);
        return std::move(*this);
    }

    TMediaBuffer& attach(const uint8_t* data, size_t size, bool copy = true) & {
        buffer_->attach(const_cast<uint8_t*>(data), static_cast<int32_t>(size), copy ? TRUE : FALSE);
        return *this;
    }
    /// @}

    // -- Getters --

    /**
     * Returns a pointer to the start of the buffer block. It is valid only when @c capacity() is greater than 0.
     */
    uint8_t*    start()           const { return buffer_->start(); }
    /// Returns the size of the buffer block in bytes. Can be zero.
    int32_t     capacity()        const { return buffer_->capacity(); }
    /**
     * Returns @c true if the buffer uses an external memory block attached with @c attach().
     */
    bool        external()        const { return buffer_->external() == TRUE; }
    /**
     * Returns a pointer to the first byte of valid data. Same as @c start() + @c dataOffset().
     */
    const uint8_t* data()         const { return buffer_->data(); }
    /**
     * Returns the size of the valid data in bytes, from 0 to @c capacity().
     */
    int32_t     dataSize()        const { return buffer_->dataSize(); }
    /// Returns the offset in bytes at which the valid data starts in the buffer block.
    int32_t     dataOffset()      const { return buffer_->dataOffset(); }
    /**
     * Returns the free space in bytes that can be used to append data without reallocation: @c capacity() - @c dataSize().
     */
    int32_t     freeSpace()       const { return buffer_->freeSpace(); }
    /**
     * Returns the contiguous free space in bytes after the valid data, which can be used to append data without
     * @c normalize(): @c capacity() - @c dataSize() - @c dataOffset(). It is at most @c freeSpace().
     */
    int32_t     freeLinearSpace() const { return buffer_->freeLinearSpace(); }

    // -- Fluent setter --

    /**
     * Sets the range of valid data. Both values are validated so that the range is always inside the buffer.
     * @param offset Offset at which valid data starts, from 0 to @c capacity() - 1.
     * @param size   Size of the valid data, from 0 to @c capacity().
     * @return This object, for chaining.
     */
    TMediaBuffer& setData(int32_t offset, int32_t size) {
        buffer_->setData(offset, size);
        return *this;
    }

    // -- Operations --

    /**
     * Allocates internal storage. Storage is allocated only if @p size exceeds the current capacity;
     * the actual capacity may be larger than requested.
     * @param size     Requested size in bytes.
     * @param keepData Whether to keep the existing data when the storage is reallocated.
     * @return @c true on success.
     */
    bool alloc(int32_t size, bool keepData) {
        return buffer_->alloc(size, keepData ? TRUE : FALSE) == TRUE;
    }

    /**
     * Allocates internal storage with the specified memory alignment. Storage is allocated only if @p size
     * exceeds the current capacity.
     * @param alignment Alignment in bytes. Must be a power of 2 and at least 8.
     * @param size      Requested size in bytes.
     * @param keepData  Whether to keep the existing data when the storage is reallocated.
     * @return @c true on success.
     */
    bool alignedAlloc(int32_t alignment, int32_t size, bool keepData) {
        return buffer_->alignedAlloc(alignment, size, keepData ? TRUE : FALSE) == TRUE;
    }

    /// Frees the internal storage. Does nothing if an external memory block is attached. Returns this object, for chaining.
    TMediaBuffer& free() {
        buffer_->free();
        return *this;
    }

    /// Clears the data. The capacity does not change. Returns this object, for chaining.
    TMediaBuffer& clear() {
        buffer_->clear();
        return *this;
    }

    /**
     * Moves the valid data to the start of the buffer, which maximizes the free linear space. Useful after data
     * is added by copying it directly after the existing data and then extended with @c setData().
     * The capacity does not change. Returns this object, for chaining.
     */
    TMediaBuffer& normalize() {
        buffer_->normalize();
        return *this;
    }

    /**
     * Appends data to the end of the valid data. Internal storage grows when needed; external storage does not.
     * @param data     Pointer to the data to append.
     * @param dataSize Number of bytes to append.
     * @return @c true on success.
     */
    bool append(const void* data, int32_t dataSize) {
        return buffer_->append(data, dataSize) == TRUE;
    }

    /**
     * Removes data from the beginning of the valid data. The capacity does not change.
     * @param dataSize Number of bytes to remove.
     * @return This object, for chaining.
     */
    TMediaBuffer& remove(int32_t dataSize) {
        buffer_->remove(dataSize);
        return *this;
    }

    /**
     * Detaches the external memory block. Returns a pointer to it, or @c nullptr if none was attached.
     */
    uint8_t* detach() {
        return buffer_->detach();
    }

    /// Creates a deep copy. If the buffer uses an external memory block only the pointer to it is copied.
    TMediaBuffer clone() const {
        return TMediaBuffer(primo::ref<primo::codecs::MediaBuffer>(buffer_->clone()));
    }

    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::codecs::MediaBuffer* get() const { return buffer_.get(); }
};

/**
 * Fluent wrapper for @c primo::codecs::MediaSample, one or more samples of audio or video data.
 *
 * Holds compressed or uncompressed data in a @c TMediaBuffer together with timing and stream information.
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TMediaSample {
    primo::ref<primo::codecs::MediaSample> sample_;
    
public:
    /// Creates an empty media sample.
    TMediaSample() 
        : sample_(primo::avblocks::Library::createMediaSample()) {}
    
    /**
     * Takes ownership of an existing @c MediaSample reference.
     */
    explicit TMediaSample(primo::ref<primo::codecs::MediaSample> sample)
        : sample_(std::move(sample)) {}
    
    // Delete copy operations
    TMediaSample(const TMediaSample&) = delete;
    TMediaSample& operator=(const TMediaSample&) = delete;
    
    // Enable move operations
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMediaSample(TMediaSample&&) = default;
    TMediaSample& operator=(TMediaSample&&) = default;
    /// @}

    // -- Fluent buffer setters --

    /// @{
    /**
     * Sets the buffer of the sample. The sample retains the buffer and releases the previous one.
     * @param buf The new buffer.
     * @return This object, for chaining.
     */
    TMediaSample&& buffer(TMediaBuffer&& buf) && {
        sample_->setBuffer(buf.get());
        return std::move(*this);
    }

    TMediaSample& buffer(TMediaBuffer&& buf) & {
        sample_->setBuffer(buf.get());
        return *this;
    }
    /// @}

    /// Removes the buffer from the sample. Returns this object, for chaining.
    TMediaSample& buffer(std::nullptr_t) {
        sample_->setBuffer(nullptr);
        return *this;
    }

    // -- Const getters --

    /// Returns the buffer of the sample. The returned wrapper is invalid if the sample has no buffer.
    TMediaBuffer buffer() const {
        return TMediaBuffer(sample_->buffer());
    }

    /// Returns the presentation timestamp (PTS) of the sample in seconds.
    double  startTime()    const { return sample_->startTime(); }
    /// Returns the end time of the sample in seconds. It is optional and valid only if greater than the start time.
    double  endTime()      const { return sample_->endTime(); }
    /**
     * Returns the sample flags, a combination of @c primo::codecs::MediaSampleFlags::Enum values.
     */
    int32_t flags()        const { return sample_->flags(); }
    /// Returns the zero-based index of the elementary stream in the container to which the sample belongs.
    int32_t streamNumber() const { return sample_->streamNumber(); }

    /// Returns the picture type (I/P/B/etc.) of a demuxed, decoded or encoded frame. Currently set only by the MPEG-2 encoder.
    primo::codecs::PictureType::Enum pictureType() const {
        return sample_->pictureType();
    }

    /// Returns the frame type of a demuxed, decoded or encoded frame. Currently used only by the G.711 a-law and u-law codecs.
    primo::codecs::FrameType::Enum frameType() const {
        return sample_->frameType();
    }

    /**
     * Calculates the exact number of bytes needed for a video frame with the given properties.
     * @param width       Frame width in pixels.
     * @param height      Frame height in pixels.
     * @param colorFormat How colors are coded in the frame.
     * @return The size in bytes, or 0 if it cannot be calculated.
     */
    int32_t videoBufferSizeInBytes(int32_t width, int32_t height,
                                   primo::codecs::ColorFormat::Enum colorFormat) const {
        return sample_->videoBufferSizeInBytes(width, height, colorFormat);
    }

    // -- Fluent setters --

    /**
     * Sets the presentation timestamp (PTS) of the sample.
     * @param time Time in seconds.
     * @return This object, for chaining.
     */
    TMediaSample& startTime(double time)    { sample_->setStartTime(time); return *this; }
    /**
     * Sets the end time of the sample. It is optional and valid only if greater than the start time.
     * @param time Time in seconds.
     * @return This object, for chaining.
     */
    TMediaSample& endTime(double time)      { sample_->setEndTime(time);   return *this; }
    /**
     * Sets the sample flags.
     * @param f A combination of @c primo::codecs::MediaSampleFlags::Enum values.
     * @return This object, for chaining.
     */
    TMediaSample& flags(int32_t f)          { sample_->setFlags(f);        return *this; }
    /**
     * Sets the elementary stream to which the sample belongs.
     * @param n Zero-based index of the elementary stream in the container.
     * @return This object, for chaining.
     */
    TMediaSample& streamNumber(int32_t n)   { sample_->setStreamNumber(n); return *this; }

    /**
     * Sets the required picture type (I/P/B/etc.). Currently ignored by all encoders.
     * @return This object, for chaining.
     */
    TMediaSample& pictureType(primo::codecs::PictureType::Enum pt) {
        sample_->setPictureType(pt);
        return *this;
    }

    /**
     * Sets the frame type. Currently used only by the G.711 a-law and u-law codecs.
     * @return This object, for chaining.
     */
    TMediaSample& frameType(primo::codecs::FrameType::Enum ft) {
        sample_->setFrameType(ft);
        return *this;
    }

    // -- Operations --

    /// Resets the sample to its default values and clears the data in its buffer. Returns this object, for chaining.
    TMediaSample& reset() {
        sample_->reset();
        return *this;
    }

    /// Creates a deep copy of the sample.
    TMediaSample clone() const {
        return TMediaSample(primo::ref<primo::codecs::MediaSample>(sample_->clone()));
    }

    /// Returns the raw underlying pointer. For interop with the core API only.
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
    /// Creates an empty meta attribute.
    TMetaAttribute()
        : attr_(primo::avblocks::Library::createMetaAttribute()) {}

    /**
     * Takes ownership of an existing @c MetaAttribute reference.
     */
    explicit TMetaAttribute(primo::ref<primo::codecs::MetaAttribute> attr)
        : attr_(std::move(attr)) {}

    /// Wraps a non-owned @c MetaAttribute pointer, retaining it.
    explicit TMetaAttribute(primo::codecs::MetaAttribute* attr)
        : attr_(attr) { if (attr) attr->retain(); }

    TMetaAttribute(const TMetaAttribute&) = delete;
    TMetaAttribute& operator=(const TMetaAttribute&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMetaAttribute(TMetaAttribute&&) = default;
    TMetaAttribute& operator=(TMetaAttribute&&) = default;
    /// @}

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

    /// Returns the raw underlying pointer. For interop with the core API only.
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
    /**
     * Wraps a non-owned @c MetaAttributeList pointer.
     */
    explicit TMetaAttributeList(primo::codecs::MetaAttributeList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMetaAttribute) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        /// The list this proxy refers to.
        TMetaAttributeList& list_;
        /// Index of the item the proxy refers to.
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
    /// Creates an empty meta picture.
    TMetaPicture()
        : pic_(primo::avblocks::Library::createMetaPicture()) {}

    /**
     * Takes ownership of an existing @c MetaPicture reference.
     */
    explicit TMetaPicture(primo::ref<primo::codecs::MetaPicture> pic)
        : pic_(std::move(pic)) {}

    /// Wraps a non-owned @c MetaPicture pointer, retaining it.
    explicit TMetaPicture(primo::codecs::MetaPicture* pic)
        : pic_(pic) { if (pic) pic->retain(); }

    TMetaPicture(const TMetaPicture&) = delete;
    TMetaPicture& operator=(const TMetaPicture&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMetaPicture(TMetaPicture&&) = default;
    TMetaPicture& operator=(TMetaPicture&&) = default;
    /// @}

    // -- Getters --

    /// Returns the MIME type as an ANSI string (see @c primo::codecs::MimeType constants).
    const char*                              mimeType()    const { return pic_ ? pic_->mimeType() : nullptr; }
    /// Returns the picture type (front cover, back cover, artist, etc.).
    primo::codecs::MetaPictureType::Enum     pictureType() const { return pic_ ? pic_->pictureType() : primo::codecs::MetaPictureType::Other; }
    /// Returns the picture description as a UTF-8 @c std::string (empty if none).
    std::string description() const {
        return (pic_ && pic_->description())
            ? std::string(primo::ustring(pic_->description()))
            : std::string{};
    }
    /**
     * Returns a pointer to the image data (jpeg, png, etc., as given by @c mimeType()). The size is @c dataSize().
     */
    const uint8_t* data()     const { return pic_ ? pic_->data()     : nullptr; }
    /// Returns the number of image bytes.
    int32_t        dataSize() const { return pic_ ? pic_->dataSize() : 0; }
    /**
     * Returns @c true if the picture is immutable.
     */
    bool           immutable() const { return pic_ && pic_->immutable() == TRUE; }

    // -- Fluent setters --

    /**
     * Sets the MIME type (see @c primo::codecs::MimeType constants).
     * @return This object, for chaining.
     */
    TMetaPicture& mimeType(const char* mt)                          { if (pic_) pic_->setMimeType(mt); return *this; }
    /**
     * Sets the picture type (front cover, back cover, artist, etc.).
     * @return This object, for chaining.
     */
    TMetaPicture& pictureType(primo::codecs::MetaPictureType::Enum pt) { if (pic_) pic_->setPictureType(pt); return *this; }
    /**
     * Sets the picture description (@p desc is a UTF-8 C-string). Pass @c nullptr to clear it.
     * @return This object, for chaining.
     */
    TMetaPicture& description(const char* desc) {
        if (pic_) pic_->setDescription(primo::ustring(desc));
        return *this;
    }
    /**
     * Sets the image data. The data is copied.
     * @param d    Pointer to the image data.
     * @param size Size of the image in bytes.
     * @return This object, for chaining.
     */
    TMetaPicture& data(const void* d, int32_t size) { if (pic_) pic_->setData(d, size); return *this; }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TMetaPicture clone() const {
        return TMetaPicture(primo::ref<primo::codecs::MetaPicture>(pic_->clone()));
    }

    /// Returns @c true when this handle wraps a non-null underlying object.
    bool valid() const { return pic_.get() != nullptr; }

    /// Returns the raw underlying pointer. For interop with the core API only.
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
    /**
     * Wraps a non-owned @c MetaPictureList pointer.
     */
    explicit TMetaPictureList(primo::codecs::MetaPictureList* list) : list_(list) {}

    /**
     * Proxy returned by the non-const lvalue @c operator[](int32_t).
     * Supports both read (implicit conversion to @c TMetaPicture) and
     * write (@c operator= calls @c setAt on the parent list).
     */
    struct ItemRef {
        /// The list this proxy refers to.
        TMetaPictureList& list_;
        /// Index of the item the proxy refers to.
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
    /// Creates an empty metadata object.
    TMetadata()
        : meta_(primo::avblocks::Library::createMetadata()) {}

    /**
     * Takes ownership of an existing @c Metadata reference.
     */
    explicit TMetadata(primo::ref<primo::codecs::Metadata> meta)
        : meta_(std::move(meta)) {}

    /// Wraps a non-owned @c Metadata pointer, retaining it.
    explicit TMetadata(primo::codecs::Metadata* meta)
        : meta_(meta) { if (meta) meta->retain(); }

    TMetadata(const TMetadata&) = delete;
    TMetadata& operator=(const TMetadata&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMetadata(TMetadata&&) = default;
    TMetadata& operator=(TMetadata&&) = default;
    /// @}

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

    /// @{
    /**
     * Appends an attribute to the attribute list.
     * @param attr The attribute to add. The list retains it.
     * @return This object, for chaining.
     */
    TMetadata&& addAttribute(TMetaAttribute&& attr) && {
        meta_->attributes()->add(attr.get());
        return std::move(*this);
    }

    TMetadata& addAttribute(TMetaAttribute&& attr) & {
        meta_->attributes()->add(attr.get());
        return *this;
    }
    /// @}

    /// @{
    /**
     * Appends a picture to the picture list.
     * @param pic The picture to add. The list retains it.
     * @return This object, for chaining.
     */
    TMetadata&& addPicture(TMetaPicture&& pic) && {
        meta_->pictures()->add(pic.get());
        return std::move(*this);
    }

    TMetadata& addPicture(TMetaPicture&& pic) & {
        meta_->pictures()->add(pic.get());
        return *this;
    }
    /// @}

    // -- Getters --

    /**
     * Returns @c true if the metadata is immutable.
     */
    bool immutable() const { return meta_ && meta_->immutable() == TRUE; }

    // -- Operations --

    /// Creates a full independent clone. The returned object is always mutable.
    TMetadata clone() const {
        return TMetadata(primo::ref<primo::codecs::Metadata>(meta_->clone()));
    }

    /// Returns @c true when this handle wraps a non-null underlying object.
    bool valid() const { return meta_.get() != nullptr; }

    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::codecs::Metadata* get() const { return meta_.get(); }
};

// ---- StreamInfo CRTP base and subclasses -------------------------------------

/**
 * CRTP base for @c TAudioStreamInfo, @c TVideoStreamInfo, and @c TDataStreamInfo.
 *
 * Holds the @c primo::ref<RawT> and exposes all properties defined on the
 * raw @c primo::codecs::StreamInfo interface, with fluent setters that return
 * @c Derived& (on lvalues) or @c Derived&& (on temporaries) so chaining works naturally
 * on the concrete subclass. The rvalue overloads let a chained temporary be returned or
 * moved, e.g. @c return @c TVideoStreamInfo().frameWidth(352).frameHeight(288);
 */
template<typename Derived, typename RawT>
class TStreamInfo {
protected:
    /// The wrapped raw stream info.
    primo::ref<RawT> info_;

    /// Takes ownership of an existing raw stream info reference.
    explicit TStreamInfo(primo::ref<RawT> info)
        : info_(std::move(info)) {}

    /// Wraps a non-owned pointer, retaining it. Used by subclass raw-pointer constructors.
    explicit TStreamInfo(RawT* info)
        : info_(info) { if (info) info->retain(); }

    TStreamInfo(const TStreamInfo&) = delete;
    TStreamInfo& operator=(const TStreamInfo&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TStreamInfo(TStreamInfo&&) = default;
    TStreamInfo& operator=(TStreamInfo&&) = default;
    /// @}

private:
    Derived& self() { return static_cast<Derived&>(*this); }
    const Derived& self() const { return static_cast<const Derived&>(*this); }

public:
    // -- Fluent setters --

    /// @{
    /**
     * Sets the stream type.
     * @param type Major stream type.
     * @return This object, for chaining.
     */
    Derived& streamType(primo::codecs::StreamType::Enum type) & {
        info_->setStreamType(type);
        return self();
    }

    Derived&& streamType(primo::codecs::StreamType::Enum type) && {
        info_->setStreamType(type);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the stream subtype.
     * @param subType Stream subtype.
     * @return This object, for chaining.
     */
    Derived& streamSubType(primo::codecs::StreamSubType::Enum subType) & {
        info_->setStreamSubType(subType);
        return self();
    }

    Derived&& streamSubType(primo::codecs::StreamSubType::Enum subType) && {
        info_->setStreamSubType(subType);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the stream duration.
     * @param d Duration in seconds.
     * @return This object, for chaining.
     */
    Derived& duration(double d) & {
        info_->setDuration(d);
        return self();
    }

    Derived&& duration(double d) && {
        info_->setDuration(d);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the stream bitrate.
     * @param bps Bitrate in bits per second.
     * @return This object, for chaining.
     */
    Derived& bitrate(int32_t bps) & {
        info_->setBitrate(bps);
        return self();
    }

    Derived&& bitrate(int32_t bps) && {
        info_->setBitrate(bps);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the stream bitrate mode.
     * @param mode A @c primo::codecs::BitrateMode::Enum constant.
     * @return This object, for chaining.
     */
    Derived& bitrateMode(int32_t mode) & {
        info_->setBitrateMode(mode);
        return self();
    }

    Derived&& bitrateMode(int32_t mode) && {
        info_->setBitrateMode(mode);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the ID of the elementary stream.
     * @param id Elementary stream ID.
     * @return This object, for chaining.
     */
    Derived& ID(int32_t id) & {
        info_->setID(id);
        return self();
    }

    Derived&& ID(int32_t id) && {
        info_->setID(id);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets the program to which the elementary stream belongs.
     * @param num Program number.
     * @return This object, for chaining.
     */
    Derived& programNumber(int32_t num) & {
        info_->setProgramNumber(num);
        return self();
    }

    Derived&& programNumber(int32_t num) && {
        info_->setProgramNumber(num);
        return std::move(self());
    }
    /// @}

    /// @{
    /**
     * Sets decoder specific configuration data. It is typically obtained from a demuxer and is required to decode certain stream types.
     * @param buf Buffer with the configuration data. Can be @c nullptr.
     * @return This object, for chaining.
     */
    Derived& configData(primo::codecs::MediaBuffer* buf) & {
        info_->setConfigData(buf);
        return self();
    }

    Derived&& configData(primo::codecs::MediaBuffer* buf) && {
        info_->setConfigData(buf);
        return std::move(self());
    }
    /// @}

    // -- Getters --

    /// Returns the media type of the stream. It cannot be changed.
    primo::codecs::MediaType::Enum     mediaType()     const { return info_->mediaType(); }
    /// Returns the stream type.
    primo::codecs::StreamType::Enum    streamType()    const { return info_->streamType(); }
    /// Returns the stream subtype.
    primo::codecs::StreamSubType::Enum streamSubType() const { return info_->streamSubType(); }
    /// Returns the stream duration in seconds.
    double                             duration()      const { return info_->duration(); }
    /// Returns the stream bitrate in bits per second.
    int32_t                            bitrate()       const { return info_->bitrate(); }
    /**
     * Returns the stream bitrate mode, a @c primo::codecs::BitrateMode::Enum constant.
     */
    int32_t                            bitrateMode()   const { return info_->bitrateMode(); }
    /// Returns the ID of the elementary stream.
    int32_t                            ID()            const { return info_->ID(); }
    /// Returns the number of the program to which the elementary stream belongs.
    int32_t                            programNumber() const { return info_->programNumber(); }
    /**
     * Returns @c true if the object is immutable. An immutable object cannot be modified.
     */
    bool                               immutable()     const { return info_->immutable() == TRUE; }
    /**
     * Returns the decoder specific configuration data, or @c nullptr if not set.
     */
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

/**
 * Fluent wrapper for @c primo::codecs::AudioStreamInfo, which describes an elementary audio stream.
 *
 * The properties common to all streams are inherited from @c TStreamInfo. The setters can be chained;
 * on a temporary they return the object by rvalue reference so that the chain can be returned or moved:
 * @code
 * TAudioStreamInfo audio = TAudioStreamInfo().channels(2).sampleRate(44100).bitsPerSample(16);
 * @endcode
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TAudioStreamInfo
    : public TStreamInfo<TAudioStreamInfo, primo::codecs::AudioStreamInfo> {
    using Base = TStreamInfo<TAudioStreamInfo, primo::codecs::AudioStreamInfo>;

public:
    /// Creates an audio stream info with default values.
    TAudioStreamInfo()
        : Base(primo::ref<primo::codecs::AudioStreamInfo>(
              primo::avblocks::Library::createAudioStreamInfo())) {}

    /**
     * Takes ownership of an existing @c AudioStreamInfo reference.
     */
    explicit TAudioStreamInfo(primo::ref<primo::codecs::AudioStreamInfo> info)
        : Base(std::move(info)) {}

    /// Wraps a non-owned @c AudioStreamInfo pointer, retaining it.
    explicit TAudioStreamInfo(primo::codecs::AudioStreamInfo* info)
        : Base(info) {}

    TAudioStreamInfo(const TAudioStreamInfo&) = delete;
    TAudioStreamInfo& operator=(const TAudioStreamInfo&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TAudioStreamInfo(TAudioStreamInfo&&) = default;
    TAudioStreamInfo& operator=(TAudioStreamInfo&&) = default;
    /// @}

    // -- Fluent setters --

    /// @{
    /**
     * Sets the number of audio channels.
     * @param channels Number of channels.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& channels(int32_t channels) & {
        info_->setChannels(channels);
        return *this;
    }

    TAudioStreamInfo&& channels(int32_t channels) && {
        info_->setChannels(channels);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the audio sample rate.
     * @param sampleRate Sample rate in Hz.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& sampleRate(int32_t sampleRate) & {
        info_->setSampleRate(sampleRate);
        return *this;
    }

    TAudioStreamInfo&& sampleRate(int32_t sampleRate) && {
        info_->setSampleRate(sampleRate);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the number of bits per sample (sample resolution), usually 8, 16, 24 or 32.
     * @param bits Bits per sample.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& bitsPerSample(int32_t bits) & {
        info_->setBitsPerSample(bits);
        return *this;
    }

    TAudioStreamInfo&& bitsPerSample(int32_t bits) && {
        info_->setBitsPerSample(bits);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the audio format flags.
     * @param flags A combination of @c primo::codecs::PcmFlags::Enum values.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& pcmFlags(int32_t flags) & {
        info_->setPcmFlags(flags);
        return *this;
    }

    TAudioStreamInfo&& pcmFlags(int32_t flags) && {
        info_->setPcmFlags(flags);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the channel layout, the mapping between channels and speakers.
     * @param layout A combination of @c primo::codecs::AudioChannelFlags::Enum values.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& channelLayout(int32_t layout) & {
        info_->setChannelLayout(layout);
        return *this;
    }

    TAudioStreamInfo&& channelLayout(int32_t layout) && {
        info_->setChannelLayout(layout);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the number of bytes occupied by an audio frame, the smallest discrete unit of an audio stream. It is normally valid (greater than 0) for LPCM streams and 0 for compressed streams.
     * @param bytes Bytes per frame.
     * @return This object, for chaining.
     */
    TAudioStreamInfo& bytesPerFrame(int32_t bytes) & {
        info_->setBytesPerFrame(bytes);
        return *this;
    }

    TAudioStreamInfo&& bytesPerFrame(int32_t bytes) && {
        info_->setBytesPerFrame(bytes);
        return std::move(*this);
    }
    /// @}

    // -- Getters --

    /// Returns the number of audio channels.
    int32_t channels()      const { return info_->channels(); }
    /// Returns the audio sample rate in Hz.
    int32_t sampleRate()    const { return info_->sampleRate(); }
    /// Returns the number of bits per sample. Normally 0 for a compressed audio stream.
    int32_t bitsPerSample() const { return info_->bitsPerSample(); }
    /**
     * Returns the audio format flags, a combination of @c primo::codecs::PcmFlags::Enum values.
     */
    int32_t pcmFlags()      const { return info_->pcmFlags(); }
    /**
     * Returns the channel layout, a combination of @c primo::codecs::AudioChannelFlags::Enum values.
     */
    int32_t channelLayout() const { return info_->channelLayout(); }
    /// Returns the number of bytes occupied by an audio frame. Normally 0 for a compressed audio stream.
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

/**
 * Fluent wrapper for @c primo::codecs::VideoStreamInfo, which describes an elementary video stream.
 *
 * The properties common to all streams are inherited from @c TStreamInfo. The setters can be chained;
 * on a temporary they return the object by rvalue reference so that the chain can be returned or moved:
 * @code
 * TVideoStreamInfo video = TVideoStreamInfo().frameWidth(352).frameHeight(288).frameRate(25.0);
 * @endcode
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TVideoStreamInfo
    : public TStreamInfo<TVideoStreamInfo, primo::codecs::VideoStreamInfo> {
    using Base = TStreamInfo<TVideoStreamInfo, primo::codecs::VideoStreamInfo>;

public:
    /// Creates a video stream info with default values.
    TVideoStreamInfo()
        : Base(primo::ref<primo::codecs::VideoStreamInfo>(
              primo::avblocks::Library::createVideoStreamInfo())) {}

    /**
     * Takes ownership of an existing @c VideoStreamInfo reference.
     */
    explicit TVideoStreamInfo(primo::ref<primo::codecs::VideoStreamInfo> info)
        : Base(std::move(info)) {}

    /// Wraps a non-owned @c VideoStreamInfo pointer, retaining it. Used by @c TMediaPin::videoStreamInfo().
    explicit TVideoStreamInfo(primo::codecs::VideoStreamInfo* info)
        : Base(info) {}

    TVideoStreamInfo(const TVideoStreamInfo&) = delete;
    TVideoStreamInfo& operator=(const TVideoStreamInfo&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TVideoStreamInfo(TVideoStreamInfo&&) = default;
    TVideoStreamInfo& operator=(TVideoStreamInfo&&) = default;
    /// @}

    // -- Fluent setters --

    /// @{
    /**
     * Sets the video color format.
     * @param format A @c primo::codecs::ColorFormat::Enum constant.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& colorFormat(primo::codecs::ColorFormat::Enum format) & {
        info_->setColorFormat(format);
        return *this;
    }

    TVideoStreamInfo&& colorFormat(primo::codecs::ColorFormat::Enum format) && {
        info_->setColorFormat(format);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the video frame width.
     * @param width Width in pixels.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& frameWidth(int32_t width) & {
        info_->setFrameWidth(width);
        return *this;
    }

    TVideoStreamInfo&& frameWidth(int32_t width) && {
        info_->setFrameWidth(width);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the video frame height.
     * @param height Height in pixels.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& frameHeight(int32_t height) & {
        info_->setFrameHeight(height);
        return *this;
    }

    TVideoStreamInfo&& frameHeight(int32_t height) && {
        info_->setFrameHeight(height);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the horizontal dimension of the display aspect ratio. The value has no meaning on its own; together with @c displayRatioHeight() it expresses the ratio.
     * @param width Horizontal ratio.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& displayRatioWidth(int32_t width) & {
        info_->setDisplayRatioWidth(width);
        return *this;
    }

    TVideoStreamInfo&& displayRatioWidth(int32_t width) && {
        info_->setDisplayRatioWidth(width);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the vertical dimension of the display aspect ratio. The value has no meaning on its own; together with @c displayRatioWidth() it expresses the ratio.
     * @param height Vertical ratio.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& displayRatioHeight(int32_t height) & {
        info_->setDisplayRatioHeight(height);
        return *this;
    }

    TVideoStreamInfo&& displayRatioHeight(int32_t height) && {
        info_->setDisplayRatioHeight(height);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the video frame rate.
     * @param fps Frames per second.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& frameRate(double fps) & {
        info_->setFrameRate(fps);
        return *this;
    }

    TVideoStreamInfo&& frameRate(double fps) && {
        info_->setFrameRate(fps);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the scan type of the video frame.
     * @param scan A @c primo::codecs::ScanType::Enum constant.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& scanType(primo::codecs::ScanType::Enum scan) & {
        info_->setScanType(scan);
        return *this;
    }

    TVideoStreamInfo&& scanType(primo::codecs::ScanType::Enum scan) && {
        info_->setScanType(scan);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets the image stride (pitch): the distance in bytes from the start of one row of pixels in memory to the start of the next row.
     * @param stride Stride in bytes.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& stride(int32_t stride) & {
        info_->setStride(stride);
        return *this;
    }

    TVideoStreamInfo&& stride(int32_t stride) && {
        info_->setStride(stride);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets whether the frame is stored upside down in memory, as is typical for bitmaps.
     * @param bottomUp @c true if the frame is stored upside down, @c false if the top row is first in memory.
     * @return This object, for chaining.
     */
    TVideoStreamInfo& frameBottomUp(bool bottomUp) & {
        info_->setFrameBottomUp(bottomUp ? TRUE : FALSE);
        return *this;
    }

    TVideoStreamInfo&& frameBottomUp(bool bottomUp) && {
        info_->setFrameBottomUp(bottomUp ? TRUE : FALSE);
        return std::move(*this);
    }
    /// @}

    // -- Getters --

    /// Returns the video color format.
    primo::codecs::ColorFormat::Enum colorFormat()      const { return info_->colorFormat(); }
    /// Returns the video frame width in pixels.
    int32_t                          frameWidth()       const { return info_->frameWidth(); }
    /// Returns the video frame height in pixels.
    int32_t                          frameHeight()      const { return info_->frameHeight(); }
    /// Returns the horizontal dimension of the display aspect ratio.
    int32_t                          displayRatioWidth() const { return info_->displayRatioWidth(); }
    /// Returns the vertical dimension of the display aspect ratio.
    int32_t                          displayRatioHeight() const { return info_->displayRatioHeight(); }
    /// Returns the video frame rate in frames per second.
    double                           frameRate()        const { return info_->frameRate(); }
    /// Returns the scan type of the video frame.
    primo::codecs::ScanType::Enum    scanType()         const { return info_->scanType(); }
    /// Returns the image stride (pitch) in bytes.
    int32_t                          stride()           const { return info_->stride(); }
    /**
     * Returns @c true if the frame is stored upside down in memory.
     */
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
    /// Creates a data stream info with default values.
    TDataStreamInfo()
        : Base(primo::ref<primo::codecs::StreamInfo>(
              primo::avblocks::Library::createDataStreamInfo())) {}

    /**
     * Takes ownership of an existing @c StreamInfo reference.
     */
    explicit TDataStreamInfo(primo::ref<primo::codecs::StreamInfo> info)
        : Base(std::move(info)) {}

    /// Wraps a non-owned @c StreamInfo pointer, retaining it. Used by @c TMediaPin::streamInfo().
    explicit TDataStreamInfo(primo::codecs::StreamInfo* info)
        : Base(info) {}

    TDataStreamInfo(const TDataStreamInfo&) = delete;
    TDataStreamInfo& operator=(const TDataStreamInfo&) = delete;
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TDataStreamInfo(TDataStreamInfo&&) = default;
    TDataStreamInfo& operator=(TDataStreamInfo&&) = default;
    /// @}

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

/**
 * CRTP mixin that adds the typed, fluent @c addParam() to a class that owns a @c primo::avblocks::ParameterList
 * (@c TMediaPin and @c TMediaSocketT). @p Derived must provide a private @c paramList() and declare this mixin a friend.
 *
 * Adds a codec parameter (fluent). @p name is a parameter name constant, e.g. @c Param::Video::Crop::Left or
 * @c Param::Encoder::Audio::AAC::StereoMode. The overload is chosen by the type of @p value:
 * - integer or enum (e.g. @c InterpolationMethod::Super)      -> integer parameter
 * - @c float / @c double                                       -> floating point parameter
 * - @c std::string / @c std::wstring / string literal          -> string parameter
 * - @c TMediaBuffer                                            -> media buffer parameter
 * - @c TVideoStreamInfo                                        -> video stream info parameter
 *
 * Each overload has an lvalue version (returns @c Derived&) and an rvalue version (returns @c Derived&&) so it can be
 * used in the middle of a chain on a temporary.
 */
template<typename Derived>
class TParamsMixin {
    primo::avblocks::ParameterList* params_() { return static_cast<Derived&>(*this).paramList(); }
    Derived& self_() { return static_cast<Derived&>(*this); }

public:
    /// @{
    /**
     * Adds an integer parameter. Selected for integral and enum values, e.g. @c InterpolationMethod::Super.
     * @param name  Parameter name constant, e.g. @c Param::Video::Crop::Left.
     * @param value Parameter value.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    template<typename T> requires (std::integral<T> || std::is_enum_v<T>)
    Derived& addParam(const char* name, T value) & {
        params_()->addInt(name, static_cast<int64_t>(value));
        return self_();
    }

    template<typename T> requires (std::integral<T> || std::is_enum_v<T>)
    Derived&& addParam(const char* name, T value) && {
        params_()->addInt(name, static_cast<int64_t>(value));
        return std::move(self_());
    }
    /// @}

    /// @{
    /**
     * Adds a floating point parameter. Selected for @c float and @c double values.
     * @param name  Parameter name constant, e.g. @c Param::Video::Crop::Left.
     * @param value Parameter value.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    template<std::floating_point T>
    Derived& addParam(const char* name, T value) & {
        params_()->addFloat(name, static_cast<double>(value));
        return self_();
    }

    template<std::floating_point T>
    Derived&& addParam(const char* name, T value) && {
        params_()->addFloat(name, static_cast<double>(value));
        return std::move(self_());
    }
    /// @}

    /// @{
    /**
     * Adds a string parameter. Accepts @c std::string, @c std::wstring and string literals.
     * @param name  Parameter name constant, e.g. @c Param::Video::Crop::Left.
     * @param value Parameter value.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    Derived& addParam(const char* name, const std::string& value) & {
        params_()->addString(name, primo::ustring(value));
        return self_();
    }

    Derived&& addParam(const char* name, const std::string& value) && {
        params_()->addString(name, primo::ustring(value));
        return std::move(self_());
    }

    Derived& addParam(const char* name, const std::wstring& value) & {
        params_()->addString(name, primo::ustring(value));
        return self_();
    }

    Derived&& addParam(const char* name, const std::wstring& value) && {
        params_()->addString(name, primo::ustring(value));
        return std::move(self_());
    }
    /// @}

    /// @{
    /**
     * Adds a media buffer parameter.
     * @param name  Parameter name constant, e.g. @c Param::Video::Crop::Left.
     * @param value The buffer to store in the parameter.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    Derived& addParam(const char* name, const TMediaBuffer& value) & {
        params_()->addMediaBuffer(name, value.get());
        return self_();
    }

    Derived&& addParam(const char* name, const TMediaBuffer& value) && {
        params_()->addMediaBuffer(name, value.get());
        return std::move(self_());
    }
    /// @}

    /// @{
    /**
     * Adds a video stream info parameter.
     * @param name  Parameter name constant, e.g. @c Param::Video::Crop::Left.
     * @param value The video stream info to store in the parameter.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    Derived& addParam(const char* name, const TVideoStreamInfo& value) & {
        params_()->addVideoStreamInfo(name, value.get());
        return self_();
    }

    Derived&& addParam(const char* name, const TVideoStreamInfo& value) && {
        params_()->addVideoStreamInfo(name, value.get());
        return std::move(self_());
    }
    /// @}
};

/**
 * Fluent wrapper for @c primo::avblocks::MediaPin, which represents an elementary media stream.
 *
 * A pin can exist for audio, video or any other kind of stream, but AVBlocks processes only audio and video
 * streams. Pins are the inputs and outputs of a @c TMediaSocketT. A transcoder matches input and output pins by
 * their connection ID (see @c connection()). Codec parameters are added with @c addParam().
 *
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
class TMediaPin : public TParamsMixin<TMediaPin> {
    friend class TParamsMixin<TMediaPin>;

    primo::ref<primo::avblocks::MediaPin> pin_;
    
public:
    /// Creates an empty media pin.
    TMediaPin() 
        : pin_(primo::avblocks::Library::createMediaPin()) {}
    
    /**
     * Takes ownership of an existing @c MediaPin reference.
     */
    explicit TMediaPin(primo::ref<primo::avblocks::MediaPin> pin)
        : pin_(std::move(pin)) {}

    /// Wraps a non-owned @c MediaPin pointer, retaining it. Used by @c TMediaSocketT::pin().
    explicit TMediaPin(primo::avblocks::MediaPin* pin)
        : pin_(pin) { if (pin) pin->retain(); }
    
    // Delete copy operations
    TMediaPin(const TMediaPin&) = delete;
    TMediaPin& operator=(const TMediaPin&) = delete;
    
    // Enable move operations
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMediaPin(TMediaPin&&) = default;
    TMediaPin& operator=(TMediaPin&&) = default;
    /// @}
    
    /// @{
    /**
     * Sets the stream info of the pin. The pin retains the stream info object, so the pin and @p info share it.
     * @param info Audio or video stream info.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
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
    /// @}
    
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

    /// @{
    /**
     * Sets the connection ID of the pin. A transcoder matches and connects input and output pins by their
     * connection ID. Do not confuse it with the stream ID defined for certain stream types.
     * @param connectionID @c PinConnection::Disabled to exclude the pin, @c PinConnection::Auto for automatic
     *                      matching, or any positive integer to match with an output pin that has the same ID.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference.
     */
    TMediaPin&& connection(int32_t connectionID) && {
        pin_->setConnection(connectionID);
        return std::move(*this);
    }

    TMediaPin& connection(int32_t connectionID) & {
        pin_->setConnection(connectionID);
        return *this;
    }
    /// @}

    /// Returns the pin's current connection ID.
    int32_t connection() const {
        return pin_->connection();
    }

    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::avblocks::MediaPin* get() const { return pin_.get(); }

private:
    primo::avblocks::ParameterList* paramList() { return pin_->params(); }
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
        /// The list this proxy refers to.
        TMediaPinList& list_;
        /// Index of the item the proxy refers to.
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
/**
 * Maps the character type of the fluent API (@c char or @c wchar_t) to the matching string type and converts
 * to and from @c primo::ustring. Used by @c TMediaSocketT, @c TMediaInfoT and @c TTranscoderT.
 */
template<typename CharT>
struct string_traits;

/**
 * Narrow strings: @c std::string holding UTF-8.
 */
template<>
struct string_traits<char> {
    /**
     * The string type: @c std::string.
     */
    using string_type = std::string;
    /**
     * Converts a string to a @c primo::ustring.
     */
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
    /**
     * Converts a @c primo::ustring to a string.
     */
    static string_type from_ustring(const primo::ustring& u) {
        return u.str();
    }
};

/**
 * Wide strings: @c std::wstring.
 */
template<>
struct string_traits<wchar_t> {
    /**
     * The string type: @c std::wstring.
     */
    using string_type = std::wstring;
    /**
     * Converts a string to a @c primo::ustring.
     */
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
    /**
     * Converts a @c primo::ustring to a string.
     */
    static string_type from_ustring(const primo::ustring& u) {
        return u.wstr();
    }
};

/**
 * Fluent wrapper for @c primo::avblocks::MediaSocket, an input or output point of a transcoder.
 *
 * A socket describes either a container (WAV, AVI, MP4, etc.) with one or more streams, or a single elementary
 * stream (MP3, M2V, etc.). Its pins (see @c pins()) describe the contained streams. Socket parameters, such as
 * muxer options, are added with @c addParam(). The setters can be chained and return the object by rvalue
 * reference on a temporary, so a whole socket can be built in one expression:
 * @code
 * TMediaSocket("out.mp4").addPin(TMediaPin().streamInfo(video));
 * @endcode
 * @tparam CharT @c char for UTF-8 @c std::string paths, @c wchar_t for @c std::wstring paths.
 * Use the aliases @c TMediaSocket and @c TMediaSocketW. Owns the raw object through a @c primo::ref.
 * Not copyable, movable.
 */
template<typename CharT = char>
class TMediaSocketT : public TParamsMixin<TMediaSocketT<CharT>> {
    friend class TParamsMixin<TMediaSocketT<CharT>>;
    primo::ref<primo::avblocks::MediaSocket> socket_;
    
public:
    /**
     * The string type for paths: @c std::string for @c char, @c std::wstring for @c wchar_t.
     */
    using string_type = typename string_traits<CharT>::string_type;
    
    /// Creates an empty media socket.
    TMediaSocketT() 
        : socket_(primo::avblocks::Library::createMediaSocket()) {}
    
    /**
     * Takes ownership of an existing @c MediaSocket reference.
     */
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
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMediaSocketT(TMediaSocketT&&) = default;
    TMediaSocketT& operator=(TMediaSocketT&&) = default;
    /// @}
    
    /// @{
    /**
     * Sets the file associated with the socket.
     * @param path File name.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& file(const string_type& path) && {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return std::move(*this);
    }
    
    TMediaSocketT& file(const string_type& path) & {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return *this;
    }
    /// @}

    /// @{
    /**
     * Clears the file associated with the socket.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& file(std::nullptr_t) && {
        socket_->setFile(nullptr);
        return std::move(*this);
    }

    TMediaSocketT& file(std::nullptr_t) & {
        socket_->setFile(nullptr);
        return *this;
    }
    /// @}

    /// Returns the file path as a @c string_type (UTF-8 @c std::string for @c char, @c std::wstring for @c wchar_t), or empty string if not set.
    string_type file() const {
        const char_t* f = socket_ ? socket_->file() : nullptr;
        return f ? string_traits<CharT>::from_ustring(primo::ustring(f)) : string_type{};
    }

    /// @{
    /**
     * Sets explicitly the format of the associated file or stream, e.g. AVI, WAV or MP4. If the socket has a file and
     * no type, the type is deduced from the file name extension.
     * @param type Stream type.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& streamType(primo::codecs::StreamType::Enum type) && {
        socket_->setStreamType(type);
        return std::move(*this);
    }
    
    TMediaSocketT& streamType(primo::codecs::StreamType::Enum type) & {
        socket_->setStreamType(type);
        return *this;
    }
    /// @}

    /// @{
    /**
     * Sets the user stream, an alternative to a file, that is the source or sink of the socket. The socket retains
     * the stream. When both a file and a stream are set, the transcoder uses the stream.
     * @param s The stream. Pass @c nullptr to clear a previously set stream.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& stream(primo::Stream* s) && {
        socket_->setStream(s);
        return std::move(*this);
    }

    TMediaSocketT& stream(primo::Stream* s) & {
        socket_->setStream(s);
        return *this;
    }
    /// @}

    /// @{
    /**
     * Sets explicitly the stream subtype of the associated file or stream.
     * @param subType Stream subtype.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& streamSubType(primo::codecs::StreamSubType::Enum subType) && {
        socket_->setStreamSubType(subType);
        return std::move(*this);
    }
    
    TMediaSocketT& streamSubType(primo::codecs::StreamSubType::Enum subType) & {
        socket_->setStreamSubType(subType);
        return *this;
    }
    /// @}
    
    /// @{
    /**
     * Adds a pin (elementary stream) to the socket. The pin list retains the pin.
     * @param pin The pin to add.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& addPin(TMediaPin&& pin) && {
        socket_->pins()->add(pin.get());
        return std::move(*this);
    }
    
    TMediaSocketT& addPin(TMediaPin&& pin) & {
        socket_->pins()->add(pin.get());
        return *this;
    }
    /// @}

    /// @{
    /**
     * Sets the time position of the socket. On an input socket the transcoder seeks to this position before the
     * transcoding starts. It cannot be changed while the transcoder is open. Has no effect if the socket is immutable.
     * @param time Time position in seconds.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaSocketT&& timePosition(double time) && {
        socket_->setTimePosition(time);
        return std::move(*this);
    }

    TMediaSocketT& timePosition(double time) & {
        socket_->setTimePosition(time);
        return *this;
    }
    /// @}

    /// @{
    /// Gives you a reference to the pin at @p index, so you can do something with it (read it or change it)
    /// without leaving a fluent chain.
    ///
    /// @p fn is called once, immediately, with a @c TMediaPin& that refers to the pin inside this socket (it is not a
    /// copy), so changes made through it apply to the socket. The socket itself is returned, so chaining continues.
    ///
    /// Use this instead of @c pins(index): that returns the pin and ends the chain, so the socket is no longer
    /// available to pass on, e.g. to @c TTranscoder::addOutput().
    /// @code
    /// TTranscoder()
    ///     .addInput(TMediaSocket(mediaInfo))
    ///     .addOutput(
    ///         TMediaSocket(mediaInfo)
    ///             .file("out.mp4")
    ///             .pinRef(0, [](TMediaPin& pin) {
    ///                 pin.videoStreamInfo().frameRate(30.0);
    ///             }))
    ///     .open().run().close();
    /// @endcode
    /// @param index Zero-based index of the pin.
    /// @param fn    Callable taking @c TMediaPin&.
    template<typename Fn>
    TMediaSocketT&& pinRef(int32_t index, Fn&& fn) && {
        TMediaPin pin = pins(index);
        std::forward<Fn>(fn)(pin);
        return std::move(*this);
    }

    template<typename Fn>
    TMediaSocketT& pinRef(int32_t index, Fn&& fn) & {
        TMediaPin pin = pins(index);
        std::forward<Fn>(fn)(pin);
        return *this;
    }
    /// @}

    /// @{
    /// Gives you a reference to the video stream info of the pin at @p index, so you can do something with it
    /// (read it or change it) without leaving a fluent chain. Shorter than @c pinRef() when only the stream info
    /// is needed.
    ///
    /// @p fn is called once, immediately, with a @c TVideoStreamInfo& that refers to the stream info inside the pin
    /// (it is not a copy), so changes made through it apply to the socket. The socket is returned, so chaining
    /// continues. The pin must be a video pin.
    /// @code
    /// TMediaSocket(mediaInfo).file("out.mp4")
    ///     .videoInfoRef(0, [](TVideoStreamInfo& video) { video.frameRate(30.0); })
    /// @endcode
    template<typename Fn>
    TMediaSocketT&& videoInfoRef(int32_t index, Fn&& fn) && {
        TVideoStreamInfo info = pins(index).videoStreamInfo();
        std::forward<Fn>(fn)(info);
        return std::move(*this);
    }

    template<typename Fn>
    TMediaSocketT& videoInfoRef(int32_t index, Fn&& fn) & {
        TVideoStreamInfo info = pins(index).videoStreamInfo();
        std::forward<Fn>(fn)(info);
        return *this;
    }
    /// @}

    /// @{
    /// Gives you a reference to the audio stream info of the pin at @p index, so you can do something with it
    /// (read it or change it) without leaving a fluent chain. Same as @c videoInfoRef() for an audio pin.
    /// @code
    /// TMediaSocket(mediaInfo).file("out.mp3")
    ///     .audioInfoRef(0, [](TAudioStreamInfo& audio) { audio.sampleRate(48000); })
    /// @endcode
    template<typename Fn>
    TMediaSocketT&& audioInfoRef(int32_t index, Fn&& fn) && {
        TAudioStreamInfo info = pins(index).audioStreamInfo();
        std::forward<Fn>(fn)(info);
        return std::move(*this);
    }

    template<typename Fn>
    TMediaSocketT& audioInfoRef(int32_t index, Fn&& fn) & {
        TAudioStreamInfo info = pins(index).audioStreamInfo();
        std::forward<Fn>(fn)(info);
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

    /// Creates an independent deep copy of this socket (file, stream types and pins). Typical use is to start the
    /// output from the input: @c input.clone().file("out.mp4").
    TMediaSocketT clone() const {
        return TMediaSocketT(primo::ref<primo::avblocks::MediaSocket>(socket_->clone()));
    }

    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::avblocks::MediaSocket* get() const { return socket_.get(); }

private:
    primo::avblocks::ParameterList* paramList() { return socket_->params(); }
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
        /// The list this proxy refers to.
        TMediaSocketList& list_;
        /// Index of the item the proxy refers to.
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

/**
 * Fluent wrapper for @c primo::avblocks::MediaInfo, which analyzes a file or a stream and reports its type and
 * properties.
 *
 * Configure the input socket (see @c file() and @c inputs()), call @c open(), then read the detected streams from
 * @c outputs() and the metadata from the sockets. In push mode, feed data with @c push() until @c isReady().
 * @code
 * auto info = TMediaInfo().file("in.mp4").open();
 * int32_t pins = info.outputs(0).pins().count();
 * @endcode
 * @tparam CharT @c char for UTF-8 @c std::string paths, @c wchar_t for @c std::wstring paths.
 * Use the aliases @c TMediaInfo and @c TMediaInfoW. Owns the raw object through a @c primo::ref.
 * Not copyable, movable.
 */
template<typename CharT = char>
class TMediaInfoT {
    primo::ref<primo::avblocks::MediaInfo> info_;

public:
    /// Creates a media info object with one default input socket.
    TMediaInfoT()
        : info_(primo::avblocks::Library::createMediaInfo()) {}

    // Delete copy operations
    TMediaInfoT(const TMediaInfoT&) = delete;
    TMediaInfoT& operator=(const TMediaInfoT&) = delete;

    // Enable move operations
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TMediaInfoT(TMediaInfoT&&) = default;
    TMediaInfoT& operator=(TMediaInfoT&&) = default;
    /// @}

    /**
     * The string type for paths: @c std::string for @c char, @c std::wstring for @c wchar_t.
     */
    using string_type = typename string_traits<CharT>::string_type;

    /// @{
    /**
     * Sets the file of the first input socket, so a probe can be written as @c TMediaInfo().file("in.mp4").open().
     * @param path File name.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TMediaInfoT&& file(const string_type& path) && {
        inputs(0).file(path);
        return std::move(*this);
    }

    TMediaInfoT& file(const string_type& path) & {
        inputs(0).file(path);
        return *this;
    }
    /// @}

    /// @{
    /**
     * Analyzes the input. If it succeeds, the detected streams and metadata can be read from the object.
     * On a temporary, @c auto @c info @c = @c TMediaInfo().file(f).open(); moves the opened object into @c info.
     * @throws TAVBlocksException if the input cannot be analyzed.
     * @return This object, for chaining.
     */
    TMediaInfoT& open() & {
        if (!info_->open()) {
            throw TAVBlocksException("Failed to open MediaInfo", TErrorInfo(info_->error()));
        }
        return *this;
    }

    TMediaInfoT&& open() && {
        open();
        return std::move(*this);
    }
    /// @}

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

    /// Returns the error information for the last operation.
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

    /// Returns the raw underlying pointer. For interop with the core API only.
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
    /// Sets the function called periodically to report progress, with the current and total time in seconds.
    void setOnProgress(std::function<void(double, double)> f)   { onProgress_    = std::move(f); }
    /// Sets the function called to report the status of the current operation.
    void setOnStatus(std::function<void(primo::avblocks::TranscoderStatus::Enum)> f) { onStatus_ = std::move(f); }
    /**
     * Sets the function called to let the user code stop a running transcoder. It returns @c true to continue.
     */
    void setOnContinue(std::function<bool(double)> f)            { onContinue_    = std::move(f); }
    /// Sets the function called when an input format is detected or changes, with the index of the input socket.
    void setOnInputChange(std::function<void(int32_t)> f)        { onInputChange_ = std::move(f); }

    /// Calls the progress function, if set.
    void onProgress(double cur, double total) override {
        if (onProgress_) onProgress_(cur, total);
    }
    /// Calls the status function, if set.
    void onStatus(primo::avblocks::TranscoderStatus::Enum s) override {
        if (onStatus_) onStatus_(s);
    }
    /**
     * Calls the continue function, if set. Returns @c TRUE when no function is set, so the transcoder keeps running.
     */
    bool_t onContinue(double cur) override {
        return onContinue_ ? (onContinue_(cur) ? TRUE : FALSE) : TRUE;
    }
    /// Calls the input change function, if set.
    void onInputChange(int32_t idx) override {
        if (onInputChange_) onInputChange_(idx);
    }
};

/**
 * Fluent wrapper for @c primo::avblocks::Transcoder, which encodes, decodes and transforms audio and video.
 *
 * Add input and output sockets, then either run the whole job with @c transcode() (file to file), or drive it
 * step by step: @c open(), @c run() or @c push() / @c pull() / @c flush(), then @c close(). Failures are reported with
 * @c TAVBlocksException; the @c try* variants return @c false instead. Progress and status are received through
 * lambdas (see @c onProgress()).
 * @code
 * TTranscoder()
 *     .allowDemoMode(true)
 *     .addInput(TMediaSocket(mediaInfo))
 *     .addOutput(TMediaSocket("out.mp4"))
 *     .transcode();
 * @endcode
 * @tparam CharT @c char or @c wchar_t, matching the sockets used. Use the aliases @c TTranscoder and @c TTranscoderW.
 * Owns the raw object through a @c primo::ref. Not copyable, movable.
 */
template<typename CharT = char>
class TTranscoderT {
    // callback_ must be declared BEFORE transcoder_ so that it is destroyed
    // AFTER the raw Transcoder (which holds a raw pointer to the callback).
    std::unique_ptr<TTranscoderCallback>   callback_;
    primo::ref<primo::avblocks::Transcoder> transcoder_;
    
public:
    /// Creates a transcoder with no inputs and outputs.
    TTranscoderT() 
        : transcoder_(primo::avblocks::Library::createTranscoder()) {}
    
    // Delete copy operations
    TTranscoderT(const TTranscoderT&) = delete;
    TTranscoderT& operator=(const TTranscoderT&) = delete;
    
    // Enable move operations
    /// @{
    /// Move construction/assignment transfers ownership of the underlying object. The moved-from object no longer wraps anything.
    TTranscoderT(TTranscoderT&&) = default;
    TTranscoderT& operator=(TTranscoderT&&) = default;
    /// @}
    
        /// Returns the callback adapter, creating it and installing it in the transcoder on first use.
    TTranscoderCallback& ensureCallback() {
        if (!callback_) {
            callback_ = std::make_unique<TTranscoderCallback>();
            transcoder_->setCallback(callback_.get());
        }
        return *callback_;
    }

    /// @{
    /**
     * Sets whether demo mode (unlicensed transcoding) is allowed. When allowed, a demo overlay is applied if a
     * required codec is not licensed; otherwise the transcoder fails with @c AVBlocksError::UnlicensedFeature.
     * Demo mode is on by default in the Demo release and off in the OEM release.
     * @param allow @c true to allow demo mode.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& allowDemoMode(bool allow = true) & {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }

    TTranscoderT&& allowDemoMode(bool allow = true) && {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets a function called periodically during @c run() to report progress.
     * @param f Function taking the current and total time in seconds.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& onProgress(std::function<void(double, double)> f) & {
        ensureCallback().setOnProgress(std::move(f));
        return *this;
    }

    TTranscoderT&& onProgress(std::function<void(double, double)> f) && {
        ensureCallback().setOnProgress(std::move(f));
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets a function called during @c run() to report the status of the current operation.
     * @param f Function taking the @c TranscoderStatus.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& onStatus(std::function<void(primo::avblocks::TranscoderStatus::Enum)> f) & {
        ensureCallback().setOnStatus(std::move(f));
        return *this;
    }

    TTranscoderT&& onStatus(std::function<void(primo::avblocks::TranscoderStatus::Enum)> f) && {
        ensureCallback().setOnStatus(std::move(f));
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets a function that controls whether @c run() continues. It is called with the current time in seconds.
     * Return @c true to continue or @c false to stop gracefully, leaving valid output.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& onContinue(std::function<bool(double)> f) & {
        ensureCallback().setOnContinue(std::move(f));
        return *this;
    }

    TTranscoderT&& onContinue(std::function<bool(double)> f) && {
        ensureCallback().setOnContinue(std::move(f));
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Sets a function called when an input format is detected or changes in push mode, with the index of the input
     * socket. It can inspect the detected inputs and reconfigure outputs.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& onInputChange(std::function<void(int32_t)> f) & {
        ensureCallback().setOnInputChange(std::move(f));
        return *this;
    }

    TTranscoderT&& onInputChange(std::function<void(int32_t)> f) && {
        ensureCallback().setOnInputChange(std::move(f));
        return std::move(*this);
    }
    /// @}
    
    /// @{
    /**
     * Adds an input socket. The transcoder retains the socket.
     * @param socket The input socket.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& addInput(const TMediaSocketT<CharT>& socket) & {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }

    TTranscoderT&& addInput(const TMediaSocketT<CharT>& socket) && {
        transcoder_->inputs()->add(socket.get());
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Removes the input socket at @p index.
     * @param index Zero-based index of the socket.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& removeInput(int32_t index) & {
        transcoder_->inputs()->remove(index);
        return *this;
    }

    TTranscoderT&& removeInput(int32_t index) && {
        transcoder_->inputs()->remove(index);
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Adds an output socket. The transcoder retains the socket.
     * @param socket The output socket.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& addOutput(const TMediaSocketT<CharT>& socket) & {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }

    TTranscoderT&& addOutput(const TMediaSocketT<CharT>& socket) && {
        transcoder_->outputs()->add(socket.get());
        return std::move(*this);
    }
    /// @}

    /// @{
    /**
     * Removes the output socket at @p index.
     * @param index Zero-based index of the socket.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& removeOutput(int32_t index) & {
        transcoder_->outputs()->remove(index);
        return *this;
    }

    TTranscoderT&& removeOutput(int32_t index) && {
        transcoder_->outputs()->remove(index);
        return std::move(*this);
    }
    /// @}

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

    /// @{
    /**
     * Initializes the transcoder from the configured inputs and outputs. When it succeeds the transcoder is ready
     * to process data with @c run(), or with @c push() and @c pull().
     * @throws TAVBlocksException if the transcoder cannot be opened.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& open() & {
        if (!transcoder_->open()) {
            throw TAVBlocksException("Failed to open transcoder", TErrorInfo(transcoder_->error()));
        }
        return *this;
    }

    TTranscoderT&& open() && {
        if (!transcoder_->open()) {
            throw TAVBlocksException("Failed to open transcoder", TErrorInfo(transcoder_->error()));
        }
        return std::move(*this);
    }
    /// @}

    /// Opens the transcoder without throwing. Returns @c true on success.
    bool tryOpen() {
        return transcoder_->open() == TRUE;
    }
    
    /**
     * Pulls output data from the transcoder.
     * @param[out] outputIndex Receives the index of the output socket the data belongs to.
     * @param[out] sample      Receives the output data.
     * @return @c true on success.
     */
    bool pull(int32_t& outputIndex, TMediaSample& sample) {
        return transcoder_->pull(outputIndex, sample.get()) == TRUE;
    }

    /**
     * Pushes input data to the transcoder. The sample is used only within this call and is not retained. The
     * transcoder may consume only part of it: on return the buffer's data offset and size show what is left, which
     * matters when more data is appended for the next push.
     * @param inputIndex Index of the input socket the data is for.
     * @param sample     The input data.
     * @return @c true on success.
     */
    bool push(int32_t inputIndex, TMediaSample& sample) {
        return transcoder_->push(inputIndex, sample.get()) == TRUE;
    }

    /// Signals end-of-stream on the input socket at @p inputIndex.
    /// Equivalent to pushing a null (empty) sample.
    bool pushEos(int32_t inputIndex) {
        return transcoder_->push(inputIndex, nullptr) == TRUE;
    }

    /// @{
    /**
     * Runs an automatic transcoding. It returns when the transcoding is finished. All inputs and outputs must be
     * associated with files or user streams.
     * @throws TAVBlocksException if the transcoding fails or run mode cannot be used with these inputs and outputs.
     * @return This object, for chaining. On a temporary the rvalue overload returns the object by rvalue reference, so the chain can be returned or moved.
     */
    TTranscoderT& run() & {
        if (!transcoder_->run()) {
            throw TAVBlocksException("Failed to run transcoder", TErrorInfo(transcoder_->error()));
        }
        return *this;
    }

    TTranscoderT&& run() && {
        if (!transcoder_->run()) {
            throw TAVBlocksException("Failed to run transcoder", TErrorInfo(transcoder_->error()));
        }
        return std::move(*this);
    }
    /// @}
    
    /// Closes the transcoder. When closed it neither accepts nor delivers data. The inputs and outputs are not modified.
    void close() {
        transcoder_->close();
    }

    /// Opens, runs and closes the transcoder (file to file mode): the whole @c open().run().close() sequence in
    /// one call. The transcoder is closed even if @c run() throws; @c TAVBlocksException is thrown if it cannot be
    /// opened or fails to run.
    /// @code
    /// TTranscoder().allowDemoMode(true).addInput(in).addOutput(out).transcode();
    /// @endcode
    void transcode() {
        open();
        try {
            run();
        } catch (...) {
            close();
            throw;
        }
        close();
    }

    /**
     * Flushes the data buffered in the transcoder to the output and marks the end of the stream for all inputs. Push
     * mode only. Do not push data after flushing.
     * @return @c true on success.
     */
    bool flush() {
        return transcoder_->flush() == TRUE;
    }

    /// Returns the error information for the last operation.
    TErrorInfo error() const {
        return TErrorInfo(transcoder_->error());
    }
    
    /// Returns the raw underlying pointer. For interop with the core API only.
    primo::avblocks::Transcoder* get() const { return transcoder_.get(); }
};

// Type aliases for convenience
using TTranscoder = TTranscoderT<char>;
using TTranscoderW = TTranscoderT<wchar_t>;

} // namespace primo::avblocks::modern