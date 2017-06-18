/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class mao_archive_libzip_ZipFile */

#ifndef _Included_mao_archive_libzip_ZipFile
#define _Included_mao_archive_libzip_ZipFile
#ifdef __cplusplus
extern "C" {
#endif
#undef mao_archive_libzip_ZipFile_ZIP_CREATE
#define mao_archive_libzip_ZipFile_ZIP_CREATE 1L
#undef mao_archive_libzip_ZipFile_ZIP_EXCL
#define mao_archive_libzip_ZipFile_ZIP_EXCL 2L
#undef mao_archive_libzip_ZipFile_ZIP_CHECKCONS
#define mao_archive_libzip_ZipFile_ZIP_CHECKCONS 4L
#undef mao_archive_libzip_ZipFile_ZIP_TRUNCATE
#define mao_archive_libzip_ZipFile_ZIP_TRUNCATE 8L
#undef mao_archive_libzip_ZipFile_ZIP_RDONLY
#define mao_archive_libzip_ZipFile_ZIP_RDONLY 16L
/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_initIDs
  (JNIEnv *, jclass);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    open
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_open
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    setPassword
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_setPassword
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    getEntriesCount
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntriesCount
  (JNIEnv *, jclass, jlong);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    getEntry0
 * Signature: (JJ)Lmao/archive/libzip/ZipEntry;
 */
JNIEXPORT jobject JNICALL Java_mao_archive_libzip_ZipFile_getEntry0
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    getEntry1
 * Signature: (J[B)Lmao/archive/libzip/ZipEntry;
 */
JNIEXPORT jobject JNICALL Java_mao_archive_libzip_ZipFile_getEntry1
  (JNIEnv *, jclass, jlong, jbyteArray);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    removeEntry
 * Signature: (JJ)Z
 */
JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_removeEntry
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    renameEntry
 * Signature: (JJLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_renameEntry
  (JNIEnv *, jclass, jlong, jlong, jstring);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    addFileEntry
 * Signature: (JLjava/lang/String;Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_addFileEntry
  (JNIEnv *, jclass, jlong, jstring, jstring, jint, jint);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    addBufferEntry
 * Signature: (JLjava/lang/String;[B)J
 */
JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_addBufferEntry
  (JNIEnv *, jclass, jlong, jstring, jbyteArray);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    addDirectoryEntry
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_addDirectoryEntry
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    openEntry
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_openEntry
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    readEntryBytes
 * Signature: (J[BII)J
 */
JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_readEntryBytes
  (JNIEnv *, jclass, jlong, jbyteArray, jint, jint);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    readEntryBuffer
 * Signature: (JLjava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_mao_archive_libzip_ZipFile_readEntryBuffer
  (JNIEnv *, jclass, jlong, jobject, jint);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    closeEntry
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_closeEntry
  (JNIEnv *, jclass, jlong);

/*
 * Class:     mao_archive_libzip_ZipFile
 * Method:    close
 * Signature: (JLmao/archive/libzip/ProgressListener;)V
 */
JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_close
  (JNIEnv *, jclass, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
