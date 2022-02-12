#include "packets.h"

#include <stdarg.h>

#include "utils/dynstring.h"
#include "utils/buffer.h"

#include "debug.h"

int send_packet(stream_t *stream, int packet_id, const char *data_str, ...)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = buffer_write_varint(buff, packet_id);
    if(err) {
        return err;
    }

    va_list args;
    va_start(args, data_str);

    while(*data_str) {
        switch(*data_str++) {
        case 'v':;
            int32_t hmm = va_arg(args, int32_t);
            err = buffer_write_varint(buff, hmm);
            break;
        case 'V':
            err = buffer_write_varlong(buff, va_arg(args, int64_t));
            break;
        // case 'i':
        //     err = buffer_write_int(buff, va_arg(args, int));
        //     break;
        case 'h':
            err = buffer_write_short(buff, va_arg(args, int));
            break;
        case 'b':
            err = buffer_write_byte(buff, va_arg(args, int));
            break;
        case 's':
            err = buffer_write_c_string(buff, va_arg(args, const char *));
            break;
        case 'S':
            err = buffer_write_string(buff, va_arg(args, string_t *));
            break;
        }
        if(err) {
            return err;
        }
    }
    va_end(args);

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}

extern int send_Handshake(stream_t *stream, int32_t proto_version, const char *ip, uint16_t port, conn_state_t next_state);
extern int send_StatusRequest(stream_t *stream);
extern int send_LoginStart(stream_t *stream, const char *username);
extern int send_EncryptionResponse(stream_t *stream, string_t *enc_aes_key, string_t *enc_verify_token);
extern int send_KeepAlive(stream_t *stream, int32_t keep_alive_id);
extern int send_ChatMessage(stream_t *stream, const char *message);
extern int send_ClientStatus(stream_t *stream, enum action_id action_id);
