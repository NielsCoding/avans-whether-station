# Protocol spec

This is a brief overview of the protocol specifications that the weather
station uses to send and receive data between the weather station and qt
client. This protocol is text-based, and used over a TCP connection. This
document will only go into detail about the data sent over this connection, not
requirements about the connection itself.

The protocol is only used in a request-response fashion, so all commands are
assumed to be sent by the qt client, and responded to by the weather station.

~Functions for generating commands and parsing incoming data are provided by
the protocol.c and protocol.h files in this folder.~ A server using these files
should implement every protocol handler function in a seperate c file, along
with a data sending function that is also used internally.

- LF for newline instead of CRLF
- Commands are single-line
- Spaces used for separating command arguments
- Commands with malformed data are discarded and return error
- Response consist of `ok` or `error`, a comma, and the byte length of the
  remaining response (if any)
- Numbers are sent as hexadecimal

## Commands

### `last-records <n> <o>`

Returns the last `n` records with offset `<o>` in csv format. The first line
has the csv table header, with the fields `id`, `temperature`, `humidity`, and
`atmospheric_pressure`. The rest of the response consists of 1 record per line.
The amount of records is limited to the amount of valid records in the backlog
buffer. When the amount of returned records is 0, the response consists of the
csv header, but without any following records.

Offset `<o>` is a positive integer, representing the starting point for the
most recent record that is returned, this will get subtracted from the id of
the most recent record. E.g. if the last record has id `00f0`, and a request is
sent with parameters `n=3` and `o=5`, the records with id's `00eb`, `00ea`, and
`00e9` will be returned.

## Example transaction

In the following example, newlines are indicated by `<0a>`, request by lines
starting with `<`, and response by lines starting with `>`.

```
< last-records 5 0<0a>
> ok,73<0a>
> id,temperature,humidity,atmospheric_pressure<0a>
> 10dc,2f,c5,7f<0a>
> 10dd,30,c6,7f<0a>
> 10de,31,c7,7f<0a>
> 10df,35,ca,7e<0a>
> 10e0,34,c9,7e<0a>
```

