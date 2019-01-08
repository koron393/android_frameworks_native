/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file hardware_buffer.h
 * @brief API for native hardware buffers.
 */
/**
 * @defgroup AHardwareBuffer Native Hardware Buffer
 *
 * AHardwareBuffer objects represent chunks of memory that can be
 * accessed by various hardware components in the system. It can be
 * easily converted to the Java counterpart
 * android.hardware.HardwareBuffer and passed between processes using
 * Binder. All operations involving AHardwareBuffer and HardwareBuffer
 * are zero-copy, i.e., passing AHardwareBuffer to another process
 * creates a shared view of the same region of memory.
 *
 * AHardwareBuffers can be bound to EGL/OpenGL and Vulkan primitives.
 * For EGL, use the extension function eglGetNativeClientBufferANDROID
 * to obtain an EGLClientBuffer and pass it directly to
 * eglCreateImageKHR. Refer to the EGL extensions
 * EGL_ANDROID_get_native_client_buffer and
 * EGL_ANDROID_image_native_buffer for more information. In Vulkan,
 * the contents of the AHardwareBuffer can be accessed as external
 * memory. See the VK_ANDROID_external_memory_android_hardware_buffer
 * extension for details.
 *
 * @{
 */

#ifndef ANDROID_HARDWARE_BUFFER_H
#define ANDROID_HARDWARE_BUFFER_H

#include <inttypes.h>

#include <sys/cdefs.h>

#include <android/rect.h>

__BEGIN_DECLS

/**
 * Buffer pixel formats.
 */
enum AHardwareBuffer_Format {
    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGBA8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM           = 1,

    /**
     * 32 bits per pixel, 8 bits per channel format where alpha values are
     * ignored (always opaque).
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM           = 2,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM             = 3,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R5G6B5_UNORM_PACK16
     *   OpenGL ES: GL_RGB565
     */
    AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM             = 4,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R16G16B16A16_SFLOAT
     *   OpenGL ES: GL_RGBA16F
     */
    AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT       = 0x16,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_A2B10G10R10_UNORM_PACK32
     *   OpenGL ES: GL_RGB10_A2
     */
    AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM        = 0x2b,

    /**
     * An opaque binary blob format that must have height 1, with width equal to
     * the buffer size in bytes.
     */
    AHARDWAREBUFFER_FORMAT_BLOB                     = 0x21,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D16_UNORM
     *   OpenGL ES: GL_DEPTH_COMPONENT16
     */
    AHARDWAREBUFFER_FORMAT_D16_UNORM                = 0x30,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_X8_D24_UNORM_PACK32
     *   OpenGL ES: GL_DEPTH_COMPONENT24
     */
    AHARDWAREBUFFER_FORMAT_D24_UNORM                = 0x31,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D24_UNORM_S8_UINT
     *   OpenGL ES: GL_DEPTH24_STENCIL8
     */
    AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT        = 0x32,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D32_SFLOAT
     *   OpenGL ES: GL_DEPTH_COMPONENT32F
     */
    AHARDWAREBUFFER_FORMAT_D32_FLOAT                = 0x33,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D32_SFLOAT_S8_UINT
     *   OpenGL ES: GL_DEPTH32F_STENCIL8
     */
    AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT        = 0x34,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_S8_UINT
     *   OpenGL ES: GL_STENCIL_INDEX8
     */
    AHARDWAREBUFFER_FORMAT_S8_UINT                  = 0x35,
};

/**
 * Buffer usage flags, specifying how the buffer will be accessed.
 */
enum AHardwareBuffer_UsageFlags {
    /// The buffer will never be locked for direct CPU reads using the
    /// AHardwareBuffer_lock() function. Note that reading the buffer
    /// using OpenGL or Vulkan functions or memory mappings is still
    /// allowed.
    AHARDWAREBUFFER_USAGE_CPU_READ_NEVER        = 0UL,
    /// The buffer will sometimes be locked for direct CPU reads using
    /// the AHardwareBuffer_lock() function. Note that reading the
    /// buffer using OpenGL or Vulkan functions or memory mappings
    /// does not require the presence of this flag.
    AHARDWAREBUFFER_USAGE_CPU_READ_RARELY       = 2UL,
    /// The buffer will often be locked for direct CPU reads using
    /// the AHardwareBuffer_lock() function. Note that reading the
    /// buffer using OpenGL or Vulkan functions or memory mappings
    /// does not require the presence of this flag.
    AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN        = 3UL,
    /// CPU read value mask.
    AHARDWAREBUFFER_USAGE_CPU_READ_MASK         = 0xFUL,

    /// The buffer will never be locked for direct CPU writes using the
    /// AHardwareBuffer_lock() function. Note that writing the buffer
    /// using OpenGL or Vulkan functions or memory mappings is still
    /// allowed.
    AHARDWAREBUFFER_USAGE_CPU_WRITE_NEVER       = 0UL << 4,
    /// The buffer will sometimes be locked for direct CPU writes using
    /// the AHardwareBuffer_lock() function. Note that writing the
    /// buffer using OpenGL or Vulkan functions or memory mappings
    /// does not require the presence of this flag.
    AHARDWAREBUFFER_USAGE_CPU_WRITE_RARELY      = 2UL << 4,
    /// The buffer will often be locked for direct CPU writes using
    /// the AHardwareBuffer_lock() function. Note that writing the
    /// buffer using OpenGL or Vulkan functions or memory mappings
    /// does not require the presence of this flag.
    AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN       = 3UL << 4,
    /// CPU write value mask.
    AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK        = 0xFUL << 4,

    /// The buffer will be read from by the GPU as a texture.
    AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE      = 1UL << 8,
    /// The buffer will be written to by the GPU as a framebuffer attachment.
    AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER        = 1UL << 9,
    /**
     * The buffer will be written to by the GPU as a framebuffer attachment.
     * Note that the name of this flag is somewhat misleading: it does not imply
     * that the buffer contains a color format. A buffer with depth or stencil
     * format that will be used as a framebuffer attachment should also have
     * this flag.
     */
    AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT       = AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER,
    /**
     * The buffer is protected from direct CPU access or being read by non-secure
     * hardware, such as video encoders. This flag is incompatible with CPU
     * read and write flags. It is mainly used when handling DRM video.
     * Refer to the EGL extension EGL_EXT_protected_content and GL extension
     * EXT_protected_textures for more information on how these buffers are expected
     * to behave.
     */
    AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT      = 1UL << 14,
    /// The buffer will be read by a hardware video encoder.
    AHARDWAREBUFFER_USAGE_VIDEO_ENCODE           = 1UL << 16,
    /**
     * The buffer will be used for direct writes from sensors.
     * When this flag is present, the format must be AHARDWAREBUFFER_FORMAT_BLOB.
     */
    AHARDWAREBUFFER_USAGE_SENSOR_DIRECT_DATA     = 1UL << 23,
    /**
     * The buffer will be used as a shader storage or uniform buffer object.
     * When this flag is present, the format must be AHARDWAREBUFFER_FORMAT_BLOB.
     */
    AHARDWAREBUFFER_USAGE_GPU_DATA_BUFFER        = 1UL << 24,
    /**
     * The buffer will be used as a cube map texture.
     * When this flag is present, the buffer must have a layer count that is
     * a multiple of 6.
     */
    AHARDWAREBUFFER_USAGE_GPU_CUBE_MAP               = 1UL << 25,
    /// The buffer contains a complete mipmap hierarchy.
    AHARDWAREBUFFER_USAGE_GPU_MIPMAP_COMPLETE        = 1UL << 26,

    AHARDWAREBUFFER_USAGE_VENDOR_0  = 1ULL << 28,
    AHARDWAREBUFFER_USAGE_VENDOR_1  = 1ULL << 29,
    AHARDWAREBUFFER_USAGE_VENDOR_2  = 1ULL << 30,
    AHARDWAREBUFFER_USAGE_VENDOR_3  = 1ULL << 31,
    AHARDWAREBUFFER_USAGE_VENDOR_4  = 1ULL << 48,
    AHARDWAREBUFFER_USAGE_VENDOR_5  = 1ULL << 49,
    AHARDWAREBUFFER_USAGE_VENDOR_6  = 1ULL << 50,
    AHARDWAREBUFFER_USAGE_VENDOR_7  = 1ULL << 51,
    AHARDWAREBUFFER_USAGE_VENDOR_8  = 1ULL << 52,
    AHARDWAREBUFFER_USAGE_VENDOR_9  = 1ULL << 53,
    AHARDWAREBUFFER_USAGE_VENDOR_10 = 1ULL << 54,
    AHARDWAREBUFFER_USAGE_VENDOR_11 = 1ULL << 55,
    AHARDWAREBUFFER_USAGE_VENDOR_12 = 1ULL << 56,
    AHARDWAREBUFFER_USAGE_VENDOR_13 = 1ULL << 57,
    AHARDWAREBUFFER_USAGE_VENDOR_14 = 1ULL << 58,
    AHARDWAREBUFFER_USAGE_VENDOR_15 = 1ULL << 59,
    AHARDWAREBUFFER_USAGE_VENDOR_16 = 1ULL << 60,
    AHARDWAREBUFFER_USAGE_VENDOR_17 = 1ULL << 61,
    AHARDWAREBUFFER_USAGE_VENDOR_18 = 1ULL << 62,
    AHARDWAREBUFFER_USAGE_VENDOR_19 = 1ULL << 63,
};

/**
 * Buffer description. Used for allocating new buffers and querying parameters
 * of existing ones.
 */
typedef struct AHardwareBuffer_Desc {
    uint32_t    width;      ///< Width in pixels.
    uint32_t    height;     ///< Height in pixels.
    uint32_t    layers;     ///< Number of images in an image array.
    uint32_t    format;     ///< One of AHardwareBuffer_Format.
    uint64_t    usage;      ///< Combination of AHardwareBuffer_UsageFlags.
    uint32_t    stride;     ///< Row stride in pixels, ignored for AHardwareBuffer_allocate()
    uint32_t    rfu0;       ///< Initialize to zero, reserved for future use.
    uint64_t    rfu1;       ///< Initialize to zero, reserved for future use.
} AHardwareBuffer_Desc;

/**
 * Opaque handle for a native hardware buffer.
 */
typedef struct AHardwareBuffer AHardwareBuffer;

#if __ANDROID_API__ >= 26

/**
 * Allocates a buffer that matches the passed AHardwareBuffer_Desc.
 *
 * If allocation succeeds, the buffer can be used according to the
 * usage flags specified in its description. If a buffer is used in ways
 * not compatible with its usage flags, the results are undefined and
 * may include program termination.
 *
 * \return 0 on success, or an error number of the allocation fails for
 * any reason. The returned buffer has a reference count of 1.
 */
int AHardwareBuffer_allocate(const AHardwareBuffer_Desc* desc,
        AHardwareBuffer** outBuffer) __INTRODUCED_IN(26);
/**
 * Acquire a reference on the given AHardwareBuffer object.  This prevents the
 * object from being deleted until the last reference is removed.
 */
void AHardwareBuffer_acquire(AHardwareBuffer* buffer) __INTRODUCED_IN(26);

/**
 * Remove a reference that was previously acquired with
 * AHardwareBuffer_acquire() or AHardwareBuffer_allocate().
 */
void AHardwareBuffer_release(AHardwareBuffer* buffer) __INTRODUCED_IN(26);

/**
 * Return a description of the AHardwareBuffer in the passed
 * AHardwareBuffer_Desc struct.
 */
void AHardwareBuffer_describe(const AHardwareBuffer* buffer,
        AHardwareBuffer_Desc* outDesc) __INTRODUCED_IN(26);

/**
 * Lock the AHardwareBuffer for reading or writing, depending on the usage flags
 * passed.  This call may block if the hardware needs to finish rendering or if
 * CPU caches need to be synchronized, or possibly for other implementation-
 * specific reasons.  If fence is not negative, then it specifies a fence file
 * descriptor that will be signaled when the buffer is locked, otherwise the
 * caller will block until the buffer is available.
 *
 * If \a rect is not NULL, the caller promises to modify only data in the area
 * specified by rect. If rect is NULL, the caller may modify the contents of the
 * entire buffer.
 *
 * The content of the buffer outside of the specified rect is NOT modified
 * by this call.
 *
 * The \a usage parameter may only specify AHARDWAREBUFFER_USAGE_CPU_*. If set,
 * then outVirtualAddress is filled with the address of the buffer in virtual
 * memory.
 *
 * THREADING CONSIDERATIONS:
 *
 * It is legal for several different threads to lock a buffer for read access;
 * none of the threads are blocked.
 *
 * Locking a buffer simultaneously for write or read/write is undefined, but
 * will neither terminate the process nor block the caller; AHardwareBuffer_lock
 * may return an error or leave the buffer's content into an indeterminate
 * state.
 *
 * \return 0 on success, -EINVAL if \a buffer is NULL or if the usage
 * flags are not a combination of AHARDWAREBUFFER_USAGE_CPU_*, or an error
 * number of the lock fails for any reason.
 */
int AHardwareBuffer_lock(AHardwareBuffer* buffer, uint64_t usage,
        int32_t fence, const ARect* rect, void** outVirtualAddress) __INTRODUCED_IN(26);

/**
 * Unlock the AHardwareBuffer; must be called after all changes to the buffer
 * are completed by the caller. If fence is not NULL then it will be set to a
 * file descriptor that is signaled when all pending work on the buffer is
 * completed. The caller is responsible for closing the fence when it is no
 * longer needed.
 *
 * \return 0 on success, -EINVAL if \a buffer is NULL, or an error
 * number if the unlock fails for any reason.
 */
int AHardwareBuffer_unlock(AHardwareBuffer* buffer, int32_t* fence) __INTRODUCED_IN(26);

/**
 * Send the AHardwareBuffer to an AF_UNIX socket.
 *
 * \return 0 on success, -EINVAL if \a buffer is NULL, or an error
 * number if the operation fails for any reason.
 */
int AHardwareBuffer_sendHandleToUnixSocket(const AHardwareBuffer* buffer, int socketFd) __INTRODUCED_IN(26);

/**
 * Receive the AHardwareBuffer from an AF_UNIX socket.
 *
 * \return 0 on success, -EINVAL if \a outBuffer is NULL, or an error
 * number if the operation fails for any reason.
 */
int AHardwareBuffer_recvHandleFromUnixSocket(int socketFd, AHardwareBuffer** outBuffer) __INTRODUCED_IN(26);

#endif // __ANDROID_API__ >= 26

__END_DECLS

#endif // ANDROID_HARDWARE_BUFFER_H

/** @} */
