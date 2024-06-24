# things that have to get fixed before monday

- [ ] more documentation in header files (for both client and stm code)
- [ ] design/architecture document
- [x] more tests in the test document
- [x] handle errors from `AT+CIPSEND`, these look like this:
      ```
      > AT0,CONNECT

      > +IPD,0,15:last-records 5<0a>
      < AT+CIPSEND=0,125
      > AT+CIPSEND=0,125

      > ERROR
      ```

## `// TODO:`'s

- [x] `sensor.c:24:    return (uint8_t) temp_c; //TODO: convert with range -> util.h`
- [x] `sensor.c:36:    return (uint8_t) humidity; //TODO: convert with range -> util.h`
- [x] `sensor.c:51:    return (uint8_t) val; // TODO: convert with range`
- [x] `server.c:47:// TODO: next_few_bytes_are assumes that the complete search string is in the`
- [x] `server.c:146:   // TODO: buffer overrun protection`
- [x] `server.c:152:// TODO: refactor this`
- [x] `server.c:165:// TODO: refactor this`
- [x] `server.c:174:// TODO: refactor this`
- [x] `setup.c:95:     // TODO: remove debug size`
- [x] `setup.c:187:            .Pin = GPIO_PIN_8|GPIO_PIN_9, //TODO: use #defines in setup.h`
