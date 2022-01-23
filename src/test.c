#include "console.h"

void cmd_callback(const char *cmd)
{
    console_debug(cmd);
}

void print_chat_chatter(void)
{
    console_chat("player 1> Hello, I'm looking for new players to join my guild! We're really active and participate in events. If you're interested, please DM me.");
    console_chat("player 2> Hey, I'd really love to join your guild. how many players are online rn?");
    console_chat("player 1> Dude I won't tell you, just join us please");
    console_chat("player 2> wtf I won't join until you tell me");
    console_chat("player 1> what an attitude... we have more than 10 players on rn");
    console_chat("player 2> so... 11?");
    console_chat("player 1> dude wanna join or nah?");
    console_chat("player 2> I'm good, thanks :)");
    console_chat("player 1> **** off");
}

int main()
{
    console_init();
    print_chat_chatter();
    console_main(cmd_callback);
    console_free();
    return 0;
}
