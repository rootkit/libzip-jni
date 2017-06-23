//
// Created by mao on 16-10-30.
//
#include "lib/zip.h"
#include <malloc.h>
#include "mao_archive_libzip_ZipFile.h"
#include "jni_util.h"
#include "jlong.h"
#include "lib/zipint.h"
#include <android/log.h>

#define  LOG_TAG    "libzip-jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define MAXNAME 1024


JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_initIDs
        (JNIEnv *env, jclass cls) {
}


JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_open(JNIEnv *env, jclass cls, jstring jname,
                                                             jint mode) {
    char errbuf[2048];
    zip_error_t error;
    zip_t *za;
    int err;
    const char *name = (*env)->GetStringUTFChars(env, jname, 0);
    if ((za = zip_open(name, mode, &err)) == NULL) {
        (*env)->ReleaseStringUTFChars(env, jname, name);
        zip_error_init_with_code(&error, err);
        const char *string = zip_error_strerror(&error);
        sprintf(errbuf, "can't open zip archive '%s': %s\n", name, string);
        zip_error_fini(&error);
        JNU_ThrowIOException(env, errbuf);
        return 0;
    }
    (*env)->ReleaseStringUTFChars(env, jname, name);
    return ptr_to_jlong(za);
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntriesCount
        (JNIEnv *env, jclass cls, jlong jzip) {

    zip_t *za = jlong_to_ptr(jzip);
    zip_int64_t num_entries = zip_get_num_entries(za, 0);
    if (num_entries < 0) {
        LOGE("get_entries_count error %s", zip_strerror(za));
        zip_error_clear(za);
    }
    return num_entries;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_removeEntry
        (JNIEnv *env, jclass cls, jlong jzip, jlong index) {
    zip_t *za = jlong_to_ptr(jzip);
    if (zip_delete(za, (zip_uint64_t) index) < 0) {
        LOGE("remove entry error %s", zip_strerror(za));
        zip_error_clear(za);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_renameEntry
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jstring jname) {
    zip_t *za = jlong_to_ptr(jzip);

    const char *name = (*env)->GetStringUTFChars(env, jname, 0);
    if (zip_file_rename(za, (zip_uint64_t) index, name, ZIP_FL_ENC_UTF_8) < 0) {
        LOGE("rename error %s", zip_strerror(za));
        zip_error_clear(za);
        (*env)->ReleaseStringUTFChars(env, jname, name);
        return JNI_FALSE;
    }
    (*env)->ReleaseStringUTFChars(env, jname, name);
    return JNI_TRUE;

}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_addFileEntry
        (JNIEnv *env, jclass cls, jlong jzip, jstring jname, jstring jpath, jlong off,
         jlong len) {
    const char *path = NULL;
    const char *name = NULL;
    zip_source_t *source_t = NULL;

    zip_t *za = jlong_to_ptr(jzip);


    path = (*env)->GetStringUTFChars(env, jpath, 0);
    name = (*env)->GetStringUTFChars(env, jname, 0);
    if (path == NULL || name == NULL || (*env)->ExceptionCheck(env))
        goto err;

    source_t = zip_source_file(za, path, (zip_uint64_t) off, len);
    if (source_t == NULL) {
        goto err;
    }
    zip_int64_t index;
    if ((index = zip_file_add(za, name, source_t, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8)) < 0) {
        goto err;
    }


    done:
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jname, name);
    return index;

    err:
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jname, name);
    JNU_ThrowIOException(env, zip_strerror(za));
    zip_error_clear(za);
    return -1;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_addBufferEntry
        (JNIEnv *env, jclass cls, jlong jzip, jstring jname, jbyteArray jbuf) {
    const char *name = NULL;
    char *buf = NULL;
    zip_source_t *zip_source;
    zip_t *za = jlong_to_ptr(jzip);

    jsize length = (*env)->GetArrayLength(env, jbuf);

    //释放zip_source时会自动释放buf
    buf = malloc((size_t) length);
    if (buf == NULL) {
        JNU_ThrowOutOfMemoryError(env, "can't malloc memory!");
        return -1;
    }
    (*env)->GetByteArrayRegion(env, jbuf, 0, length, (jbyte *) buf);

    name = (*env)->GetStringUTFChars(env, jname, 0);

    //关闭整个zip文件时自动释放
    zip_source = zip_source_buffer(za, buf, (zip_uint64_t) length, 1);
    if (zip_source == NULL) {
        goto err;
    }
    zip_int64_t index;
    if ((index = zip_file_add(za, name, zip_source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8)) < 0) {
        goto err;
    }

    (*env)->ReleaseStringUTFChars(env, jname, name);
    return index;

    err:
    free(buf);
    (*env)->ReleaseStringUTFChars(env, jname, name);
    JNU_ThrowIOException(env, zip_strerror(za));
    zip_error_clear(za);
    return -1;
}


JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_addDirectoryEntry
        (JNIEnv *env, jclass cls, jlong jzip, jstring jname) {
    zip_t *za = jlong_to_ptr(jzip);
    zip_int64_t index;

    const char *name = (*env)->GetStringUTFChars(env, jname, 0);

    if ((index = zip_dir_add(za, name, ZIP_FL_ENC_UTF_8)) < 0) {
        (*env)->ReleaseStringUTFChars(env, jname, name);
        JNU_ThrowIOException(env, zip_strerror(za));
        zip_error_clear(za);
        return -1;
    }
    (*env)->ReleaseStringUTFChars(env, jname, name);
    return index;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_nameLocate
        (JNIEnv *env, jclass cls, jlong jzip, jbyteArray jrawName) {
    zip_t *za = jlong_to_ptr(jzip);
    jsize ulen = (*env)->GetArrayLength(env, jrawName);
    char buf[MAXNAME + 2], *rawName;

    if (ulen > MAXNAME) {
        rawName = malloc((size_t) (ulen + 2));
        if (rawName == NULL) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return -1;
        }
    } else {
        rawName = buf;
    }
    (*env)->GetByteArrayRegion(env, jrawName, 0, ulen, (jbyte *) rawName);
    rawName[ulen] = '\0';
    zip_int64_t index;

    if ((index = zip_name_locate(za, rawName, ZIP_FL_ENC_RAW)) < 0) {
        zip_error_clear(za);
    }
    if (rawName != buf) {
        free(rawName);
    }
    return index;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_openEntryStat
        (JNIEnv *env, jclass cls, jlong jzip, jlong index) {
    zip_t *za = jlong_to_ptr(jzip);
    zip_stat_t *stat = malloc(sizeof(zip_stat_t));
    if (zip_stat_index(za, (zip_uint64_t) index, 0, stat) < 0) {
        zip_error_clear(za);
        return 0;
    }
    return ptr_to_jlong(stat);
}

JNIEXPORT jbyteArray JNICALL Java_mao_archive_libzip_ZipFile_getEntryName
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    size_t len = strlen(stat->name);

    jbyteArray jba = NULL;
    if ((jba = (*env)->NewByteArray(env, len)) == NULL) {
        goto done;
    }
    (*env)->SetByteArrayRegion(env, jba, 0, len, (jbyte *) stat->name);
    done:
    return jba;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntrySize
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->size;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntryCSize
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->comp_size != 0 ? stat->comp_size : stat->size;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntryMTime
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->mtime & 0xffffffffUL;
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_getEntryCrc
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->crc & 0xffffffffUL;
}

JNIEXPORT jint JNICALL Java_mao_archive_libzip_ZipFile_getEntryMethod
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->comp_method;

}

JNIEXPORT jint JNICALL Java_mao_archive_libzip_ZipFile_getEntryEncryptionMethod
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->encryption_method;
}

JNIEXPORT jint JNICALL Java_mao_archive_libzip_ZipFile_getEntryFlags
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    return stat->flags;
}

JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_freeEntryStat
        (JNIEnv *env, jclass cls, jlong jstat) {
    zip_stat_t *stat = jlong_to_ptr(jstat);
    free(stat);
}

JNIEXPORT jbyteArray JNICALL Java_mao_archive_libzip_ZipFile_getEntryComment
        (JNIEnv *env, jclass cls, jlong jzip, jlong index) {
    zip_t *za = jlong_to_ptr(jzip);
    const char *comment = NULL;
    jbyteArray jba = NULL;
    zip_uint32_t len = 0;
    if ((comment = zip_file_get_comment(za, (zip_uint64_t) index, &len, ZIP_FL_ENC_RAW)) == NULL ||
        len == 0) {
        zip_error_clear(za);
        goto done;
    }
    if ((jba = (*env)->NewByteArray(env, len)) == NULL) {
        goto done;
    }
    (*env)->SetByteArrayRegion(env, jba, 0, len, (const jbyte *) comment);
    done:
    return jba;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_setEntryMTime
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jlong time) {
    zip_t *za = jlong_to_ptr(jzip);
    if (zip_file_set_mtime(za, (zip_uint64_t) index, (time_t) time, 0) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_setEntryEncryptionMethod
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jint emethod) {
    zip_t *za = jlong_to_ptr(jzip);
    if (zip_file_set_encryption(za, (zip_uint64_t) index, (zip_uint16_t) emethod, NULL) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_setEntryMethod
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jint method) {
    zip_t *za = jlong_to_ptr(jzip);
    if (zip_set_file_compression(za, (zip_uint64_t) index, method, 0) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_setEntryComment
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jbyteArray comment) {
#define COMMENT_MAX 1024
    zip_t *za = jlong_to_ptr(jzip);
    char buf[COMMENT_MAX + 2], *comm;
    jsize ulen = (*env)->GetArrayLength(env, comment);

    if (ulen > MAXNAME) {
        comm = malloc((size_t) (ulen + 2));
        if (comm == NULL) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return JNI_FALSE;
        }
    } else {
        comm = buf;
    }
    (*env)->GetByteArrayRegion(env, comment, 0, ulen, (jbyte *) comm);
    comm[ulen] = '\0';
    if (zip_set_file_comment(za, (zip_uint64_t) index, comm, ulen) < 0) {
        return JNI_FALSE;
    }
    if (comm != buf) {
        free(comm);
    }
    return JNI_TRUE;
}

JNIEXPORT jbyteArray JNICALL Java_mao_archive_libzip_ZipFile_getComment
        (JNIEnv *env, jclass cls, jlong jzip) {
    zip_t *za = jlong_to_ptr(jzip);
    const char *comment = NULL;
    jbyteArray jba = NULL;
    int len = 0;

    if ((comment = zip_get_archive_comment(za, &len, ZIP_FL_ENC_RAW)) == NULL ||
        len == 0) {
        zip_error_clear(za);
        goto done;
    }
    if ((jba = (*env)->NewByteArray(env, len)) == NULL) {
        goto done;
    }
    (*env)->SetByteArrayRegion(env, jba, 0, len, (const jbyte *) comment);
    done:
    return jba;
}

JNIEXPORT jboolean JNICALL Java_mao_archive_libzip_ZipFile_setComment
        (JNIEnv *env, jclass cls, jlong jzip, jbyteArray comment) {
#define COMMENT_MAX 1024
    zip_t *za = jlong_to_ptr(jzip);
    char buf[COMMENT_MAX + 2], *comm;
    jsize ulen = (*env)->GetArrayLength(env, comment);
    if (ulen > MAXNAME) {
        comm = malloc((size_t) (ulen + 2));
        if (comm == NULL) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return JNI_FALSE;
        }
    } else {
        comm = buf;
    }
    (*env)->GetByteArrayRegion(env, comment, 0, ulen, (jbyte *) comm);
    comm[ulen] = '\0';

    if (zip_set_archive_comment(za, comm, (zip_uint16_t) ulen) < 0) {
        return JNI_FALSE;
    }
    if (comm != buf) {
        free(comm);
    }
    return JNI_TRUE;
}


JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_openEntry
        (JNIEnv *env, jclass cls, jlong jzip, jlong index, jstring password) {
    zip_t *za = jlong_to_ptr(jzip);
#define MAX_PASSWORD 128
    char *passwd;

    if (password == NULL || (*env)->GetStringLength(env, password) == 0) {
        passwd = NULL;
    } else {
        passwd = (char *) (*env)->GetStringUTFChars(env, password, 0);
    }


    zip_file_t *zf = zip_fopen_index_encrypted(za, (zip_uint64_t) index, ZIP_FL_UNCHANGED, passwd);
    if (zf == NULL) {
        if (passwd != NULL) {
            (*env)->ReleaseStringUTFChars(env, password, passwd);
        }
        switch (za->error.zip_err) {
            case ZIP_ER_WRONGPASSWD:
            case ZIP_ER_NOPASSWD: {
                JNU_ThrowByName(env, "mao/archive/libzip/PasswordException", zip_strerror(za));
                break;
            }
            case ZIP_ER_COMPNOTSUPP:
            case ZIP_ER_ENCRNOTSUPP: {
                JNU_ThrowByName(env, "java/util/zip/ZipException", zip_strerror(za));
                break;
            }
            default:
                JNU_ThrowIOException(env, zip_strerror(za));
        }
        zip_error_clear(za);
        return 0;
    }
    if (passwd != NULL) {
        (*env)->ReleaseStringUTFChars(env, password, passwd);
    }
    return ptr_to_jlong(zf);
}

JNIEXPORT jlong JNICALL Java_mao_archive_libzip_ZipFile_readEntryBytes
        (JNIEnv *env, jclass cls, jlong jzf, jbyteArray jbuf, jint off, jint jlen) {
    zip_file_t *zf = jlong_to_ptr(jzf);

    jbyte *buf = (*env)->GetByteArrayElements(env, jbuf, NULL);
    if (buf == NULL || (*env)->ExceptionCheck(env)) {
        (*env)->ReleaseByteArrayElements(env, jbuf, buf, JNI_ABORT);
        JNU_ThrowIOException(env, "can't get byte array buffer!");
        return -1;
    }

    zip_int64_t len = zip_fread(zf, buf + off, (zip_uint64_t) (jlen - off));
    (*env)->ReleaseByteArrayElements(env, jbuf, buf, JNI_ABORT);
    return len;

}


JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_closeEntry
        (JNIEnv *env, jclass cls, jlong jzf) {
    zip_file_t *zf = jlong_to_ptr(jzf);
    if (zip_fclose(zf)) {
        JNU_ThrowIOException(env, zip_error_strerror(&zf->error));
        zip_file_error_clear(zf);
    }
}

static JNIEnv *g_env = NULL;
static jobject g_listener = NULL;
static jmethodID progress_method_id = NULL;

static void progress_callback(double progress) {
//    LOGE("progress %f\n", progress);
    if (g_env != NULL && g_listener != NULL && progress_method_id != NULL) {
        (*g_env)->CallVoidMethod(g_env, g_listener, progress_method_id, (int) (progress * 100));
    }

}

JNIEXPORT void JNICALL Java_mao_archive_libzip_ZipFile_close
        (JNIEnv *env, jclass cls, jlong jzip, jobject listener) {
    zip_t *za = jlong_to_ptr(jzip);
    if (listener != NULL) {
        g_listener = listener;
        if (progress_method_id == NULL) {
            jclass listener_cls = (*env)->GetObjectClass(env, listener);
            progress_method_id = (*env)->GetMethodID(env, listener_cls, "onProgressing", "(I)V");
            (*env)->DeleteLocalRef(env, listener_cls);
            if ((*env)->ExceptionCheck(env)) {
                goto done;
            }
        }
        if (g_env == NULL) {
            g_env = env;
        }
        zip_register_progress_callback(za, progress_callback);
    }
    done:
    if (zip_close(za) < 0) {
        JNU_ThrowIOException(env, zip_strerror(za));
    }
}
//TODO register native mathod


