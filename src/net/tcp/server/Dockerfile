FROM prantlf/alpine-make-gcc AS builder

COPY ./fileserver /usr/src/fileserver-dir

WORKDIR /usr/src/fileserver-dir

RUN make

FROM alpine

WORKDIR /usr/src/fileserver-dir

COPY --from=builder /usr/src/fileserver-dir/fileserver /bin/fileserver

VOLUME ["/fileserver"]

EXPOSE 22007

CMD ["/bin/fileserver", "22007", "&"]