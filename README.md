# daemon

simple daemon program using file locks, listening on a port and loging mesages received with `syslog`

## Example usage

```sh
$ ps aux | grep '[m]att-daemon'
$ ./matt-daemon 
$ ps aux | grep '[m]att-daemon'
root      224397  0.0  0.0   5776   188 ?        Ss   20:40   0:00 ./matt-daemon
$ netstat -an | grep 4242
tcp        0      0 0.0.0.0:4242            0.0.0.0:*               LISTEN     
$ nc localhost 4242
well hello there
greetings, mister daemon
$ nc localhost 4242
another connection?
^Z
[1]+  Stopped                 nc localhost 4242
$ nc localhost 4242
two AT ONCE???????
$ fg
nc localhost 4242
quit
$ ps aux | grep '[m]att-daemon'
$ journalctl --reverse | head -n50 | grep 'matt-daemon' | tac
Apr 16 20:40:57 oogabooga matt-daemon[224397]: Daemon running
Apr 16 20:42:13 oogabooga matt-daemon[224397]: Message received: well hello there
Apr 16 20:42:25 oogabooga matt-daemon[224397]: Message received: greetings, mister daemon
Apr 16 20:42:35 oogabooga matt-daemon[224397]: Message received: another connection?
Apr 16 20:42:47 oogabooga matt-daemon[224397]: Message received: two AT ONCE???????
Apr 16 20:42:52 oogabooga matt-daemon[224397]: Message received: quit
Apr 16 20:42:52 oogabooga matt-daemon[224397]: Shutting down daemon
$ ./matt-daemon 
$ killall matt-daemon
$ journalctl --reverse | head -n50 | grep 'matt-daemon' | tac
Apr 16 20:42:13 oogabooga matt-daemon[224397]: Message received: well hello there
Apr 16 20:42:25 oogabooga matt-daemon[224397]: Message received: greetings, mister daemon
Apr 16 20:42:35 oogabooga matt-daemon[224397]: Message received: another connection?
Apr 16 20:42:47 oogabooga matt-daemon[224397]: Message received: two AT ONCE???????
Apr 16 20:42:52 oogabooga matt-daemon[224397]: Message received: quit
Apr 16 20:42:52 oogabooga matt-daemon[224397]: Shutting down daemon
Apr 16 20:43:09 oogabooga matt-daemon[224603]: Daemon running
Apr 16 20:43:11 oogabooga matt-daemon[224603]: Received shutdown signal
Apr 16 20:43:11 oogabooga matt-daemon[224603]: Shutting down daemon
$ 
```
