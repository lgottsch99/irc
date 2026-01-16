When connecting to the server, getting this response:
```
16:20 -!- Irssi: Looking up 127.0.0.1
16:20 -!- Irssi: Connecting to 127.0.0.1 [127.0.0.1] port 1111
16:20 -!- Irssi: Connection to 127.0.0.1 established
16:20 -!- LS Unknown command // <- is this a client thing or do we send this?
16:20 -!- kata kata 127.0.0.1 Welcome to the server!
16:20 -!- kata: No such channel // <- is this a client thing or do we send this? Seems like the client (irssi) sends a whole bunch of commands when registering and we do not handle all of them
```


When joining an existing channel, the nc gets:
```
:ft_irc 331 kata2 #cha2 :No topic is set 
:ft_irc 353 kata2 #cha2 **:nicknames** // <- should the real nicknames go here?
:ft_irc 366 kata2 #cha2 :End of /NAMES list 
```


KICK
Should we allow kicking yourself from a channel?

What do we do when the only operator leaves the channel?

There is a mode query (not required by the subject):
`MODE #chan` Should we support this? Expected:

- return channel/user modes
- no parsing of mode string

These are probably errors from the previous version:

JOIN (to create a channel)
- only happens in WSL, works correctly on school pc
When creating a new channel for the first time, the nc gets the following response:
:ft_irc 403 * #cha :No such channel 
This does not happen for the second time or when creating another new channel. The channel is created correctly and the user is added to it.
It is similar with the irssi, just the client does not show the error number.

Channel mode +l
I could not reproduce this behavior: When trying to add a user to a full channel, there is no error message (behaved correctly when the server was run for the second time)