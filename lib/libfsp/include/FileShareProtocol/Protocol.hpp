/*
** Project LibFileShareProtocol, 2022
**
** Author Francois Michaut
**
** Started on  Thu Aug 25 22:59:37 2022 Francois Michaut
** Last update Thu Aug 25 23:16:20 2022 Francois Michaut
**
** Protocol.hpp : Main class to interract with the protocol
*/

namespace FileShareProtocol {
    enum class CommandCode {
        REQ_SEND_FILE       = 0x10,
        REQ_RECIVE_FILE     = 0x11,
        SEND_FILE           = 0x20,
        RECIVE_FILE         = 0x21,
        LIST_FILES          = 0x30,
        FILE_LIST           = 0x31,
        DATA_PACKET         = 0x42,
        PAIR_REQUEST        = 0x50,
        ACCEPT_PAIR_REQUEST = 0x51,
    };

    enum class StatusCode {
        STATUS_OK       = 0x00,
        UNKNOWN_COMMAND = 0x44,
        INTERNAL_ERROR  = 0x50,
    };

    class Protocol {
        public:
            Protocol();

        private:
    };
}
