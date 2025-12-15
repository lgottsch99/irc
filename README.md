# Clients:
    irssi → terminal, clean, stable, widely used at 42 schools
    WeeChat → fancier, better UI, very nice
    HexChat → GUI, friendly
    mIRC → old Windows classic
Irssi is common simply because:
    it’s available everywhere
    it supports RFC standards well
    it’s stable and predictable
    no weird behaviour
Weechat is arguably nicer, but irssi is “the safe default.”

yeva - * i would try to use both irssi and weechat *

Sources:
    1. https://www.rfc-editor.org/rfc/rfc1459.html#section-4

# Quick start

To run the server:  `./ircserv <port> <pw>`

Connect to the server using **irssi**:
1. start irssi in terminal
2. `/CONNECT 127.0.0.1 <port> <pw>`

Connect to the server using **nc**:
1. open terminal
2. run: `nc -C 127.0.0.1 <port>`