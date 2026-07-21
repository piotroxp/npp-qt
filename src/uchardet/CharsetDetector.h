// CharsetDetector.h — uchardet-compatible charset detection with Qt fallback
// Copyright (C) 2026 Agent Army | GPL v3
//
// Semantic lift: Win32/NP++ uchardet integration → Qt6 QStringConverter fallback.
// This header provides a uchardet-compatible interface while falling back to
// Qt's built-in encoding detection when uchardet is not available.
//
// Win32 → Qt translation:
//   uchardet_* functions        → CharsetDetector class with Qt strings
//   uchardet_data_t (byte)      → const char* + size_t
//   uchardet_reset()            → reset()
//   uchardet_handle_data()       → feed()
//   uchardet_get_charset()       → getCharsetName()
//   No equivalent in Win32      → setConfidenceThreshold(), getConfidence()
//
// uchardet is a Mozilla-derived encoding detector. If it's found at configure
// time, we use the real library. Otherwise we use Qt's heuristic detection.

#pragma once

#include <QString>
#include <QStringConverter>
#include <QVector>
#include <QByteArray>
#include <cstddef>

// Detect whether real uchardet is available
// Set USE_UCHARDET=ON in CMake to enable; otherwise Qt fallback is used.
#ifdef USE_UCHARDET
#include <uchardet.h>
#define HAVE_UCHARDET 1
#else
#define HAVE_UCHARDET 0
#endif

/**
 * @class CharsetDetector
 * @brief Character set detector with uchardet backend and Qt fallback.
 *
 * Provides a uchardet-compatible interface for detecting the character encoding
 * of byte streams. When built without uchardet, falls back to Qt's
 * QStringConverter heuristics (BOM detection + byte sequence analysis).
 *
 * @note Win32: In the original N++, uchardet was used via uchardet.h and
 *   uchardet-data.h. This class wraps the same API in a Qt-friendly way.
 *
 * Example:
 * @code
 *   CharsetDetector detector;
 *   QFile file("document.txt");
 *   file.open(QIODevice::ReadOnly);
 *   QByteArray chunk;
 *   while (!(chunk = file.read(4096)).isEmpty()) {
 *       detector.feed(chunk.constData(), chunk.size());
 *   }
 *   QString charset = detector.getCharsetName(); // e.g., "UTF-8", "ISO-8859-1"
 * @endcode
 */
class CharsetDetector {
public:
    /**
     * @brief Construct a new CharsetDetector.
     *
     * Initializes the internal state. On systems with uchardet, creates
     * a uchardet detector handle; otherwise uses Qt's QStringDecoder.
     */
    CharsetDetector();

    /**
     * @brief Destructor.
     *
     * Frees the uchardet handle if uchardet is in use.
     */
    ~CharsetDetector();

    // Non-copyable, non-movable
    Q_DISABLE_COPY_MOVE(CharsetDetector)

    /**
     * @brief Feed a chunk of byte data to the detector.
     *
     * Call this repeatedly with successive chunks of the file/data being analyzed.
     * The detector accumulates state across calls.
     *
     * @param data Pointer to byte data (may contain embedded NUL bytes)
     * @param len  Number of bytes in data
     *
     * @note Win32 → Qt: uchardet_handle_data(det, data, len)
     */
    void feed(const char* data, size_t len);

    /**
     * @brief Get the detected charset name.
     *
     * Returns the name of the detected character encoding. Call after
     * feeding sufficient data (at least a few hundred bytes is recommended
     * for reliable detection).
     *
     * @return Charset name string (e.g., "UTF-8", "windows-1252", "ISO-8859-1")
     *         Returns empty string if no charset has been determined yet
     *         or if detection failed.
     *
     * @note Win32 → Qt: uchardet_get_charset(det) → QString
     * @note For UTF-16 variants, Qt returns "UTF-16". For Big Endian detection,
     *       the BOM (FE FF) is checked first; if no BOM, heuristic analysis
     *       is used (byte pair frequency analysis).
     */
    const char* getCharsetName() const;

    /**
     * @brief Reset the detector state.
     *
     * Clears all accumulated state, allowing the detector to be reused
     * for a new document.
     *
     * @note Win32 → Qt: uchardet_reset(det)
     */
    void reset();

    /**
     * @brief Check whether uchardet library is in use (vs. Qt fallback).
     * @return true if real uchardet is active, false if Qt fallback is active.
     */
    bool usingUchardet() const { return m_hasUchardet; }

    /**
     * @brief Get a confidence score for the detected charset.
     *
     * Returns a value from 0.0 to 1.0 indicating confidence in the detection.
     * Only meaningful when using real uchardet; Qt fallback always returns 1.0
     * (BOM-detected) or 0.5 (heuristic).
     *
     * @return Confidence value between 0.0 (no confidence) and 1.0 (certain).
     */
    double getConfidence() const;

    /**
     * @brief Set the minimum confidence threshold for auto-detection.
     *
     * When using Qt fallback, if the confidence is below this threshold,
     * the detector returns "ISO-8859-1" (binary-safe default) instead
     * of a potentially incorrect encoding.
     *
     * @param threshold Value between 0.0 and 1.0 (default: 0.0)
     */
    void setConfidenceThreshold(double threshold);

    /**
     * @brief Get the detected charset as a Qt encoding enum.
     *
     * Converts the detected charset name to a QStringConverter::Encoding
     * for use with QTextCodec/QStringConverter.
     *
     * @return QStringConverter::Encoding value, or QStringConverter::Utf8 as fallback.
     */
    QStringConverter::Encoding getEncoding() const;

    /**
     * @brief Get the last error message, if any.
     * @return Error description string, or empty if no error occurred.
     */
    QString errorString() const { return m_errorString; }

private:
    /**
     * @brief Detect charset using Qt's built-in heuristics.
     *
     * Win32 → Qt translation:
     *   In Win32, this would be handled by uchardet. Here we use:
     *   1. BOM detection (UTF-8, UTF-16 LE/BE, UTF-32 LE/BE)
     *   2. Byte frequency analysis for single-byte charsets
     *   3. Qt's QStringConverter::Encoding for multi-byte detection
     *
     * @param data Pointer to byte data
     * @param len  Number of bytes
     */
    void detectWithQt(const char* data, size_t len);

    /**
     * @brief Analyze byte frequency to guess single-byte charset.
     *
     * Scans for byte patterns characteristic of specific charsets:
     * - High frequency of 0x80-0x9F suggests Windows-1252
     * - 0x80 appearing before other high bytes suggests CP437
     * - Balanced distribution suggests ISO-8859-1
     *
     * @param data Pointer to byte data
     * @param len  Number of bytes
     * @return Detected charset name or empty string
     */
    QString analyzeSingleByteCharset(const char* data, size_t len) const;

    // uchardet handle — valid only when HAVE_UCHARDET is set
    void* m_uchardetHandle = nullptr;

    // Accumulated raw data for Qt-based detection
    QByteArray m_accumulatedData;
    size_t m_accumulatedLen = 0;

    // Detected charset (cached after detection)
    mutable QString m_detectedCharset;

    // Whether uchardet is available
    bool m_hasUchardet = false;

    // Confidence threshold for Qt fallback
    double m_confidenceThreshold = 0.0;

    // Current confidence score
    // mutable: assigned from const getCharsetName() (cached detection result).
    mutable double m_confidence = 0.0;

    // Error message
    QString m_errorString;

    // BOM detection state
    // mutable: assigned from const getCharsetName() while caching the result.
    mutable bool m_bomChecked = false;
    mutable QString m_bomCharset;  // charset determined by BOM

    // High-byte statistics for heuristic detection
    int m_highByteCount = 0;    // bytes in 0x80-0xFF range
    int m_win1252Markers = 0;   // bytes in 0x80-0x9F range
    int m_nullCharCount = 0;    // embedded NUL bytes (suggests binary or UTF-16)
    int m_totalAnalyzed = 0;    // total bytes analyzed so far
};
